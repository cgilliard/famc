//===- GVN.cpp - Eliminate redundant values and loads ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass performs global value numbering to eliminate fully redundant
// instructions.  It also performs simple dead load elimination.
//
// Note that this pass does the value numbering itself; it does not use the
// ValueNumbering analysis passes.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/Loads.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/OptimizationDiagnosticInfo.h"
#include "llvm/Analysis/PHITransAddr.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/SSAUpdater.h"
#include <vector>
using namespace llvm;
using namespace llvm::gvn;
using namespace PatternMatch;

#define DEBUG_TYPE "gvn"

STATISTIC(NumGVNInstr,  "Number of instructions deleted");
STATISTIC(NumGVNLoad,   "Number of loads deleted");
STATISTIC(NumGVNPRE,    "Number of instructions PRE'd");
STATISTIC(NumGVNBlocks, "Number of blocks merged");
STATISTIC(NumGVNSimpl,  "Number of instructions simplified");
STATISTIC(NumGVNEqProp, "Number of equalities propagated");
STATISTIC(NumPRELoad,   "Number of loads PRE'd");

static cl::opt<bool> EnablePRE("enable-pre",
                               cl::init(true), cl::Hidden);
static cl::opt<bool> EnableLoadPRE("enable-load-pre", cl::init(true));

// Maximum allowed recursion depth.
static cl::opt<uint32_t>
MaxRecurseDepth("max-recurse-depth", cl::Hidden, cl::init(1000), cl::ZeroOrMore,
                cl::desc("Max recurse depth (default = 1000)"));

struct llvm::GVN::Expression {
  uint32_t opcode;
  Type *type;
  SmallVector<uint32_t, 4> varargs;

  Expression(uint32_t o = ~2U) : opcode(o) {}

  bool operator==(const Expression &other) const {
    if (opcode != other.opcode)
      return false;
    if (opcode == ~0U || opcode == ~1U)
      return true;
    if (type != other.type)
      return false;
    if (varargs != other.varargs)
      return false;
    return true;
  }

  friend hash_code hash_value(const Expression &Value) {
    return hash_combine(
        Value.opcode, Value.type,
        hash_combine_range(Value.varargs.begin(), Value.varargs.end()));
  }
};

namespace llvm {
template <> struct DenseMapInfo<GVN::Expression> {
  static inline GVN::Expression getEmptyKey() { return ~0U; }

  static inline GVN::Expression getTombstoneKey() { return ~1U; }

  static unsigned getHashValue(const GVN::Expression &e) {
    using llvm::hash_value;
    return static_cast<unsigned>(hash_value(e));
  }
  static bool isEqual(const GVN::Expression &LHS, const GVN::Expression &RHS) {
    return LHS == RHS;
  }
};
} // End llvm namespace.

/// Represents a particular available value that we know how to materialize.
/// Materialization of an AvailableValue never fails.  An AvailableValue is
/// implicitly associated with a rematerialization point which is the
/// location of the instruction from which it was formed.
struct llvm::gvn::AvailableValue {
  enum ValType {
    SimpleVal, // A simple offsetted value that is accessed.
    LoadVal,   // A value produced by a load.
    MemIntrin, // A memory intrinsic which is loaded from.
    UndefVal   // A UndefValue representing a value from dead block (which
               // is not yet physically removed from the CFG).
  };

  /// V - The value that is live out of the block.
  PointerIntPair<Value *, 2, ValType> Val;

  /// Offset - The byte offset in Val that is interesting for the load query.
  unsigned Offset;

  static AvailableValue get(Value *V, unsigned Offset = 0) {
    AvailableValue Res;
    Res.Val.setPointer(V);
    Res.Val.setInt(SimpleVal);
    Res.Offset = Offset;
    return Res;
  }

  static AvailableValue getMI(MemIntrinsic *MI, unsigned Offset = 0) {
    AvailableValue Res;
    Res.Val.setPointer(MI);
    Res.Val.setInt(MemIntrin);
    Res.Offset = Offset;
    return Res;
  }

  static AvailableValue getLoad(LoadInst *LI, unsigned Offset = 0) {
    AvailableValue Res;
    Res.Val.setPointer(LI);
    Res.Val.setInt(LoadVal);
    Res.Offset = Offset;
    return Res;
  }

  static AvailableValue getUndef() {
    AvailableValue Res;
    Res.Val.setPointer(nullptr);
    Res.Val.setInt(UndefVal);
    Res.Offset = 0;
    return Res;
  }

  bool isSimpleValue() const { return Val.getInt() == SimpleVal; }
  bool isCoercedLoadValue() const { return Val.getInt() == LoadVal; }
  bool isMemIntrinValue() const { return Val.getInt() == MemIntrin; }
  bool isUndefValue() const { return Val.getInt() == UndefVal; }

  Value *getSimpleValue() const {
    assert(isSimpleValue() && "Wrong accessor");
    return Val.getPointer();
  }

  LoadInst *getCoercedLoadValue() const {
    assert(isCoercedLoadValue() && "Wrong accessor");
    return cast<LoadInst>(Val.getPointer());
  }

  MemIntrinsic *getMemIntrinValue() const {
    assert(isMemIntrinValue() && "Wrong accessor");
    return cast<MemIntrinsic>(Val.getPointer());
  }

  /// Emit code at the specified insertion point to adjust the value defined
  /// here to the specified type. This handles various coercion cases.
  Value *MaterializeAdjustedValue(LoadInst *LI, Instruction *InsertPt,
                                  GVN &gvn) const;
};

/// Represents an AvailableValue which can be rematerialized at the end of
/// the associated BasicBlock.
struct llvm::gvn::AvailableValueInBlock {
  /// BB - The basic block in question.
  BasicBlock *BB;

  /// AV - The actual available value
  AvailableValue AV;

  static AvailableValueInBlock get(BasicBlock *BB, AvailableValue &&AV) {
    AvailableValueInBlock Res;
    Res.BB = BB;
    Res.AV = std::move(AV);
    return Res;
  }

  static AvailableValueInBlock get(BasicBlock *BB, Value *V,
                                   unsigned Offset = 0) {
    return get(BB, AvailableValue::get(V, Offset));
  }
  static AvailableValueInBlock getUndef(BasicBlock *BB) {
    return get(BB, AvailableValue::getUndef());
  }

  /// Emit code at the end of this block to adjust the value defined here to
  /// the specified type. This handles various coercion cases.
  Value *MaterializeAdjustedValue(LoadInst *LI, GVN &gvn) const {
    return AV.MaterializeAdjustedValue(LI, BB->getTerminator(), gvn);
  }
};

//===----------------------------------------------------------------------===//
//                     ValueTable Internal Functions
//===----------------------------------------------------------------------===//

GVN::Expression GVN::ValueTable::createExpr(Instruction *I) {
  Expression e;
  e.type = I->getType();
  e.opcode = I->getOpcode();
  for (Instruction::op_iterator OI = I->op_begin(), OE = I->op_end();
       OI != OE; ++OI)
    e.varargs.push_back(lookupOrAdd(*OI));
  if (I->isCommutative()) {
    // Ensure that commutative instructions that only differ by a permutation
    // of their operands get the same value number by sorting the operand value
    // numbers.  Since all commutative instructions have two operands it is more
    // efficient to sort by hand rather than using, say, std::sort.
    assert(I->getNumOperands() == 2 && "Unsupported commutative instruction!");
    if (e.varargs[0] > e.varargs[1])
      std::swap(e.varargs[0], e.varargs[1]);
  }

  if (CmpInst *C = dyn_cast<CmpInst>(I)) {
    // Sort the operand value numbers so x<y and y>x get the same value number.
    CmpInst::Predicate Predicate = C->getPredicate();
    if (e.varargs[0] > e.varargs[1]) {
      std::swap(e.varargs[0], e.varargs[1]);
      Predicate = CmpInst::getSwappedPredicate(Predicate);
    }
    e.opcode = (C->getOpcode() << 8) | Predicate;
  } else if (InsertValueInst *E = dyn_cast<InsertValueInst>(I)) {
    for (InsertValueInst::idx_iterator II = E->idx_begin(), IE = E->idx_end();
         II != IE; ++II)
      e.varargs.push_back(*II);
  }

  return e;
}

GVN::Expression GVN::ValueTable::createCmpExpr(unsigned Opcode,
                                               CmpInst::Predicate Predicate,
                                               Value *LHS, Value *RHS) {
  assert((Opcode == Instruction::ICmp || Opcode == Instruction::FCmp) &&
         "Not a comparison!");
  Expression e;
  e.type = CmpInst::makeCmpResultType(LHS->getType());
  e.varargs.push_back(lookupOrAdd(LHS));
  e.varargs.push_back(lookupOrAdd(RHS));

  // Sort the operand value numbers so x<y and y>x get the same value number.
  if (e.varargs[0] > e.varargs[1]) {
    std::swap(e.varargs[0], e.varargs[1]);
    Predicate = CmpInst::getSwappedPredicate(Predicate);
  }
  e.opcode = (Opcode << 8) | Predicate;
  return e;
}

GVN::Expression GVN::ValueTable::createExtractvalueExpr(ExtractValueInst *EI) {
  assert(EI && "Not an ExtractValueInst?");
  Expression e;
  e.type = EI->getType();
  e.opcode = 0;

  IntrinsicInst *I = dyn_cast<IntrinsicInst>(EI->getAggregateOperand());
  if (I != nullptr && EI->getNumIndices() == 1 && *EI->idx_begin() == 0 ) {
    // EI might be an extract from one of our recognised intrinsics. If it
    // is we'll synthesize a semantically equivalent expression instead on
    // an extract value expression.
    switch (I->getIntrinsicID()) {
      case Intrinsic::sadd_with_overflow:
      case Intrinsic::uadd_with_overflow:
        e.opcode = Instruction::Add;
        break;
      case Intrinsic::ssub_with_overflow:
      case Intrinsic::usub_with_overflow:
        e.opcode = Instruction::Sub;
        break;
      case Intrinsic::smul_with_overflow:
      case Intrinsic::umul_with_overflow:
        e.opcode = Instruction::Mul;
        break;
      default:
        break;
    }

    if (e.opcode != 0) {
      // Intrinsic recognized. Grab its args to finish building the expression.
      assert(I->getNumArgOperands() == 2 &&
             "Expect two args for recognised intrinsics.");
      e.varargs.push_back(lookupOrAdd(I->getArgOperand(0)));
      e.varargs.push_back(lookupOrAdd(I->getArgOperand(1)));
      return e;
    }
  }

  // Not a recognised intrinsic. Fall back to producing an extract value
  // expression.
  e.opcode = EI->getOpcode();
  for (Instruction::op_iterator OI = EI->op_begin(), OE = EI->op_end();
       OI != OE; ++OI)
    e.varargs.push_back(lookupOrAdd(*OI));

  for (ExtractValueInst::idx_iterator II = EI->idx_begin(), IE = EI->idx_end();
         II != IE; ++II)
    e.varargs.push_back(*II);

  return e;
}

//===----------------------------------------------------------------------===//
//                     ValueTable External Functions
//===----------------------------------------------------------------------===//

GVN::ValueTable::ValueTable() : nextValueNumber(1) {}
GVN::ValueTable::ValueTable(const ValueTable &) = default;
GVN::ValueTable::ValueTable(ValueTable &&) = default;
GVN::ValueTable::~ValueTable() = default;

/// add - Insert a value into the table with a specified value number.
void GVN::ValueTable::add(Value *V, uint32_t num) {
  valueNumbering.insert(std::make_pair(V, num));
}

uint32_t GVN::ValueTable::lookupOrAddCall(CallInst *C) {
  if (AA->doesNotAccessMemory(C)) {
    Expression exp = createExpr(C);
    uint32_t &e = expressionNumbering[exp];
    if (!e) e = nextValueNumber++;
    valueNumbering[C] = e;
    return e;
  } else if (AA->onlyReadsMemory(C)) {
    Expression exp = createExpr(C);
    uint32_t &e = expressionNumbering[exp];
    if (!e) {
      e = nextValueNumber++;
      valueNumbering[C] = e;
      return e;
    }
    if (!MD) {
      e = nextValueNumber++;
      valueNumbering[C] = e;
      return e;
    }

    MemDepResult local_dep = MD->getDependency(C);

    if (!local_dep.isDef() && !local_dep.isNonLocal()) {
      valueNumbering[C] =  nextValueNumber;
      return nextValueNumber++;
    }

    if (local_dep.isDef()) {
      CallInst* local_cdep = cast<CallInst>(local_dep.getInst());

      if (local_cdep->getNumArgOperands() != C->getNumArgOperands()) {
        valueNumbering[C] = nextValueNumber;
        return nextValueNumber++;
      }

      for (unsigned i = 0, e = C->getNumArgOperands(); i < e; ++i) {
        uint32_t c_vn = lookupOrAdd(C->getArgOperand(i));
        uint32_t cd_vn = lookupOrAdd(local_cdep->getArgOperand(i));
        if (c_vn != cd_vn) {
          valueNumbering[C] = nextValueNumber;
          return nextValueNumber++;
        }
      }

      uint32_t v = lookupOrAdd(local_cdep);
      valueNumbering[C] = v;
      return v;
    }

    // Non-local case.
    const MemoryDependenceResults::NonLocalDepInfo &deps =
      MD->getNonLocalCallDependency(CallSite(C));
    // FIXME: Move the checking logic to MemDep!
    CallInst* cdep = nullptr;

    // Check to see if we have a single dominating call instruction that is
    // identical to C.
    for (unsigned i = 0, e = deps.size(); i != e; ++i) {
      const NonLocalDepEntry *I = &deps[i];
      if (I->getResult().isNonLocal())
        continue;

      // We don't handle non-definitions.  If we already have a call, reject
      // instruction dependencies.
      if (!I->getResult().isDef() || cdep != nullptr) {
        cdep = nullptr;
        break;
      }

      CallInst *NonLocalDepCall = dyn_cast<CallInst>(I->getResult().getInst());
      // FIXME: All duplicated with non-local case.
      if (NonLocalDepCall && DT->properlyDominates(I->getBB(), C->getParent())){
        cdep = NonLocalDepCall;
        continue;
      }

      cdep = nullptr;
      break;
    }

    if (!cdep) {
      valueNumbering[C] = nextValueNumber;
      return nextValueNumber++;
    }

    if (cdep->getNumArgOperands() != C->getNumArgOperands()) {
      valueNumbering[C] = nextValueNumber;
      return nextValueNumber++;
    }
    for (unsigned i = 0, e = C->getNumArgOperands(); i < e; ++i) {
      uint32_t c_vn = lookupOrAdd(C->getArgOperand(i));
      uint32_t cd_vn = lookupOrAdd(cdep->getArgOperand(i));
      if (c_vn != cd_vn) {
        valueNumbering[C] = nextValueNumber;
        return nextValueNumber++;
      }
    }

    uint32_t v = lookupOrAdd(cdep);
    valueNumbering[C] = v;
    return v;

  } else {
    valueNumbering[C] = nextValueNumber;
    return nextValueNumber++;
  }
}

/// Returns true if a value number exists for the specified value.
bool GVN::ValueTable::exists(Value *V) const { return valueNumbering.count(V) != 0; }

/// lookup_or_add - Returns the value number for the specified value, assigning
/// it a new number if it did not have one before.
uint32_t GVN::ValueTable::lookupOrAdd(Value *V) {
  DenseMap<Value*, uint32_t>::iterator VI = valueNumbering.find(V);
  if (VI != valueNumbering.end())
    return VI->second;

  if (!isa<Instruction>(V)) {
    valueNumbering[V] = nextValueNumber;
    return nextValueNumber++;
  }

  Instruction* I = cast<Instruction>(V);
  Expression exp;
  switch (I->getOpcode()) {
    case Instruction::Call:
      return lookupOrAddCall(cast<CallInst>(I));
    case Instruction::Add:
    case Instruction::FAdd:
    case Instruction::Sub:
    case Instruction::FSub:
    case Instruction::Mul:
    case Instruction::FMul:
    case Instruction::UDiv:
    case Instruction::SDiv:
    case Instruction::FDiv:
    case Instruction::URem:
    case Instruction::SRem:
    case Instruction::FRem:
    case Instruction::Shl:
    case Instruction::LShr:
    case Instruction::AShr:
    case Instruction::And:
    case Instruction::Or:
    case Instruction::Xor:
    case Instruction::ICmp:
    case Instruction::FCmp:
    case Instruction::Trunc:
    case Instruction::ZExt:
    case Instruction::SExt:
    case Instruction::FPToUI:
    case Instruction::FPToSI:
    case Instruction::UIToFP:
    case Instruction::SIToFP:
    case Instruction::FPTrunc:
    case Instruction::FPExt:
    case Instruction::PtrToInt:
    case Instruction::IntToPtr:
    case Instruction::BitCast:
    case Instruction::Select:
    case Instruction::ExtractElement:
    case Instruction::InsertElement:
    case Instruction::ShuffleVector:
    case Instruction::InsertValue:
    case Instruction::GetElementPtr:
      exp = createExpr(I);
      break;
    case Instruction::ExtractValue:
      exp = createExtractvalueExpr(cast<ExtractValueInst>(I));
      break;
    default:
      valueNumbering[V] = nextValueNumber;
      return nextValueNumber++;
  }

  uint32_t& e = expressionNumbering[exp];
  if (!e) e = nextValueNumber++;
  valueNumbering[V] = e;
  return e;
}

/// Returns the value number of the specified value. Fails if
/// the value has not yet been numbered.
uint32_t GVN::ValueTable::lookup(Value *V) const {
  DenseMap<Value*, uint32_t>::const_iterator VI = valueNumbering.find(V);
  assert(VI != valueNumbering.end() && "Value not numbered?");
  return VI->second;
}

/// Returns the value number of the given comparison,
/// assigning it a new number if it did not have one before.  Useful when
/// we deduced the result of a comparison, but don't immediately have an
/// instruction realizing that comparison to hand.
uint32_t GVN::ValueTable::lookupOrAddCmp(unsigned Opcode,
                                         CmpInst::Predicate Predicate,
                                         Value *LHS, Value *RHS) {
  Expression exp = createCmpExpr(Opcode, Predicate, LHS, RHS);
  uint32_t& e = expressionNumbering[exp];
  if (!e) e = nextValueNumber++;
  return e;
}

/// Remove all entries from the ValueTable.
void GVN::ValueTable::clear() {
  valueNumbering.clear();
  expressionNumbering.clear();
  nextValueNumber = 1;
}

/// Remove a value from the value numbering.
void GVN::ValueTable::erase(Value *V) {
  valueNumbering.erase(V);
}

/// verifyRemoved - Verify that the value is removed from all internal data
/// structures.
void GVN::ValueTable::verifyRemoved(const Value *V) const {
  for (DenseMap<Value*, uint32_t>::const_iterator
         I = valueNumbering.begin(), E = valueNumbering.end(); I != E; ++I) {
    assert(I->first != V && "Inst still occurs in value numbering map!");
  }
}

//===----------------------------------------------------------------------===//
//                                GVN Pass
//===----------------------------------------------------------------------===//

PreservedAnalyses GVN::run(Function &F, FunctionAnalysisManager &AM) {
  // FIXME: The order of evaluation of these 'getResult' calls is very
  // significant! Re-ordering these variables will cause GVN when run alone to
  // be less effective! We should fix memdep and basic-aa to not exhibit this
  // behavior, but until then don't change the order here.
  auto &AC = AM.getResult<AssumptionAnalysis>(F);
  auto &DT = AM.getResult<DominatorTreeAnalysis>(F);
  auto &TLI = AM.getResult<TargetLibraryAnalysis>(F);
  auto &AA = AM.getResult<AAManager>(F);
  auto &MemDep = AM.getResult<MemoryDependenceAnalysis>(F);
  auto *LI = AM.getCachedResult<LoopAnalysis>(F);
  auto &ORE = AM.getResult<OptimizationRemarkEmitterAnalysis>(F);
  bool Changed = runImpl(F, AC, DT, TLI, AA, &MemDep, LI, &ORE);
  if (!Changed)
    return PreservedAnalyses::all();
  PreservedAnalyses PA;
  PA.preserve<DominatorTreeAnalysis>();
  PA.preserve<GlobalsAA>();
  return PA;
}

LLVM_DUMP_METHOD
void GVN::dump(DenseMap<uint32_t, Value*>& d) {
  errs() << "{\n";
  for (DenseMap<uint32_t, Value*>::iterator I = d.begin(),
       E = d.end(); I != E; ++I) {
      errs() << I->first << "\n";
      I->second->dump();
  }
  errs() << "}\n";
}

/// Return true if we can prove that the value
/// we're analyzing is fully available in the specified block.  As we go, keep
/// track of which blocks we know are fully alive in FullyAvailableBlocks.  This
/// map is actually a tri-state map with the following values:
///   0) we know the block *is not* fully available.
///   1) we know the block *is* fully available.
///   2) we do not know whether the block is fully available or not, but we are
///      currently speculating that it will be.
///   3) we are speculating for this block and have used that to speculate for
///      other blocks.
static bool IsValueFullyAvailableInBlock(BasicBlock *BB,
                            DenseMap<BasicBlock*, char> &FullyAvailableBlocks,
                            uint32_t RecurseDepth) {
  if (RecurseDepth > MaxRecurseDepth)
    return false;

  // Optimistically assume that the block is fully available and check to see
  // if we already know about this block in one lookup.
  std::pair<DenseMap<BasicBlock*, char>::iterator, char> IV =
    FullyAvailableBlocks.insert(std::make_pair(BB, 2));

  // If the entry already existed for this block, return the precomputed value.
  if (!IV.second) {
    // If this is a speculative "available" value, mark it as being used for
    // speculation of other blocks.
    if (IV.first->second == 2)
      IV.first->second = 3;
    return IV.first->second != 0;
  }

  // Otherwise, see if it is fully available in all predecessors.
  pred_iterator PI = pred_begin(BB), PE = pred_end(BB);

  // If this block has no predecessors, it isn't live-in here.
  if (PI == PE)
    goto SpeculationFailure;

  for (; PI != PE; ++PI)
    // If the value isn't fully available in one of our predecessors, then it
    // isn't fully available in this block either.  Undo our previous
    // optimistic assumption and bail out.
    if (!IsValueFullyAvailableInBlock(*PI, FullyAvailableBlocks,RecurseDepth+1))
      goto SpeculationFailure;

  return true;

// If we get here, we found out that this is not, after
// all, a fully-available block.  We have a problem if we speculated on this and
// used the speculation to mark other blocks as available.
SpeculationFailure:
  char &BBVal = FullyAvailableBlocks[BB];

  // If we didn't speculate on this, just return with it set to false.
  if (BBVal == 2) {
    BBVal = 0;
    return false;
  }

  // If we did speculate on this value, we could have blocks set to 1 that are
  // incorrect.  Walk the (transitive) successors of this block and mark them as
  // 0 if set to one.
  SmallVector<BasicBlock*, 32> BBWorklist;
  BBWorklist.push_back(BB);

  do {
    BasicBlock *Entry = BBWorklist.pop_back_val();
    // Note that this sets blocks to 0 (unavailable) if they happen to not
    // already be in FullyAvailableBlocks.  This is safe.
    char &EntryVal = FullyAvailableBlocks[Entry];
    if (EntryVal == 0) continue;  // Already unavailable.

    // Mark as unavailable.
    EntryVal = 0;

    BBWorklist.append(succ_begin(Entry), succ_end(Entry));
  } while (!BBWorklist.empty());

  return false;
}


/// Return true if CoerceAvailableValueToLoadType will succeed.
static bool CanCoerceMustAliasedValueToLoad(Value *StoredVal,
                                            Type *LoadTy,
                                            const DataLayout &DL) {
  // If the loaded or stored value is an first class array or struct, don't try
  // to transform them.  We need to be able to bitcast to integer.
  if (LoadTy->isStructTy() || LoadTy->isArrayTy() ||
      StoredVal->getType()->isStructTy() ||
      StoredVal->getType()->isArrayTy())
    return false;

  // The store has to be at least as big as the load.
  if (DL.getTypeSizeInBits(StoredVal->getType()) <
        DL.getTypeSizeInBits(LoadTy))
    return false;

  return true;
}

/// If we saw a store of a value to memory, and
/// then a load from a must-aliased pointer of a different type, try to coerce
/// the stored value.  LoadedTy is the type of the load we want to replace.
/// IRB is IRBuilder used to insert new instructions.
///
/// If we can't do it, return null.
static Value *CoerceAvailableValueToLoadType(Value *StoredVal, Type *LoadedTy,
                                             IRBuilder<> &IRB,
                                             const DataLayout &DL) {
  assert(CanCoerceMustAliasedValueToLoad(StoredVal, LoadedTy, DL) &&
         "precondition violation - materialization can't fail");

  if (auto *C = dyn_cast<Constant>(StoredVal))
    if (auto *FoldedStoredVal = ConstantFoldConstant(C, DL))
      StoredVal = FoldedStoredVal;

  // If this is already the right type, just return it.
  Type *StoredValTy = StoredVal->getType();

  uint64_t StoredValSize = DL.getTypeSizeInBits(StoredValTy);
  uint64_t LoadedValSize = DL.getTypeSizeInBits(LoadedTy);

  // If the store and reload are the same size, we can always reuse it.
  if (StoredValSize == LoadedValSize) {
    // Pointer to Pointer -> use bitcast.
    if (StoredValTy->getScalarType()->isPointerTy() &&
        LoadedTy->getScalarType()->isPointerTy()) {
      StoredVal = IRB.CreateBitCast(StoredVal, LoadedTy);
    } else {
      // Convert source pointers to integers, which can be bitcast.
      if (StoredValTy->getScalarType()->isPointerTy()) {
        StoredValTy = DL.getIntPtrType(StoredValTy);
        StoredVal = IRB.CreatePtrToInt(StoredVal, StoredValTy);
      }

      Type *TypeToCastTo = LoadedTy;
      if (TypeToCastTo->getScalarType()->isPointerTy())
        TypeToCastTo = DL.getIntPtrType(TypeToCastTo);

      if (StoredValTy != TypeToCastTo)
        StoredVal = IRB.CreateBitCast(StoredVal, TypeToCastTo);

      // Cast to pointer if the load needs a pointer type.
      if (LoadedTy->getScalarType()->isPointerTy())
        StoredVal = IRB.CreateIntToPtr(StoredVal, LoadedTy);
    }

    if (auto *C = dyn_cast<ConstantExpr>(StoredVal))
      if (auto *FoldedStoredVal = ConstantFoldConstant(C, DL))
        StoredVal = FoldedStoredVal;

    return StoredVal;
  }

  // If the loaded value is smaller than the available value, then we can
  // extract out a piece from it.  If the available value is too small, then we
  // can't do anything.
  assert(StoredValSize >= LoadedValSize &&
         "CanCoerceMustAliasedValueToLoad fail");

  // Convert source pointers to integers, which can be manipulated.
  if (StoredValTy->getScalarType()->isPointerTy()) {
    StoredValTy = DL.getIntPtrType(StoredValTy);
    StoredVal = IRB.CreatePtrToInt(StoredVal, StoredValTy);
  }

  // Convert vectors and fp to integer, which can be manipulated.
  if (!StoredValTy->isIntegerTy()) {
    StoredValTy = IntegerType::get(StoredValTy->getContext(), StoredValSize);
    StoredVal = IRB.CreateBitCast(StoredVal, StoredValTy);
  }

  // If this is a big-endian system, we need to shift the value down to the low
  // bits so that a truncate will work.
  if (DL.isBigEndian()) {
    uint64_t ShiftAmt = DL.getTypeStoreSizeInBits(StoredValTy) -
                        DL.getTypeStoreSizeInBits(LoadedTy);
    StoredVal = IRB.CreateLShr(StoredVal, ShiftAmt, "tmp");
  }

  // Truncate the integer to the right size now.
  Type *NewIntTy = IntegerType::get(StoredValTy->getContext(), LoadedValSize);
  StoredVal  = IRB.CreateTrunc(StoredVal, NewIntTy, "trunc");

  if (LoadedTy != NewIntTy) {
    // If the result is a pointer, inttoptr.
    if (LoadedTy->getScalarType()->isPointerTy())
      StoredVal = IRB.CreateIntToPtr(StoredVal, LoadedTy, "inttoptr");
    else
      // Otherwise, bitcast.
      StoredVal = IRB.CreateBitCast(StoredVal, LoadedTy, "bitcast");
  }

  if (auto *C = dyn_cast<Constant>(StoredVal))
    if (auto *FoldedStoredVal = ConstantFoldConstant(C, DL))
      StoredVal = FoldedStoredVal;

  return StoredVal;
}

/// This function is called when we have a
/// memdep query of a load that ends up being a clobbering memory write (store,
/// memset, memcpy, memmove).  This means that the write *may* provide bits used
/// by the load but we can't be sure because the pointers don't mustalias.
///
/// Check this case to see if there is anything more we can do before we give
/// up.  This returns -1 if we have to give up, or a byte number in the stored
/// value of the piece that feeds the load.
static int AnalyzeLoadFromClobberingWrite(Type *LoadTy, Value *LoadPtr,
                                          Value *WritePtr,
                                          uint64_t WriteSizeInBits,
                                          const DataLayout &DL) {
  // If the loaded or stored value is a first class array or struct, don't try
  // to transform them.  We need to be able to bitcast to integer.
  if (LoadTy->isStructTy() || LoadTy->isArrayTy())
    return -1;

  int64_t StoreOffset = 0, LoadOffset = 0;
  Value *StoreBase =
      GetPointerBaseWithConstantOffset(WritePtr, StoreOffset, DL);
  Value *LoadBase = GetPointerBaseWithConstantOffset(LoadPtr, LoadOffset, DL);
  if (StoreBase != LoadBase)
    return -1;

  // If the load and store are to the exact same address, they should have been
  // a must alias.  AA must have gotten confused.
  // FIXME: Study to see if/when this happens.  One case is forwarding a memset
  // to a load from the base of the memset.

  // If the load and store don't overlap at all, the store doesn't provide
  // anything to the load.  In this case, they really don't alias at all, AA
  // must have gotten confused.
  uint64_t LoadSize = DL.getTypeSizeInBits(LoadTy);

  if ((WriteSizeInBits & 7) | (LoadSize & 7))
    return -1;
  uint64_t StoreSize = WriteSizeInBits / 8;  // Convert to bytes.
  LoadSize /= 8;


  bool isAAFailure = false;
  if (StoreOffset < LoadOffset)
    isAAFailure = StoreOffset+int64_t(StoreSize) <= LoadOffset;
  else
    isAAFailure = LoadOffset+int64_t(LoadSize) <= StoreOffset;

  if (isAAFailure)
    return -1;

  // If the Load isn't completely contained within the stored bits, we don't
  // have all the bits to feed it.  We could do something crazy in the future
  // (issue a smaller load then merge the bits in) but this seems unlikely to be
  // valuable.
  if (StoreOffset > LoadOffset ||
      StoreOffset+StoreSize < LoadOffset+LoadSize)
    return -1;

  // Okay, we can do this transformation.  Return the number of bytes into the
  // store that the load is.
  return LoadOffset-StoreOffset;
}

/// This function is called when we have a
/// memdep query of a load that ends up being a clobbering store.
static int AnalyzeLoadFromClobberingStore(Type *LoadTy, Value *LoadPtr,
                                          StoreInst *DepSI) {
  // Cannot handle reading from store of first-class aggregate yet.
  if (DepSI->getValueOperand()->getType()->isStructTy() ||
      DepSI->getValueOperand()->getType()->isArrayTy())
    return -1;

  const DataLayout &DL = DepSI->getModule()->getDataLayout();
  Value *StorePtr = DepSI->getPointerOperand();
  uint64_t StoreSize =DL.getTypeSizeInBits(DepSI->getValueOperand()->getType());
  return AnalyzeLoadFromClobberingWrite(LoadTy, LoadPtr,
                                        StorePtr, StoreSize, DL);
}

/// This function is called when we have a
/// memdep query of a load that ends up being clobbered by another load.  See if
/// the other load can feed into the second load.
static int AnalyzeLoadFromClobberingLoad(Type *LoadTy, Value *LoadPtr,
                                         LoadInst *DepLI, const DataLayout &DL){
  // Cannot handle reading from store of first-class aggregate yet.
  if (DepLI->getType()->isStructTy() || DepLI->getType()->isArrayTy())
    return -1;

  Value *DepPtr = DepLI->getPointerOperand();
  uint64_t DepSize = DL.getTypeSizeInBits(DepLI->getType());
  int R = AnalyzeLoadFromClobberingWrite(LoadTy, LoadPtr, DepPtr, DepSize, DL);
  if (R != -1) return R;

  // If we have a load/load clobber an DepLI can be widened to cover this load,
  // then we should widen it!
  int64_t LoadOffs = 0;
  const Value *LoadBase =
      GetPointerBaseWithConstantOffset(LoadPtr, LoadOffs, DL);
  unsigned LoadSize = DL.getTypeStoreSize(LoadTy);

  unsigned Size = MemoryDependenceResults::getLoadLoadClobberFullWidthSize(
      LoadBase, LoadOffs, LoadSize, DepLI);
  if (Size == 0) return -1;

  // Check non-obvious conditions enforced by MDA which we rely on for being
  // able to materialize this potentially available value
  assert(DepLI->isSimple() && "Cannot widen volatile/atomic load!");
  assert(DepLI->getType()->isIntegerTy() && "Can't widen non-integer load");

  return AnalyzeLoadFromClobberingWrite(LoadTy, LoadPtr, DepPtr, Size*8, DL);
}



static int AnalyzeLoadFromClobberingMemInst(Type *LoadTy, Value *LoadPtr,
                                            MemIntrinsic *MI,
                                            const DataLayout &DL) {
  // If the mem operation is a non-constant size, we can't handle it.
  ConstantInt *SizeCst = dyn_cast<ConstantInt>(MI->getLength());
  if (!SizeCst) return -1;
  uint64_t MemSizeInBits = SizeCst->getZExtValue()*8;

  // If this is memset, we just need to see if the offset is valid in the size
  // of the memset..
  if (MI->getIntrinsicID() == Intrinsic::memset)
    return AnalyzeLoadFromClobberingWrite(LoadTy, LoadPtr, MI->getDest(),
                                          MemSizeInBits, DL);

  // If we have a memcpy/memmove, the only case we can handle is if this is a
  // copy from constant memory.  In that case, we can read directly from the
  // constant memory.
  MemTransferInst *MTI = cast<MemTransferInst>(MI);

  Constant *Src = dyn_cast<Constant>(MTI->getSource());
  if (!Src) return -1;

  GlobalVariable *GV = dyn_cast<GlobalVariable>(GetUnderlyingObject(Src, DL));
  if (!GV || !GV->isConstant()) return -1;

  // See if the access is within the bounds of the transfer.
  int Offset = AnalyzeLoadFromClobberingWrite(LoadTy, LoadPtr,
                                              MI->getDest(), MemSizeInBits, DL);
  if (Offset == -1)
    return Offset;

  unsigned AS = Src->getType()->getPointerAddressSpace();
  // Otherwise, see if we can constant fold a load from the constant with the
  // offset applied as appropriate.
  Src = ConstantExpr::getBitCast(Src,
                                 Type::getInt8PtrTy(Src->getContext(), AS));
  Constant *OffsetCst =
    ConstantInt::get(Type::getInt64Ty(Src->getContext()), (unsigned)Offset);
  Src = ConstantExpr::getGetElementPtr(Type::getInt8Ty(Src->getContext()), Src,
                                       OffsetCst);
  Src = ConstantExpr::getBitCast(Src, PointerType::get(LoadTy, AS));
  if (ConstantFoldLoadFromConstPtr(Src, LoadTy, DL))
    return Offset;
  return -1;
}


/// This function is called when we have a
/// memdep query of a load that ends up being a clobbering store.  This means
/// that the store provides bits used by the load but we the pointers don't
/// mustalias.  Check this case to see if there is anything more we can do
/// before we give up.
static Value *GetStoreValueForLoad(Value *SrcVal, unsigned Offset,
                                   Type *LoadTy,
                                   Instruction *InsertPt, const DataLayout &DL){
  LLVMContext &Ctx = SrcVal->getType()->getContext();

  uint64_t StoreSize = (DL.getTypeSizeInBits(SrcVal->getType()) + 7) / 8;
  uint64_t LoadSize = (DL.getTypeSizeInBits(LoadTy) + 7) / 8;

  IRBuilder<> Builder(InsertPt);

  // Compute which bits of the stored value are being used by the load.  Convert
  // to an integer type to start with.
  if (SrcVal->getType()->getScalarType()->isPointerTy())
    SrcVal = Builder.CreatePtrToInt(SrcVal,
        DL.getIntPtrType(SrcVal->getType()));
  if (!SrcVal->getType()->isIntegerTy())
    SrcVal = Builder.CreateBitCast(SrcVal, IntegerType::get(Ctx, StoreSize*8));

  // Shift the bits to the least significant depending on endianness.
  unsigned ShiftAmt;
  if (DL.isLittleEndian())
    ShiftAmt = Offset*8;
  else
    ShiftAmt = (StoreSize-LoadSize-Offset)*8;

  if (ShiftAmt)
    SrcVal = Builder.CreateLShr(SrcVal, ShiftAmt);

  if (LoadSize != StoreSize)
    SrcVal = Builder.CreateTrunc(SrcVal, IntegerType::get(Ctx, LoadSize*8));

  return CoerceAvailableValueToLoadType(SrcVal, LoadTy, Builder, DL);
}

/// This function is called when we have a
/// memdep query of a load that ends up being a clobbering load.  This means
/// that the load *may* provide bits used by the load but we can't be sure
/// because the pointers don't mustalias.  Check this case to see if there is
/// anything more we can do before we give up.
static Value *GetLoadValueForLoad(LoadInst *SrcVal, unsigned Offset,
                                  Type *LoadTy, Instruction *InsertPt,
                                  GVN &gvn) {
  const DataLayout &DL = SrcVal->getModule()->getDataLayout();
  // If Offset+LoadTy exceeds the size of SrcVal, then we must be wanting to
  // widen SrcVal out to a larger load.
  unsigned SrcValStoreSize = DL.getTypeStoreSize(SrcVal->getType());
  unsigned LoadSize = DL.getTypeStoreSize(LoadTy);
  if (Offset+LoadSize > SrcValStoreSize) {
    assert(SrcVal->isSimple() && "Cannot widen volatile/atomic load!");
    assert(SrcVal->getType()->isIntegerTy() && "Can't widen non-integer load");
    // If we have a load/load clobber an DepLI can be widened to cover this
    // load, then we should widen it to the next power of 2 size big enough!
    unsigned NewLoadSize = Offset+LoadSize;
    if (!isPowerOf2_32(NewLoadSize))
      NewLoadSize = NextPowerOf2(NewLoadSize);

    Value *PtrVal = SrcVal->getPointerOperand();

    // Insert the new load after the old load.  This ensures that subsequent
    // memdep queries will find the new load.  We can't easily remove the old
    // load completely because it is already in the value numbering table.
    IRBuilder<> Builder(SrcVal->getParent(), ++BasicBlock::iterator(SrcVal));
    Type *DestPTy =
      IntegerType::get(LoadTy->getContext(), NewLoadSize*8);
    DestPTy = PointerType::get(DestPTy,
                               PtrVal->getType()->getPointerAddressSpace());
    Builder.SetCurrentDebugLocation(SrcVal->getDebugLoc());
    PtrVal = Builder.CreateBitCast(PtrVal, DestPTy);
    LoadInst *NewLoad = Builder.CreateLoad(PtrVal);
    NewLoad->takeName(SrcVal);
    NewLoad->setAlignment(SrcVal->getAlignment());

    DEBUG(dbgs() << "GVN WIDENED LOAD: " << *SrcVal << "\n");
    DEBUG(dbgs() << "TO: " << *NewLoad << "\n");

    // Replace uses of the original load with the wider load.  On a big endian
    // system, we need to shift down to get the relevant bits.
    Value *RV = NewLoad;
    if (DL.isBigEndian())
      RV = Builder.CreateLShr(RV, (NewLoadSize - SrcValStoreSize) * 8);
    RV = Builder.CreateTrunc(RV, SrcVal->getType());
    SrcVal->replaceAllUsesWith(RV);

    // We would like to use gvn.markInstructionForDeletion here, but we can't
    // because the load is already memoized into the leader map table that GVN
    // tracks.  It is potentially possible to remove the load from the table,
    // but then there all of the operations based on it would need to be
    // rehashed.  Just leave the dead load around.
    gvn.getMemDep().removeInstruction(SrcVal);
    SrcVal = NewLoad;
  }

  return GetStoreValueForLoad(SrcVal, Offset, LoadTy, InsertPt, DL);
}


/// This function is called when we have a
/// memdep query of a load that ends up being a clobbering mem intrinsic.
static Value *GetMemInstValueForLoad(MemIntrinsic *SrcInst, unsigned Offset,
                                     Type *LoadTy, Instruction *InsertPt,
                                     const DataLayout &DL){
  LLVMContext &Ctx = LoadTy->getContext();
  uint64_t LoadSize = DL.getTypeSizeInBits(LoadTy)/8;

  IRBuilder<> Builder(InsertPt);

  // We know that this method is only called when the mem transfer fully
  // provides the bits for the load.
  if (MemSetInst *MSI = dyn_cast<MemSetInst>(SrcInst)) {
    // memset(P, 'x', 1234) -> splat('x'), even if x is a variable, and
    // independently of what the offset is.
    Value *Val = MSI->getValue();
    if (LoadSize != 1)
      Val = Builder.CreateZExt(Val, IntegerType::get(Ctx, LoadSize*8));

    Value *OneElt = Val;

    // Splat the value out to the right number of bits.
    for (unsigned NumBytesSet = 1; NumBytesSet != LoadSize; ) {
      // If we can double the number of bytes set, do it.
      if (NumBytesSet*2 <= LoadSize) {
        Value *ShVal = Builder.CreateShl(Val, NumBytesSet*8);
        Val = Builder.CreateOr(Val, ShVal);
        NumBytesSet <<= 1;
        continue;
      }

      // Otherwise insert one byte at a time.
      Value *ShVal = Builder.CreateShl(Val, 1*8);
      Val = Builder.CreateOr(OneElt, ShVal);
      ++NumBytesSet;
    }

    return CoerceAvailableValueToLoadType(Val, LoadTy, Builder, DL);
  }

  // Otherwise, this is a memcpy/memmove from a constant global.
  MemTransferInst *MTI = cast<MemTransferInst>(SrcInst);
  Constant *Src = cast<Constant>(MTI->getSource());
  unsigned AS = Src->getType()->getPointerAddressSpace();

  // Otherwise, see if we can constant fold a load from the constant with the
  // offset applied as appropriate.
  Src = ConstantExpr::getBitCast(Src,
                                 Type::getInt8PtrTy(Src->getContext(), AS));
  Constant *OffsetCst =
    ConstantInt::get(Type::getInt64Ty(Src->getContext()), (unsigned)Offset);
  Src = ConstantExpr::getGetElementPtr(Type::getInt8Ty(Src->getContext()), Src,
                                       OffsetCst);
  Src = ConstantExpr::getBitCast(Src, PointerType::get(LoadTy, AS));
  return ConstantFoldLoadFromConstPtr(Src, LoadTy, DL);
}


/// Given a set of loads specified by ValuesPerBlock,
/// construct SSA form, allowing us to eliminate LI.  This returns the value
/// that should be used at LI's definition site.
static Value *ConstructSSAForLoadSet(LoadInst *LI,
                         SmallVectorImpl<AvailableValueInBlock> &ValuesPerBlock,
                                     GVN &gvn) {
  // Check for the fully redundant, dominating load case.  In this case, we can
  // just use the dominating value directly.
  if (ValuesPerBlock.size() == 1 &&
      gvn.getDominatorTree().properlyDominates(ValuesPerBlock[0].BB,
                                               LI->getParent())) {
    assert(!ValuesPerBlock[0].AV.isUndefValue() &&
           "Dead BB dominate this block");
    return ValuesPerBlock[0].MaterializeAdjustedValue(LI, gvn);
  }

  // Otherwise, we have to construct SSA form.
  SmallVector<PHINode*, 8> NewPHIs;
  SSAUpdater SSAUpdate(&NewPHIs);
  SSAUpdate.Initialize(LI->getType(), LI->getName());

  for (const AvailableValueInBlock &AV : ValuesPerBlock) {
    BasicBlock *BB = AV.BB;

    if (SSAUpdate.HasValueForBlock(BB))
      continue;

    SSAUpdate.AddAvailableValue(BB, AV.MaterializeAdjustedValue(LI, gvn));
  }

  // Perform PHI construction.
  return SSAUpdate.GetValueInMiddleOfBlock(LI->getParent());
}

Value *AvailableValue::MaterializeAdjustedValue(LoadInst *LI,
                                                Instruction *InsertPt,
                                                GVN &gvn) const {
  Value *Res;
  Type *LoadTy = LI->getType();
  const DataLayout &DL = LI->getModule()->getDataLayout();
  if (isSimpleValue()) {
    Res = getSimpleValue();
    if (Res->getType() != LoadTy) {
      Res = GetStoreValueForLoad(Res, Offset, LoadTy, InsertPt, DL);

      DEBUG(dbgs() << "GVN COERCED NONLOCAL VAL:\nOffset: " << Offset << "  "
                   << *getSimpleValue() << '\n'
                   << *Res << '\n' << "\n\n\n");
    }
  } else if (isCoercedLoadValue()) {
    LoadInst *Load = getCoercedLoadValue();
    if (Load->getType() == LoadTy && Offset == 0) {
      Res = Load;
    } else {
      Res = GetLoadValueForLoad(Load, Offset, LoadTy, InsertPt, gvn);

      DEBUG(dbgs() << "GVN COERCED NONLOCAL LOAD:\nOffset: " << Offset << "  "
                   << *getCoercedLoadValue() << '\n'
                   << *Res << '\n' << "\n\n\n");
    }
  } else if (isMemIntrinValue()) {
    Res = GetMemInstValueForLoad(getMemIntrinValue(), Offset, LoadTy,
                                 InsertPt, DL);
    DEBUG(dbgs() << "GVN COERCED NONLOCAL MEM INTRIN:\nOffset: " << Offset
                 << "  " << *getMemIntrinValue() << '\n'
                 << *Res << '\n' << "\n\n\n");
  } else {
    assert(isUndefValue() && "Should be UndefVal");
    DEBUG(dbgs() << "GVN COERCED NONLOCAL Undef:\n";);
    return UndefValue::get(LoadTy);
  }
  assert(Res && "failed to materialize?");
  return Res;
}

static bool isLifetimeStart(const Instruction *Inst) {
  if (const IntrinsicInst* II = dyn_cast<IntrinsicInst>(Inst))
    return II->getIntrinsicID() == Intrinsic::lifetime_start;
  return false;
}

/// \brief Try to locate the three instruction involved in a missed
/// load-elimination case that is due to an intervening store.
static void reportMayClobberedLoad(LoadInst *LI, MemDepResult DepInfo,
                                   DominatorTree *DT,
                                   OptimizationRemarkEmitter *ORE) {
  using namespace ore;
  User *OtherAccess = nullptr;

  OptimizationRemarkMissed R(DEBUG_TYPE, "LoadClobbered", LI);
  R << "load of type " << NV("Type", LI->getType()) << " not eliminated"
    << setExtraArgs();

  for (auto *U : LI->getPointerOperand()->users())
    if (U != LI && (isa<LoadInst>(U) || isa<StoreInst>(U)) &&
        DT->dominates(cast<Instruction>(U), LI)) {
      // FIXME: for now give up if there are multiple memory accesses that
      // dominate the load.  We need further analysis to decide which one is
      // that we're forwarding from.
      if (OtherAccess)
        OtherAccess = nullptr;
      else
        OtherAccess = U;
    }

  if (OtherAccess)
    R << " in favor of " << NV("OtherAccess", OtherAccess);

  R << " because it is clobbered by " << NV("ClobberedBy", DepInfo.getInst());

  ORE->emit(R);
}

bool GVN::AnalyzeLoadAvailability(LoadInst *LI, MemDepResult DepInfo,
                                  Value *Address, AvailableValue &Res) {

  assert((DepInfo.isDef() || DepInfo.isClobber()) &&
         "expected a local dependence");
  assert(LI->isUnordered() && "rules below are incorrect for ordered access");

  const DataLayout &DL = LI->getModule()->getDataLayout();

  if (DepInfo.isClobber()) {
    // If the dependence is to a store that writes to a superset of the bits
    // read by the load, we can extract the bits we need for the load from the
    // stored value.
    if (StoreInst *DepSI = dyn_cast<StoreInst>(DepInfo.getInst())) {
      // Can't forward from non-atomic to atomic without violating memory model.
      if (Address && LI->isAtomic() <= DepSI->isAtomic()) {
        int Offset =
          AnalyzeLoadFromClobberingStore(LI->getType(), Address, DepSI);
        if (Offset != -1) {
          Res = AvailableValue::get(DepSI->getValueOperand(), Offset);
          return true;
        }
      }
    }

    // Check to see if we have something like this:
    //    load i32* P
    //    load i8* (P+1)
    // if we have this, replace the later with an extraction from the former.
    if (LoadInst *DepLI = dyn_cast<LoadInst>(DepInfo.getInst())) {
      // If this is a clobber and L is the first instruction in its block, then
      // we have the first instruction in the entry block.
      // Can't forward from non-atomic to atomic without violating memory model.
      if (DepLI != LI && Address && LI->isAtomic() <= DepLI->isAtomic()) {
        int Offset =
          AnalyzeLoadFromClobberingLoad(LI->getType(), Address, DepLI, DL);

        if (Offset != -1) {
          Res = AvailableValue::getLoad(DepLI, Offset);
          return true;
        }
      }
    }

    // If the clobbering value is a memset/memcpy/memmove, see if we can
    // forward a value on from it.
    if (MemIntrinsic *DepMI = dyn_cast<MemIntrinsic>(DepInfo.getInst())) {
      if (Address && !LI->isAtomic()) {
        int Offset = AnalyzeLoadFromClobberingMemInst(LI->getType(), Address,
                                                      DepMI, DL);
        if (Offset != -1) {
          Res = AvailableValue::getMI(DepMI, Offset);
          return true;
        }
      }
    }
    // Nothing known about this clobber, have to be conservative
    DEBUG(
      // fast print dep, using operator<< on instruction is too slow.
      dbgs() << "GVN: load ";
      LI->printAsOperand(dbgs());
      Instruction *I = DepInfo.getInst();
      dbgs() << " is clobbered by " << *I << '\n';
    );

    if (ORE->allowExtraAnalysis())
      reportMayClobberedLoad(LI, DepInfo, DT, ORE);

    return false;
  }
  assert(DepInfo.isDef() && "follows from above");

  Instruction *DepInst = DepInfo.getInst();

  // Loading the allocation -> undef.
  if (isa<AllocaInst>(DepInst) || isMallocLikeFn(DepInst, TLI) ||
      // Loading immediately after lifetime begin -> undef.
      isLifetimeStart(DepInst)) {
    Res = AvailableValue::get(UndefValue::get(LI->getType()));
    return true;
  }

  // Loading from calloc (which zero initializes memory) -> zero
  if (isCallocLikeFn(DepInst, TLI)) {
    Res = AvailableValue::get(Constant::getNullValue(LI->getType()));
    return true;
  }

  if (StoreInst *S = dyn_cast<StoreInst>(DepInst)) {
    // Reject loads and stores that are to the same address but are of
    // different types if we have to. If the stored value is larger or equal to
    // the loaded value, we can reuse it.
    if (S->getValueOperand()->getType() != LI->getType() &&
        !CanCoerceMustAliasedValueToLoad(S->getValueOperand(),
                                         LI->getType(), DL))
      return false;

    // Can't forward from non-atomic to atomic without violating memory model.
    if (S->isAtomic() < LI->isAtomic())
      return false;

    Res = AvailableValue::get(S->getValueOperand());
    return true;
  }

  if (LoadInst *LD = dyn_cast<LoadInst>(DepInst)) {
    // If the types mismatch and we can't handle it, reject reuse of the load.
    // If the stored value is larger or equal to the loaded value, we can reuse
    // it.
    if (LD->getType() != LI->getType() &&
        !CanCoerceMustAliasedValueToLoad(LD, LI->getType(), DL))
      return false;

    // Can't forward from non-atomic to atomic without violating memory model.
    if (LD->isAtomic() < LI->isAtomic())
      return false;

    Res = AvailableValue::getLoad(LD);
    return true;
  }

  // Unknown def - must be conservative
  DEBUG(
    // fast print dep, using operator<< on instruction is too slow.
    dbgs() << "GVN: load ";
    LI->printAsOperand(dbgs());
    dbgs() << " has unknown def " << *DepInst << '\n';
  );
  return false;
}

void GVN::AnalyzeLoadAvailability(LoadInst *LI, LoadDepVect &Deps,
                                  AvailValInBlkVect &ValuesPerBlock,
                                  UnavailBlkVect &UnavailableBlocks) {

  // Filter out useless results (non-locals, etc).  Keep track of the blocks
  // where we have a value available in repl, also keep track of whether we see
  // dependencies that produce an unknown value for the load (such as a call
  // that could potentially clobber the load).
  unsigned NumDeps = Deps.size();
  for (unsigned i = 0, e = NumDeps; i != e; ++i) {
    BasicBlock *DepBB = Deps[i].getBB();
    MemDepResult DepInfo = Deps[i].getResult();

    if (DeadBlocks.count(DepBB)) {
      // Dead dependent mem-op disguise as a load evaluating the same value
      // as the load in question.
      ValuesPerBlock.push_back(AvailableValueInBlock::getUndef(DepBB));
      continue;
    }

    if (!DepInfo.isDef() && !DepInfo.isClobber()) {
      UnavailableBlocks.push_back(DepBB);
      continue;
    }

    // The address being loaded in this non-local block may not be the same as
    // the pointer operand of the load if PHI translation occurs.  Make sure
    // to consider the right address.
    Value *Address = Deps[i].getAddress();

    AvailableValue AV;
    if (AnalyzeLoadAvailability(LI, DepInfo, Address, AV)) {
      // subtlety: because we know this was a non-local dependency, we know
      // it's safe to materialize anywhere between the instruction within
      // DepInfo and the end of it's block.
      ValuesPerBlock.push_back(AvailableValueInBlock::get(DepBB,
                                                          std::move(AV)));
    } else {
      UnavailableBlocks.push_back(DepBB);
    }
  }

  assert(NumDeps == ValuesPerBlock.size() + UnavailableBlocks.size() &&
         "post condition violation");
}

bool GVN::PerformLoadPRE(LoadInst *LI, AvailValInBlkVect &ValuesPerBlock,
                         UnavailBlkVect &UnavailableBlocks) {
  // Okay, we have *some* definitions of the value.  This means that the value
  // is available in some of our (transitive) predecessors.  Lets think about
  // doing PRE of this load.  This will involve inserting a new load into the
  // predecessor when it's not available.  We could do this in general, but
  // prefer to not increase code size.  As such, we only do this when we know
  // that we only have to insert *one* load (which means we're basically moving
  // the load, not inserting a new one).

  SmallPtrSet<BasicBlock *, 4> Blockers(UnavailableBlocks.begin(),
                                        UnavailableBlocks.end());

  // Let's find the first basic block with more than one predecessor.  Walk
  // backwards through predecessors if needed.
  BasicBlock *LoadBB = LI->getParent();
  BasicBlock *TmpBB = LoadBB;

  while (TmpBB->getSinglePredecessor()) {
    TmpBB = TmpBB->getSinglePredecessor();
    if (TmpBB == LoadBB) // Infinite (unreachable) loop.
      return false;
    if (Blockers.count(TmpBB))
      return false;

    // If any of these blocks has more than one successor (i.e. if the edge we
    // just traversed was critical), then there are other paths through this
    // block along which the load may not be anticipated.  Hoisting the load
    // above this block would be adding the load to execution paths along
    // which it was not previously executed.
    if (TmpBB->getTerminator()->getNumSuccessors() != 1)
      return false;
  }

  assert(TmpBB);
  LoadBB = TmpBB;

  // Check to see how many predecessors have the loaded value fully
  // available.
  MapVector<BasicBlock *, Value *> PredLoads;
  DenseMap<BasicBlock*, char> FullyAvailableBlocks;
  for (const AvailableValueInBlock &AV : ValuesPerBlock)
    FullyAvailableBlocks[AV.BB] = true;
  for (BasicBlock *UnavailableBB : UnavailableBlocks)
    FullyAvailableBlocks[UnavailableBB] = false;

  SmallVector<BasicBlock *, 4> CriticalEdgePred;
  for (BasicBlock *Pred : predecessors(LoadBB)) {
    // If any predecessor block is an EH pad that does not allow non-PHI
    // instructions before the terminator, we can't PRE the load.
    if (Pred->getTerminator()->isEHPad()) {
      DEBUG(dbgs()
            << "COULD NOT PRE LOAD BECAUSE OF AN EH PAD PREDECESSOR '"
            << Pred->getName() << "': " << *LI << '\n');
      return false;
    }

    if (IsValueFullyAvailableInBlock(Pred, FullyAvailableBlocks, 0)) {
      continue;
    }

    if (Pred->getTerminator()->getNumSuccessors() != 1) {
      if (isa<IndirectBrInst>(Pred->getTerminator())) {
        DEBUG(dbgs() << "COULD NOT PRE LOAD BECAUSE OF INDBR CRITICAL EDGE '"
              << Pred->getName() << "': " << *LI << '\n');
        return false;
      }

      if (LoadBB->isEHPad()) {
        DEBUG(dbgs()
              << "COULD NOT PRE LOAD BECAUSE OF AN EH PAD CRITICAL EDGE '"
              << Pred->getName() << "': " << *LI << '\n');
        return false;
      }

      CriticalEdgePred.push_back(Pred);
    } else {
      // Only add the predecessors that will not be split for now.
      PredLoads[Pred] = nullptr;
    }
  }

  // Decide whether PRE is profitable for this load.
  unsigned NumUnavailablePreds = PredLoads.size() + CriticalEdgePred.size();
  assert(NumUnavailablePreds != 0 &&
         "Fully available value should already be eliminated!");

  // If this load is unavailable in multiple predecessors, reject it.
  // FIXME: If we could restructure the CFG, we could make a common pred with
  // all the preds that don't have an available LI and insert a new load into
  // that one block.
  if (NumUnavailablePreds != 1)
      return false;

  // Split critical edges, and update the unavailable predecessors accordingly.
  for (BasicBlock *OrigPred : CriticalEdgePred) {
    BasicBlock *NewPred = splitCriticalEdges(OrigPred, LoadBB);
    assert(!PredLoads.count(OrigPred) && "Split edges shouldn't be in map!");
    PredLoads[NewPred] = nullptr;
    DEBUG(dbgs() << "Split critical edge " << OrigPred->getName() << "->"
                 << LoadBB->getName() << '\n');
  }

  // Check if the load can safely be moved to all the unavailable predecessors.
  bool CanDoPRE = true;
  const DataLayout &DL = LI->getModule()->getDataLayout();
  SmallVector<Instruction*, 8> NewInsts;
  for (auto &PredLoad : PredLoads) {
    BasicBlock *UnavailablePred = PredLoad.first;

    // Do PHI translation to get its value in the predecessor if necessary.  The
    // returned pointer (if non-null) is guaranteed to dominate UnavailablePred.

    // If all preds have a single successor, then we know it is safe to insert
    // the load on the pred (?!?), so we can insert code to materialize the
    // pointer if it is not available.
    PHITransAddr Address(LI->getPointerOperand(), DL, AC);
    Value *LoadPtr = nullptr;
    LoadPtr = Address.PHITranslateWithInsertion(LoadBB, UnavailablePred,
                                                *DT, NewInsts);

    // If we couldn't find or insert a computation of this phi translated value,
    // we fail PRE.
    if (!LoadPtr) {
      DEBUG(dbgs() << "COULDN'T INSERT PHI TRANSLATED VALUE OF: "
            << *LI->getPointerOperand() << "\n");
      CanDoPRE = false;
      break;
    }

    PredLoad.second = LoadPtr;
  }

  if (!CanDoPRE) {
    while (!NewInsts.empty()) {
      Instruction *I = NewInsts.pop_back_val();
      if (MD) MD->removeInstruction(I);
      I->eraseFromParent();
    }
    // HINT: Don't revert the edge-splitting as following transformation may
    // also need to split these critical edges.
    return !CriticalEdgePred.empty();
  }

  // Okay, we can eliminate this load by inserting a reload in the predecessor
  // and using PHI construction to get the value in the other predecessors, do
  // it.
  DEBUG(dbgs() << "GVN REMOVING PRE LOAD: " << *LI << '\n');
  DEBUG(if (!NewInsts.empty())
          dbgs() << "INSERTED " << NewInsts.size() << " INSTS: "
                 << *NewInsts.back() << '\n');

  // Assign value numbers to the new instructions.
  for (Instruction *I : NewInsts) {
    // Instructions that have been inserted in predecessor(s) to materialize
    // the load address do not retain their original debug locations. Doing
    // so could lead to confusing (but correct) source attributions.
    // FIXME: How do we retain source locations without causing poor debugging
    // behavior?
    I->setDebugLoc(DebugLoc());

    // FIXME: We really _ought_ to insert these value numbers into their
    // parent's availability map.  However, in doing so, we risk getting into
    // ordering issues.  If a block hasn't been processed yet, we would be
    // marking a value as AVAIL-IN, which isn't what we intend.
    VN.lookupOrAdd(I);
  }

  for (const auto &PredLoad : PredLoads) {
    BasicBlock *UnavailablePred = PredLoad.first;
    Value *LoadPtr = PredLoad.second;

    auto *NewLoad = new LoadInst(LoadPtr, LI->getName()+".pre",
                                 LI->isVolatile(), LI->getAlignment(),
                                 LI->getOrdering(), LI->getSynchScope(),
                                 UnavailablePred->getTerminator());

    // Transfer the old load's AA tags to the new load.
    AAMDNodes Tags;
    LI->getAAMetadata(Tags);
    if (Tags)
      NewLoad->setAAMetadata(Tags);

    if (auto *MD = LI->getMetadata(LLVMContext::MD_invariant_load))
      NewLoad->setMetadata(LLVMContext::MD_invariant_load, MD);
    if (auto *InvGroupMD = LI->getMetadata(LLVMContext::MD_invariant_group))
      NewLoad->setMetadata(LLVMContext::MD_invariant_group, InvGroupMD);
    if (auto *RangeMD = LI->getMetadata(LLVMContext::MD_range))
      NewLoad->setMetadata(LLVMContext::MD_range, RangeMD);

    // We do not propagate the old load's debug location, because the new
    // load now lives in a different BB, and we want to avoid a jumpy line
    // table.
    // FIXME: How do we retain source locations without causing poor debugging
    // behavior?

    // Add the newly created load.
    ValuesPerBlock.push_back(AvailableValueInBlock::get(UnavailablePred,
                                                        NewLoad));
    MD->invalidateCachedPointerInfo(LoadPtr);
    DEBUG(dbgs() << "GVN INSERTED " << *NewLoad << '\n');
  }

  // Perform PHI construction.
  Value *V = ConstructSSAForLoadSet(LI, ValuesPerBlock, *this);
  LI->replaceAllUsesWith(V);
  if (isa<PHINode>(V))
    V->takeName(LI);
  if (Instruction *I = dyn_cast<Instruction>(V))
    I->setDebugLoc(LI->getDebugLoc());
  if (V->getType()->getScalarType()->isPointerTy())
    MD->invalidateCachedPointerInfo(V);
  markInstructionForDeletion(LI);
  ORE->emit(OptimizationRemark(DEBUG_TYPE, "LoadPRE", LI)
            << "load eliminated by PRE");
  ++NumPRELoad;
  return true;
}

static void reportLoadElim(LoadInst *LI, Value *AvailableValue,
                           OptimizationRemarkEmitter *ORE) {
  using namespace ore;
  ORE->emit(OptimizationRemark(DEBUG_TYPE, "LoadElim", LI)
            << "load of type " << NV("Type", LI->getType()) << " eliminated"
            << setExtraArgs() << " in favor of "
            << NV("InfavorOfValue", AvailableValue));
}

/// Attempt to eliminate a load whose dependencies are
/// non-local by performing PHI construction.
bool GVN::processNonLocalLoad(LoadInst *LI) {
  // non-local speculations are not allowed under asan.
  if (LI->getParent()->getParent()->hasFnAttribute(Attribute::SanitizeAddress))
    return false;

  // Step 1: Find the non-local dependencies of the load.
  LoadDepVect Deps;
  MD->getNonLocalPointerDependency(LI, Deps);

  // If we had to process more than one hundred blocks to find the
  // dependencies, this load isn't worth worrying about.  Optimizing
  // it will be too expensive.
  unsigned NumDeps = Deps.size();
  if (NumDeps > 100)
    return false;

  // If we had a phi translation failure, we'll have a single entry which is a
  // clobber in the current block.  Reject this early.
  if (NumDeps == 1 &&
      !Deps[0].getResult().isDef() && !Deps[0].getResult().isClobber()) {
    DEBUG(
      dbgs() << "GVN: non-local load ";
      LI->printAsOperand(dbgs());
      dbgs() << " has unknown dependencies\n";
    );
    return false;
  }

  // If this load follows a GEP, see if we can PRE the indices before analyzing.
  if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(LI->getOperand(0))) {
    for (GetElementPtrInst::op_iterator OI = GEP->idx_begin(),
                                        OE = GEP->idx_end();
         OI != OE; ++OI)
      if (Instruction *I = dyn_cast<Instruction>(OI->get()))
        performScalarPRE(I);
  }

  // Step 2: Analyze the availability of the load
  AvailValInBlkVect ValuesPerBlock;
  UnavailBlkVect UnavailableBlocks;
  AnalyzeLoadAvailability(LI, Deps, ValuesPerBlock, UnavailableBlocks);

  // If we have no predecessors that produce a known value for this load, exit
  // early.
  if (ValuesPerBlock.empty())
    return false;

  // Step 3: Eliminate fully redundancy.
  //
  // If all of the instructions we depend on produce a known value for this
  // load, then it is fully redundant and we can use PHI insertion to compute
  // its value.  Insert PHIs and remove the fully redundant value now.
  if (UnavailableBlocks.empty()) {
    DEBUG(dbgs() << "GVN REMOVING NONLOCAL LOAD: " << *LI << '\n');

    // Perform PHI construction.
    Value *V = ConstructSSAForLoadSet(LI, ValuesPerBlock, *this);
    LI->replaceAllUsesWith(V);

    if (isa<PHINode>(V))
      V->takeName(LI);
    if (Instruction *I = dyn_cast<Instruction>(V))
      // If instruction I has debug info, then we should not update it.
      // Also, if I has a null DebugLoc, then it is still potentially incorrect
      // to propagate LI's DebugLoc because LI may not post-dominate I.
      if (LI->getDebugLoc() && ValuesPerBlock.size() != 1)
        I->setDebugLoc(LI->getDebugLoc());
    if (V->getType()->getScalarType()->isPointerTy())
      MD->invalidateCachedPointerInfo(V);
    markInstructionForDeletion(LI);
    ++NumGVNLoad;
    reportLoadElim(LI, V, ORE);
    return true;
  }

  // Step 4: Eliminate partial redundancy.
  if (!EnablePRE || !EnableLoadPRE)
    return false;

  return PerformLoadPRE(LI, ValuesPerBlock, UnavailableBlocks);
}

bool GVN::processAssumeIntrinsic(IntrinsicInst *IntrinsicI) {
  assert(IntrinsicI->getIntrinsicID() == Intrinsic::assume &&
         "This function can only be called with llvm.assume intrinsic");
  Value *V = IntrinsicI->getArgOperand(0);

  if (ConstantInt *Cond = dyn_cast<ConstantInt>(V)) {
    if (Cond->isZero()) {
      Type *Int8Ty = Type::getInt8Ty(V->getContext());
      // Insert a new store to null instruction before the load to indicate that
      // this code is not reachable.  FIXME: We could insert unreachable
      // instruction directly because we can modify the CFG.
      new StoreInst(UndefValue::get(Int8Ty),
                    Constant::getNullValue(Int8Ty->getPointerTo()),
                    IntrinsicI);
    }
    markInstructionForDeletion(IntrinsicI);
    return false;
  } else if (isa<Constant>(V)) {
    // If it's not false, and constant, it must evaluate to true. This means our
    // assume is assume(true), and thus, pointless, and we don't want to do
    // anything more here.
    return false;
  }

  Constant *True = ConstantInt::getTrue(V->getContext());
  bool Changed = false;

  for (BasicBlock *Successor : successors(IntrinsicI->getParent())) {
    BasicBlockEdge Edge(IntrinsicI->getParent(), Successor);

    // This property is only true in dominated successors, propagateEquality
    // will check dominance for us.
    Changed |= propagateEquality(V, True, Edge, false);
  }

  // We can replace assume value with true, which covers cases like this:
  // call void @llvm.assume(i1 %cmp)
  // br i1 %cmp, label %bb1, label %bb2 ; will change %cmp to true
  ReplaceWithConstMap[V] = True;

  // If one of *cmp *eq operand is const, adding it to map will cover this:
  // %cmp = fcmp oeq float 3.000000e+00, %0 ; const on lhs could happen
  // call void @llvm.assume(i1 %cmp)
  // ret float %0 ; will change it to ret float 3.000000e+00
  if (auto *CmpI = dyn_cast<CmpInst>(V)) {
    if (CmpI->getPredicate() == CmpInst::Predicate::ICMP_EQ ||
        CmpI->getPredicate() == CmpInst::Predicate::FCMP_OEQ ||
        (CmpI->getPredicate() == CmpInst::Predicate::FCMP_UEQ &&
         CmpI->getFastMathFlags().noNaNs())) {
      Value *CmpLHS = CmpI->getOperand(0);
      Value *CmpRHS = CmpI->getOperand(1);
      if (isa<Constant>(CmpLHS))
        std::swap(CmpLHS, CmpRHS);
      auto *RHSConst = dyn_cast<Constant>(CmpRHS);

      // If only one operand is constant.
      if (RHSConst != nullptr && !isa<Constant>(CmpLHS))
        ReplaceWithConstMap[CmpLHS] = RHSConst;
    }
  }
  return Changed;
}

static void patchReplacementInstruction(Instruction *I, Value *Repl) {
  auto *ReplInst = dyn_cast<Instruction>(Repl);
  if (!ReplInst)
    return;

  // Patch the replacement so that it is not more restrictive than the value
  // being replaced.
  // Note that if 'I' is a load being replaced by some operation, 
  // for example, by an arithmetic operation, then andIRFlags()
  // would just erase all math flags from the original arithmetic
  // operation, which is clearly not wanted and not needed.
  if (!isa<LoadInst>(I))
    ReplInst->andIRFlags(I);

  // FIXME: If both the original and replacement value are part of the
  // same control-flow region (meaning that the execution of one
  // guarantees the execution of the other), then we can combine the
  // noalias scopes here and do better than the general conservative
  // answer used in combineMetadata().

  // In general, GVN unifies expressions over different control-flow
  // regions, and so we need a conservative combination of the noalias
  // scopes.
  static const unsigned KnownIDs[] = {
      LLVMContext::MD_tbaa,           LLVMContext::MD_alias_scope,
      LLVMContext::MD_noalias,        LLVMContext::MD_range,
      LLVMContext::MD_fpmath,         LLVMContext::MD_invariant_load,
      LLVMContext::MD_invariant_group};
  combineMetadata(ReplInst, I, KnownIDs);
}

static void patchAndReplaceAllUsesWith(Instruction *I, Value *Repl) {
  patchReplacementInstruction(I, Repl);
  I->replaceAllUsesWith(Repl);
}

/// Attempt to eliminate a load, first by eliminating it
/// locally, and then attempting non-local elimination if that fails.
bool GVN::processLoad(LoadInst *L) {
  if (!MD)
    return false;

  // This code hasn't been audited for ordered or volatile memory access
  if (!L->isUnordered())
    return false;

  if (L->use_empty()) {
    markInstructionForDeletion(L);
    return true;
  }

  // ... to a pointer that has been loaded from before...
  MemDepResult Dep = MD->getDependency(L);

  // If it is defined in another block, try harder.
  if (Dep.isNonLocal())
    return processNonLocalLoad(L);

  // Only handle the local case below
  if (!Dep.isDef() && !Dep.isClobber()) {
    // This might be a NonFuncLocal or an Unknown
    DEBUG(
      // fast print dep, using operator<< on instruction is too slow.
      dbgs() << "GVN: load ";
      L->printAsOperand(dbgs());
      dbgs() << " has unknown dependence\n";
    );
    return false;
  }

  AvailableValue AV;
  if (AnalyzeLoadAvailability(L, Dep, L->getPointerOperand(), AV)) {
    Value *AvailableValue = AV.MaterializeAdjustedValue(L, L, *this);

    // Replace the load!
    patchAndReplaceAllUsesWith(L, AvailableValue);
    markInstructionForDeletion(L);
    ++NumGVNLoad;
    reportLoadElim(L, AvailableValue, ORE);
    // Tell MDA to rexamine the reused pointer since we might have more
    // information after forwarding it.
    if (MD && AvailableValue->getType()->getScalarType()->isPointerTy())
      MD->invalidateCachedPointerInfo(AvailableValue);
    return true;
  }

  return false;
}

// In order to find a leader for a given value number at a
// specific basic block, we first obtain the list of all Values for that number,
// and then scan the list to find one whose block dominates the block in
// question.  This is fast because dominator tree queries consist of only
// a few comparisons of DFS numbers.
Value *GVN::findLeader(const BasicBlock *BB, uint32_t num) {
  LeaderTableEntry Vals = LeaderTable[num];
  if (!Vals.Val) return nullptr;

  Value *Val = nullptr;
  if (DT->dominates(Vals.BB, BB)) {
    Val = Vals.Val;
    if (isa<Constant>(Val)) return Val;
  }

  LeaderTableEntry* Next = Vals.Next;
  while (Next) {
    if (DT->dominates(Next->BB, BB)) {
      if (isa<Constant>(Next->Val)) return Next->Val;
      if (!Val) Val = Next->Val;
    }

    Next = Next->Next;
  }

  return Val;
}

/// There is an edge from 'Src' to 'Dst'.  Return
/// true if every path from the entry block to 'Dst' passes via this edge.  In
/// particular 'Dst' must not be reachable via another edge from 'Src'.
static bool isOnlyReachableViaThisEdge(const BasicBlockEdge &E,
                                       DominatorTree *DT) {
  // While in theory it is interesting to consider the case in which Dst has
  // more than one predecessor, because Dst might be part of a loop which is
  // only reachable from Src, in practice it is pointless since at the time
  // GVN runs all such loops have preheaders, which means that Dst will have
  // been changed to have only one predecessor, namely Src.
  const BasicBlock *Pred = E.getEnd()->getSinglePredecessor();
  assert((!Pred || Pred == E.getStart()) &&
         "No edge between these basic blocks!");
  return Pred != nullptr;
}

// Tries to replace instruction with const, using information from
// ReplaceWithConstMap.
bool GVN::replaceOperandsWithConsts(Instruction *Instr) const {
  bool Changed = false;
  for (unsigned OpNum = 0; OpNum < Instr->getNumOperands(); ++OpNum) {
    Value *Operand = Instr->getOperand(OpNum);
    auto it = ReplaceWithConstMap.find(Operand);
    if (it != ReplaceWithConstMap.end()) {
      assert(!isa<Constant>(Operand) &&
             "Replacing constants with constants is invalid");
      DEBUG(dbgs() << "GVN replacing: " << *Operand << " with " << *it->second
                   << " in instruction " << *Instr << '\n');
      Instr->setOperand(OpNum, it->second);
      Changed = true;
    }
  }
  return Changed;
}

/// The given values are known to be equal in every block
/// dominated by 'Root'.  Exploit this, for example by replacing 'LHS' with
/// 'RHS' everywhere in the scope.  Returns whether a change was made.
/// If DominatesByEdge is false, then it means that we will propagate the RHS
/// value starting from the end of Root.Start.
bool GVN::propagateEquality(Value *LHS, Value *RHS, const BasicBlockEdge &Root,
                            bool DominatesByEdge) {
  SmallVector<std::pair<Value*, Value*>, 4> Worklist;
  Worklist.push_back(std::make_pair(LHS, RHS));
  bool Changed = false;
  // For speed, compute a conservative fast approximation to
  // DT->dominates(Root, Root.getEnd());
  const bool RootDominatesEnd = isOnlyReachableViaThisEdge(Root, DT);

  while (!Worklist.empty()) {
    std::pair<Value*, Value*> Item = Worklist.pop_back_val();
    LHS = Item.first; RHS = Item.second;

    if (LHS == RHS)
      continue;
    assert(LHS->getType() == RHS->getType() && "Equality but unequal types!");

    // Don't try to propagate equalities between constants.
    if (isa<Constant>(LHS) && isa<Constant>(RHS))
      continue;

    // Prefer a constant on the right-hand side, or an Argument if no constants.
    if (isa<Constant>(LHS) || (isa<Argument>(LHS) && !isa<Constant>(RHS)))
      std::swap(LHS, RHS);
    assert((isa<Argument>(LHS) || isa<Instruction>(LHS)) && "Unexpected value!");

    // If there is no obvious reason to prefer the left-hand side over the
    // right-hand side, ensure the longest lived term is on the right-hand side,
    // so the shortest lived term will be replaced by the longest lived.
    // This tends to expose more simplifications.
    uint32_t LVN = VN.lookupOrAdd(LHS);
    if ((isa<Argument>(LHS) && isa<Argument>(RHS)) ||
        (isa<Instruction>(LHS) && isa<Instruction>(RHS))) {
      // Move the 'oldest' value to the right-hand side, using the value number
      // as a proxy for age.
      uint32_t RVN = VN.lookupOrAdd(RHS);
      if (LVN < RVN) {
        std::swap(LHS, RHS);
        LVN = RVN;
      }
    }

    // If value numbering later sees that an instruction in the scope is equal
    // to 'LHS' then ensure it will be turned into 'RHS'.  In order to preserve
    // the invariant that instructions only occur in the leader table for their
    // own value number (this is used by removeFromLeaderTable), do not do this
    // if RHS is an instruction (if an instruction in the scope is morphed into
    // LHS then it will be turned into RHS by the next GVN iteration anyway, so
    // using the leader table is about compiling faster, not optimizing better).
    // The leader table only tracks basic blocks, not edges. Only add to if we
    // have the simple case where the edge dominates the end.
    if (RootDominatesEnd && !isa<Instruction>(RHS))
      addToLeaderTable(LVN, RHS, Root.getEnd());

    // Replace all occurrences of 'LHS' with 'RHS' everywhere in the scope.  As
    // LHS always has at least one use that is not dominated by Root, this will
    // never do anything if LHS has only one use.
    if (!LHS->hasOneUse()) {
      unsigned NumReplacements =
          DominatesByEdge
              ? replaceDominatedUsesWith(LHS, RHS, *DT, Root)
              : replaceDominatedUsesWith(LHS, RHS, *DT, Root.getStart());

      Changed |= NumReplacements > 0;
      NumGVNEqProp += NumReplacements;
    }

    // Now try to deduce additional equalities from this one. For example, if
    // the known equality was "(A != B)" == "false" then it follows that A and B
    // are equal in the scope. Only boolean equalities with an explicit true or
    // false RHS are currently supported.
    if (!RHS->getType()->isIntegerTy(1))
      // Not a boolean equality - bail out.
      continue;
    ConstantInt *CI = dyn_cast<ConstantInt>(RHS);
    if (!CI)
      // RHS neither 'true' nor 'false' - bail out.
      continue;
    // Whether RHS equals 'true'.  Otherwise it equals 'false'.
    bool isKnownTrue = CI->isAllOnesValue();
    bool isKnownFalse = !isKnownTrue;

    // If "A && B" is known true then both A and B are known true.  If "A || B"
    // is known false then both A and B are known false.
    Value *A, *B;
    if ((isKnownTrue && match(LHS, m_And(m_Value(A), m_Value(B)))) ||
        (isKnownFalse && match(LHS, m_Or(m_Value(A), m_Value(B))))) {
      Worklist.push_back(std::make_pair(A, RHS));
      Worklist.push_back(std::make_pair(B, RHS));
      continue;
    }

    // If we are propagating an equality like "(A == B)" == "true" then also
    // propagate the equality A == B.  When propagating a comparison such as
    // "(A >= B)" == "true", replace all instances of "A < B" with "false".
    if (CmpInst *Cmp = dyn_cast<CmpInst>(LHS)) {
      Value *Op0 = Cmp->getOperand(0), *Op1 = Cmp->getOperand(1);

      // If "A == B" is known true, or "A != B" is known false, then replace
      // A with B everywhere in the scope.
      if ((isKnownTrue && Cmp->getPredicate() == CmpInst::ICMP_EQ) ||
          (isKnownFalse && Cmp->getPredicate() == CmpInst::ICMP_NE))
        Worklist.push_back(std::make_pair(Op0, Op1));

      // Handle the floating point versions of equality comparisons too.
      if ((isKnownTrue && Cmp->getPredicate() == CmpInst::FCMP_OEQ) ||
          (isKnownFalse && Cmp->getPredicate() == CmpInst::FCMP_UNE)) {

        // Floating point -0.0 and 0.0 compare equal, so we can only
        // propagate values if we know that we have a constant and that
        // its value is non-zero.

        // FIXME: We should do this optimization if 'no signed zeros' is
        // applicable via an instruction-level fast-math-flag or some other
        // indicator that relaxed FP semantics are being used.

        if (isa<ConstantFP>(Op1) && !cast<ConstantFP>(Op1)->isZero())
          Worklist.push_back(std::make_pair(Op0, Op1));
      }

      // If "A >= B" is known true, replace "A < B" with false everywhere.
      CmpInst::Predicate NotPred = Cmp->getInversePredicate();
      Constant *NotVal = ConstantInt::get(Cmp->getType(), isKnownFalse);
      // Since we don't have the instruction "A < B" immediately to hand, work
      // out the value number that it would have and use that to find an
      // appropriate instruction (if any).
      uint32_t NextNum = VN.getNextUnusedValueNumber();
      uint32_t Num = VN.lookupOrAddCmp(Cmp->getOpcode(), NotPred, Op0, Op1);
      // If the number we were assigned was brand new then there is no point in
      // looking for an instruction realizing it: there cannot be one!
      if (Num < NextNum) {
        Value *NotCmp = findLeader(Root.getEnd(), Num);
        if (NotCmp && isa<Instruction>(NotCmp)) {
          unsigned NumReplacements =
              DominatesByEdge
                  ? replaceDominatedUsesWith(NotCmp, NotVal, *DT, Root)
                  : replaceDominatedUsesWith(NotCmp, NotVal, *DT,
                                             Root.getStart());
          Changed |= NumReplacements > 0;
          NumGVNEqProp += NumReplacements;
        }
      }
      // Ensure that any instruction in scope that gets the "A < B" value number
      // is replaced with false.
      // The leader table only tracks basic blocks, not edges. Only add to if we
      // have the simple case where the edge dominates the end.
      if (RootDominatesEnd)
        addToLeaderTable(Num, NotVal, Root.getEnd());

      continue;
    }
  }

  return Changed;
}

/// When calculating availability, handle an instruction
/// by inserting it into the appropriate sets
bool GVN::processInstruction(Instruction *I) {
  // Ignore dbg info intrinsics.
  if (isa<DbgInfoIntrinsic>(I))
    return false;

  // If the instruction can be easily simplified then do so now in preference
  // to value numbering it.  Value numbering often exposes redundancies, for
  // example if it determines that %y is equal to %x then the instruction
  // "%z = and i32 %x, %y" becomes "%z = and i32 %x, %x" which we now simplify.
  const DataLayout &DL = I->getModule()->getDataLayout();
  if (Value *V = SimplifyInstruction(I, DL, TLI, DT, AC)) {
    bool Changed = false;
    if (!I->use_empty()) {
      I->replaceAllUsesWith(V);
      Changed = true;
    }
    if (isInstructionTriviallyDead(I, TLI)) {
      markInstructionForDeletion(I);
      Changed = true;
    }
    if (Changed) {
      if (MD && V->getType()->getScalarType()->isPointerTy())
        MD->invalidateCachedPointerInfo(V);
      ++NumGVNSimpl;
      return true;
    }
  }

  if (IntrinsicInst *IntrinsicI = dyn_cast<IntrinsicInst>(I))
    if (IntrinsicI->getIntrinsicID() == Intrinsic::assume)
      return processAssumeIntrinsic(IntrinsicI);

  if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    if (processLoad(LI))
      return true;

    unsigned Num = VN.lookupOrAdd(LI);
    addToLeaderTable(Num, LI, LI->getParent());
    return false;
  }

  // For conditional branches, we can perform simple conditional propagation on
  // the condition value itself.
  if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
    if (!BI->isConditional())
      return false;

    if (isa<Constant>(BI->getCondition()))
      return processFoldableCondBr(BI);

    Value *BranchCond = BI->getCondition();
    BasicBlock *TrueSucc = BI->getSuccessor(0);
    BasicBlock *FalseSucc = BI->getSuccessor(1);
    // Avoid multiple edges early.
    if (TrueSucc == FalseSucc)
      return false;

    BasicBlock *Parent = BI->getParent();
    bool Changed = false;

    Value *TrueVal = ConstantInt::getTrue(TrueSucc->getContext());
    BasicBlockEdge TrueE(Parent, TrueSucc);
    Changed |= propagateEquality(BranchCond, TrueVal, TrueE, true);

    Value *FalseVal = ConstantInt::getFalse(FalseSucc->getContext());
    BasicBlockEdge FalseE(Parent, FalseSucc);
    Changed |= propagateEquality(BranchCond, FalseVal, FalseE, true);

    return Changed;
  }

  // For switches, propagate the case values into the case destinations.
  if (SwitchInst *SI = dyn_cast<SwitchInst>(I)) {
    Value *SwitchCond = SI->getCondition();
    BasicBlock *Parent = SI->getParent();
    bool Changed = false;

    // Remember how many outgoing edges there are to every successor.
    SmallDenseMap<BasicBlock *, unsigned, 16> SwitchEdges;
    for (unsigned i = 0, n = SI->getNumSuccessors(); i != n; ++i)
      ++SwitchEdges[SI->getSuccessor(i)];

    for (SwitchInst::CaseIt i = SI->case_begin(), e = SI->case_end();
         i != e; ++i) {
      BasicBlock *Dst = i.getCaseSuccessor();
      // If there is only a single edge, propagate the case value into it.
      if (SwitchEdges.lookup(Dst) == 1) {
        BasicBlockEdge E(Parent, Dst);
        Changed |= propagateEquality(SwitchCond, i.getCaseValue(), E, true);
      }
    }
    return Changed;
  }

  // Instructions with void type don't return a value, so there's
  // no point in trying to find redundancies in them.
  if (I->getType()->isVoidTy())
    return false;

  uint32_t NextNum = VN.getNextUnusedValueNumber();
  unsigned Num = VN.lookupOrAdd(I);

  // Allocations are always uniquely numbered, so we can save time and memory
  // by fast failing them.
  if (isa<AllocaInst>(I) || isa<TerminatorInst>(I) || isa<PHINode>(I)) {
    addToLeaderTable(Num, I, I->getParent());
    return false;
  }

  // If the number we were assigned was a brand new VN, then we don't
  // need to do a lookup to see if the number already exists
  // somewhere in the domtree: it can't!
  if (Num >= NextNum) {
    addToLeaderTable(Num, I, I->getParent());
    return false;
  }

  // Perform fast-path value-number based elimination of values inherited from
  // dominators.
  Value *Repl = findLeader(I->getParent(), Num);
  if (!Repl) {
    // Failure, just remember this instance for future use.
    addToLeaderTable(Num, I, I->getParent());
    return false;
  } else if (Repl == I) {
    // If I was the result of a shortcut PRE, it might already be in the table
    // and the best replacement for itself. Nothing to do.
    return false;
  }

  // Remove it!
  patchAndReplaceAllUsesWith(I, Repl);
  if (MD && Repl->getType()->getScalarType()->isPointerTy())
    MD->invalidateCachedPointerInfo(Repl);
  markInstructionForDeletion(I);
  return true;
}

/// runOnFunction - This is the main transformation entry point for a function.
bool GVN::runImpl(Function &F, AssumptionCache &RunAC, DominatorTree &RunDT,
                  const TargetLibraryInfo &RunTLI, AAResults &RunAA,
                  MemoryDependenceResults *RunMD, LoopInfo *LI,
                  OptimizationRemarkEmitter *RunORE) {
  AC = &RunAC;
  DT = &RunDT;
  VN.setDomTree(DT);
  TLI = &RunTLI;
  VN.setAliasAnalysis(&RunAA);
  MD = RunMD;
  VN.setMemDep(MD);
  ORE = RunORE;

  bool Changed = false;
  bool ShouldContinue = true;

  // Merge unconditional branches, allowing PRE to catch more
  // optimization opportunities.
  for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ) {
    BasicBlock *BB = &*FI++;

    bool removedBlock = MergeBlockIntoPredecessor(BB, DT, LI, MD);
    if (removedBlock)
      ++NumGVNBlocks;

    Changed |= removedBlock;
  }

  unsigned Iteration = 0;
  while (ShouldContinue) {
    DEBUG(dbgs() << "GVN iteration: " << Iteration << "\n");
    ShouldContinue = iterateOnFunction(F);
    Changed |= ShouldContinue;
    ++Iteration;
  }

  if (EnablePRE) {
    // Fabricate val-num for dead-code in order to suppress assertion in
    // performPRE().
    assignValNumForDeadCode();
    bool PREChanged = true;
    while (PREChanged) {
      PREChanged = performPRE(F);
      Changed |= PREChanged;
    }
  }

  // FIXME: Should perform GVN again after PRE does something.  PRE can move
  // computations into blocks where they become fully redundant.  Note that
  // we can't do this until PRE's critical edge splitting updates memdep.
  // Actually, when this happens, we should just fully integrate PRE into GVN.

  cleanupGlobalSets();
  // Do not cleanup DeadBlocks in cleanupGlobalSets() as it's called for each
  // iteration.
  DeadBlocks.clear();

  return Changed;
}

bool GVN::processBlock(BasicBlock *BB) {
  // FIXME: Kill off InstrsToErase by doing erasing eagerly in a helper function
  // (and incrementing BI before processing an instruction).
  assert(InstrsToErase.empty() &&
         "We expect InstrsToErase to be empty across iterations");
  if (DeadBlocks.count(BB))
    return false;

  // Clearing map before every BB because it can be used only for single BB.
  ReplaceWithConstMap.clear();
  bool ChangedFunction = false;

  for (BasicBlock::iterator BI = BB->begin(), BE = BB->end();
       BI != BE;) {
    if (!ReplaceWithConstMap.empty())
      ChangedFunction |= replaceOperandsWithConsts(&*BI);
    ChangedFunction |= processInstruction(&*BI);

    if (InstrsToErase.empty()) {
      ++BI;
      continue;
    }

    // If we need some instructions deleted, do it now.
    NumGVNInstr += InstrsToErase.size();

    // Avoid iterator invalidation.
    bool AtStart = BI == BB->begin();
    if (!AtStart)
      --BI;

    for (SmallVectorImpl<Instruction *>::iterator I = InstrsToErase.begin(),
         E = InstrsToErase.end(); I != E; ++I) {
      DEBUG(dbgs() << "GVN removed: " << **I << '\n');
      if (MD) MD->removeInstruction(*I);
      DEBUG(verifyRemoved(*I));
      (*I)->eraseFromParent();
    }
    InstrsToErase.clear();

    if (AtStart)
      BI = BB->begin();
    else
      ++BI;
  }

  return ChangedFunction;
}

// Instantiate an expression in a predecessor that lacked it.
bool GVN::performScalarPREInsertion(Instruction *Instr, BasicBlock *Pred,
                                    unsigned int ValNo) {
  // Because we are going top-down through the block, all value numbers
  // will be available in the predecessor by the time we need them.  Any
  // that weren't originally present will have been instantiated earlier
  // in this loop.
  bool success = true;
  for (unsigned i = 0, e = Instr->getNumOperands(); i != e; ++i) {
    Value *Op = Instr->getOperand(i);
    if (isa<Argument>(Op) || isa<Constant>(Op) || isa<GlobalValue>(Op))
      continue;
    // This could be a newly inserted instruction, in which case, we won't
    // find a value number, and should give up before we hurt ourselves.
    // FIXME: Rewrite the infrastructure to let it easier to value number
    // and process newly inserted instructions.
    if (!VN.exists(Op)) {
      success = false;
      break;
    }
    if (Value *V = findLeader(Pred, VN.lookup(Op))) {
      Instr->setOperand(i, V);
    } else {
      success = false;
      break;
    }
  }

  // Fail out if we encounter an operand that is not available in
  // the PRE predecessor.  This is typically because of loads which
  // are not value numbered precisely.
  if (!success)
    return false;

  Instr->insertBefore(Pred->getTerminator());
  Instr->setName(Instr->getName() + ".pre");
  Instr->setDebugLoc(Instr->getDebugLoc());
  VN.add(Instr, ValNo);

  // Update the availability map to include the new instruction.
  addToLeaderTable(ValNo, Instr, Pred);
  return true;
}

bool GVN::performScalarPRE(Instruction *CurInst) {
  if (isa<AllocaInst>(CurInst) || isa<TerminatorInst>(CurInst) ||
      isa<PHINode>(CurInst) || CurInst->getType()->isVoidTy() ||
      CurInst->mayReadFromMemory() || CurInst->mayHaveSideEffects() ||
      isa<DbgInfoIntrinsic>(CurInst))
    return false;

  // Don't do PRE on compares. The PHI would prevent CodeGenPrepare from
  // sinking the compare again, and it would force the code generator to
  // move the i1 from processor flags or predicate registers into a general
  // purpose register.
  if (isa<CmpInst>(CurInst))
    return false;

  // We don't currently value number ANY inline asm calls.
  if (CallInst *CallI = dyn_cast<CallInst>(CurInst))
    if (CallI->isInlineAsm())
      return false;

  uint32_t ValNo = VN.lookup(CurInst);

  // Look for the predecessors for PRE opportunities.  We're
  // only trying to solve the basic diamond case, where
  // a value is computed in the successor and one predecessor,
  // but not the other.  We also explicitly disallow cases
  // where the successor is its own predecessor, because they're
  // more complicated to get right.
  unsigned NumWith = 0;
  unsigned NumWithout = 0;
  BasicBlock *PREPred = nullptr;
  BasicBlock *CurrentBlock = CurInst->getParent();

  SmallVector<std::pair<Value *, BasicBlock *>, 8> predMap;
  for (BasicBlock *P : predecessors(CurrentBlock)) {
    // We're not interested in PRE where the block is its
    // own predecessor, or in blocks with predecessors
    // that are not reachable.
    if (P == CurrentBlock) {
      NumWithout = 2;
      break;
    } else if (!DT->isReachableFromEntry(P)) {
      NumWithout = 2;
      break;
    }

    Value *predV = findLeader(P, ValNo);
    if (!predV) {
      predMap.push_back(std::make_pair(static_cast<Value *>(nullptr), P));
      PREPred = P;
      ++NumWithout;
    } else if (predV == CurInst) {
      /* CurInst dominates this predecessor. */
      NumWithout = 2;
      break;
    } else {
      predMap.push_back(std::make_pair(predV, P));
      ++NumWith;
    }
  }

  // Don't do PRE when it might increase code size, i.e. when
  // we would need to insert instructions in more than one pred.
  if (NumWithout > 1 || NumWith == 0)
    return false;

  // We may have a case where all predecessors have the instruction,
  // and we just need to insert a phi node. Otherwise, perform
  // insertion.
  Instruction *PREInstr = nullptr;

  if (NumWithout != 0) {
    // Don't do PRE across indirect branch.
    if (isa<IndirectBrInst>(PREPred->getTerminator()))
      return false;

    // We can't do PRE safely on a critical edge, so instead we schedule
    // the edge to be split and perform the PRE the next time we iterate
    // on the function.
    unsigned SuccNum = GetSuccessorNumber(PREPred, CurrentBlock);
    if (isCriticalEdge(PREPred->getTerminator(), SuccNum)) {
      toSplit.push_back(std::make_pair(PREPred->getTerminator(), SuccNum));
      return false;
    }
    // We need to insert somewhere, so let's give it a shot
    PREInstr = CurInst->clone();
    if (!performScalarPREInsertion(PREInstr, PREPred, ValNo)) {
      // If we failed insertion, make sure we remove the instruction.
      DEBUG(verifyRemoved(PREInstr));
      delete PREInstr;
      return false;
    }
  }

  // Either we should have filled in the PRE instruction, or we should
  // not have needed insertions.
  assert (PREInstr != nullptr || NumWithout == 0);

  ++NumGVNPRE;

  // Create a PHI to make the value available in this block.
  PHINode *Phi =
      PHINode::Create(CurInst->getType(), predMap.size(),
                      CurInst->getName() + ".pre-phi", &CurrentBlock->front());
  for (unsigned i = 0, e = predMap.size(); i != e; ++i) {
    if (Value *V = predMap[i].first)
      Phi->addIncoming(V, predMap[i].second);
    else
      Phi->addIncoming(PREInstr, PREPred);
  }

  VN.add(Phi, ValNo);
  addToLeaderTable(ValNo, Phi, CurrentBlock);
  Phi->setDebugLoc(CurInst->getDebugLoc());
  CurInst->replaceAllUsesWith(Phi);
  if (MD && Phi->getType()->getScalarType()->isPointerTy())
    MD->invalidateCachedPointerInfo(Phi);
  VN.erase(CurInst);
  removeFromLeaderTable(ValNo, CurInst, CurrentBlock);

  DEBUG(dbgs() << "GVN PRE removed: " << *CurInst << '\n');
  if (MD)
    MD->removeInstruction(CurInst);
  DEBUG(verifyRemoved(CurInst));
  CurInst->eraseFromParent();
  ++NumGVNInstr;

  return true;
}

/// Perform a purely local form of PRE that looks for diamond
/// control flow patterns and attempts to perform simple PRE at the join point.
bool GVN::performPRE(Function &F) {
  bool Changed = false;
  for (BasicBlock *CurrentBlock : depth_first(&F.getEntryBlock())) {
    // Nothing to PRE in the entry block.
    if (CurrentBlock == &F.getEntryBlock())
      continue;

    // Don't perform PRE on an EH pad.
    if (CurrentBlock->isEHPad())
      continue;

    for (BasicBlock::iterator BI = CurrentBlock->begin(),
                              BE = CurrentBlock->end();
         BI != BE;) {
      Instruction *CurInst = &*BI++;
      Changed |= performScalarPRE(CurInst);
    }
  }

  if (splitCriticalEdges())
    Changed = true;

  return Changed;
}

/// Split the critical edge connecting the given two blocks, and return
/// the block inserted to the critical edge.
BasicBlock *GVN::splitCriticalEdges(BasicBlock *Pred, BasicBlock *Succ) {
  BasicBlock *BB =
      SplitCriticalEdge(Pred, Succ, CriticalEdgeSplittingOptions(DT));
  if (MD)
    MD->invalidateCachedPredecessors();
  return BB;
}

/// Split critical edges found during the previous
/// iteration that may enable further optimization.
bool GVN::splitCriticalEdges() {
  if (toSplit.empty())
    return false;
  do {
    std::pair<TerminatorInst*, unsigned> Edge = toSplit.pop_back_val();
    SplitCriticalEdge(Edge.first, Edge.second,
                      CriticalEdgeSplittingOptions(DT));
  } while (!toSplit.empty());
  if (MD) MD->invalidateCachedPredecessors();
  return true;
}

/// Executes one iteration of GVN
bool GVN::iterateOnFunction(Function &F) {
  cleanupGlobalSets();

  // Top-down walk of the dominator tree
  bool Changed = false;
  // Save the blocks this function have before transformation begins. GVN may
  // split critical edge, and hence may invalidate the RPO/DT iterator.
  //
  std::vector<BasicBlock *> BBVect;
  BBVect.reserve(256);
  // Needed for value numbering with phi construction to work.
  ReversePostOrderTraversal<Function *> RPOT(&F);
  for (ReversePostOrderTraversal<Function *>::rpo_iterator RI = RPOT.begin(),
                                                           RE = RPOT.end();
       RI != RE; ++RI)
    BBVect.push_back(*RI);

  for (std::vector<BasicBlock *>::iterator I = BBVect.begin(), E = BBVect.end();
       I != E; I++)
    Changed |= processBlock(*I);

  return Changed;
}

void GVN::cleanupGlobalSets() {
  VN.clear();
  LeaderTable.clear();
  TableAllocator.Reset();
}

/// Verify that the specified instruction does not occur in our
/// internal data structures.
void GVN::verifyRemoved(const Instruction *Inst) const {
  VN.verifyRemoved(Inst);

  // Walk through the value number scope to make sure the instruction isn't
  // ferreted away in it.
  for (DenseMap<uint32_t, LeaderTableEntry>::const_iterator
       I = LeaderTable.begin(), E = LeaderTable.end(); I != E; ++I) {
    const LeaderTableEntry *Node = &I->second;
    assert(Node->Val != Inst && "Inst still in value numbering scope!");

    while (Node->Next) {
      Node = Node->Next;
      assert(Node->Val != Inst && "Inst still in value numbering scope!");
    }
  }
}

/// BB is declared dead, which implied other blocks become dead as well. This
/// function is to add all these blocks to "DeadBlocks". For the dead blocks'
/// live successors, update their phi nodes by replacing the operands
/// corresponding to dead blocks with UndefVal.
void GVN::addDeadBlock(BasicBlock *BB) {
  SmallVector<BasicBlock *, 4> NewDead;
  SmallSetVector<BasicBlock *, 4> DF;

  NewDead.push_back(BB);
  while (!NewDead.empty()) {
    BasicBlock *D = NewDead.pop_back_val();
    if (DeadBlocks.count(D))
      continue;

    // All blocks dominated by D are dead.
    SmallVector<BasicBlock *, 8> Dom;
    DT->getDescendants(D, Dom);
    DeadBlocks.insert(Dom.begin(), Dom.end());

    // Figure out the dominance-frontier(D).
    for (BasicBlock *B : Dom) {
      for (BasicBlock *S : successors(B)) {
        if (DeadBlocks.count(S))
          continue;

        bool AllPredDead = true;
        for (BasicBlock *P : predecessors(S))
          if (!DeadBlocks.count(P)) {
            AllPredDead = false;
            break;
          }

        if (!AllPredDead) {
          // S could be proved dead later on. That is why we don't update phi
          // operands at this moment.
          DF.insert(S);
        } else {
          // While S is not dominated by D, it is dead by now. This could take
          // place if S already have a dead predecessor before D is declared
          // dead.
          NewDead.push_back(S);
        }
      }
    }
  }

  // For the dead blocks' live successors, update their phi nodes by replacing
  // the operands corresponding to dead blocks with UndefVal.
  for(SmallSetVector<BasicBlock *, 4>::iterator I = DF.begin(), E = DF.end();
        I != E; I++) {
    BasicBlock *B = *I;
    if (DeadBlocks.count(B))
      continue;

    SmallVector<BasicBlock *, 4> Preds(pred_begin(B), pred_end(B));
    for (BasicBlock *P : Preds) {
      if (!DeadBlocks.count(P))
        continue;

      if (isCriticalEdge(P->getTerminator(), GetSuccessorNumber(P, B))) {
        if (BasicBlock *S = splitCriticalEdges(P, B))
          DeadBlocks.insert(P = S);
      }

      for (BasicBlock::iterator II = B->begin(); isa<PHINode>(II); ++II) {
        PHINode &Phi = cast<PHINode>(*II);
        Phi.setIncomingValue(Phi.getBasicBlockIndex(P),
                             UndefValue::get(Phi.getType()));
      }
    }
  }
}

// If the given branch is recognized as a foldable branch (i.e. conditional
// branch with constant condition), it will perform following analyses and
// transformation.
//  1) If the dead out-coming edge is a critical-edge, split it. Let
//     R be the target of the dead out-coming edge.
//  1) Identify the set of dead blocks implied by the branch's dead outcoming
//     edge. The result of this step will be {X| X is dominated by R}
//  2) Identify those blocks which haves at least one dead predecessor. The
//     result of this step will be dominance-frontier(R).
//  3) Update the PHIs in DF(R) by replacing the operands corresponding to
//     dead blocks with "UndefVal" in an hope these PHIs will optimized away.
//
// Return true iff *NEW* dead code are found.
bool GVN::processFoldableCondBr(BranchInst *BI) {
  if (!BI || BI->isUnconditional())
    return false;

  // If a branch has two identical successors, we cannot declare either dead.
  if (BI->getSuccessor(0) == BI->getSuccessor(1))
    return false;

  ConstantInt *Cond = dyn_cast<ConstantInt>(BI->getCondition());
  if (!Cond)
    return false;

  BasicBlock *DeadRoot =
      Cond->getZExtValue() ? BI->getSuccessor(1) : BI->getSuccessor(0);
  if (DeadBlocks.count(DeadRoot))
    return false;

  if (!DeadRoot->getSinglePredecessor())
    DeadRoot = splitCriticalEdges(BI->getParent(), DeadRoot);

  addDeadBlock(DeadRoot);
  return true;
}

// performPRE() will trigger assert if it comes across an instruction without
// associated val-num. As it normally has far more live instructions than dead
// instructions, it makes more sense just to "fabricate" a val-number for the
// dead code than checking if instruction involved is dead or not.
void GVN::assignValNumForDeadCode() {
  for (BasicBlock *BB : DeadBlocks) {
    for (Instruction &Inst : *BB) {
      unsigned ValNum = VN.lookupOrAdd(&Inst);
      addToLeaderTable(ValNum, &Inst, BB);
    }
  }
}

class llvm::gvn::GVNLegacyPass : public FunctionPass {
public:
  static char ID; // Pass identification, replacement for typeid
  explicit GVNLegacyPass(bool NoLoads = false)
      : FunctionPass(ID), NoLoads(NoLoads) {
    initializeGVNLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override {
    if (skipFunction(F))
      return false;

    auto *LIWP = getAnalysisIfAvailable<LoopInfoWrapperPass>();

    return Impl.runImpl(
        F, getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F),
        getAnalysis<DominatorTreeWrapperPass>().getDomTree(),
        getAnalysis<TargetLibraryInfoWrapperPass>().getTLI(),
        getAnalysis<AAResultsWrapperPass>().getAAResults(),
        NoLoads ? nullptr
                : &getAnalysis<MemoryDependenceWrapperPass>().getMemDep(),
        LIWP ? &LIWP->getLoopInfo() : nullptr,
        &getAnalysis<OptimizationRemarkEmitterWrapperPass>().getORE());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<AssumptionCacheTracker>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<TargetLibraryInfoWrapperPass>();
    if (!NoLoads)
      AU.addRequired<MemoryDependenceWrapperPass>();
    AU.addRequired<AAResultsWrapperPass>();

    AU.addPreserved<DominatorTreeWrapperPass>();
    AU.addPreserved<GlobalsAAWrapperPass>();
    AU.addRequired<OptimizationRemarkEmitterWrapperPass>();
  }

private:
  bool NoLoads;
  GVN Impl;
};

char GVNLegacyPass::ID = 0;

// The public interface to this file...
FunctionPass *llvm::createGVNPass(bool NoLoads) {
  return new GVNLegacyPass(NoLoads);
}

INITIALIZE_PASS_BEGIN(GVNLegacyPass, "gvn", "Global Value Numbering", false, false)
INITIALIZE_PASS_DEPENDENCY(AssumptionCacheTracker)
INITIALIZE_PASS_DEPENDENCY(MemoryDependenceWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(AAResultsWrapperPass)
INITIALIZE_PASS_DEPENDENCY(GlobalsAAWrapperPass)
INITIALIZE_PASS_DEPENDENCY(OptimizationRemarkEmitterWrapperPass)
INITIALIZE_PASS_END(GVNLegacyPass, "gvn", "Global Value Numbering", false, false)
