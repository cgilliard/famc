// Copyright 2015 The Rust Project Developers. See the COPYRIGHT
// file at the top-level directory of this distribution and at
// http://rust-lang.org/COPYRIGHT.
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

use self::ImportDirectiveSubclass::*;

use {AmbiguityError, CrateLint, Module, PerNS};
use Namespace::{self, TypeNS, MacroNS};
use {NameBinding, NameBindingKind, ToNameBinding, PathResult, PrivacyError};
use Resolver;
use {names_to_string, module_to_string};
use {resolve_error, ResolutionError};

use rustc_data_structures::ptr_key::PtrKey;
use rustc::ty;
use rustc::lint::builtin::BuiltinLintDiagnostics;
use rustc::lint::builtin::{DUPLICATE_MACRO_EXPORTS, PUB_USE_OF_PRIVATE_EXTERN_CRATE};
use rustc::hir::def_id::{CRATE_DEF_INDEX, DefId};
use rustc::hir::def::*;
use rustc::session::DiagnosticMessageId;
use rustc::util::nodemap::{FxHashMap, FxHashSet};

use syntax::ast::{Ident, Name, NodeId, CRATE_NODE_ID};
use syntax::ext::base::Determinacy::{self, Determined, Undetermined};
use syntax::ext::hygiene::Mark;
use syntax::symbol::keywords;
use syntax::util::lev_distance::find_best_match_for_name;
use syntax_pos::Span;

use std::cell::{Cell, RefCell};
use std::{mem, ptr};

/// Contains data for specific types of import directives.
#[derive(Clone, Debug)]
pub enum ImportDirectiveSubclass<'a> {
    SingleImport {
        target: Ident,
        source: Ident,
        result: PerNS<Cell<Result<&'a NameBinding<'a>, Determinacy>>>,
        type_ns_only: bool,
    },
    GlobImport {
        is_prelude: bool,
        max_vis: Cell<ty::Visibility>, // The visibility of the greatest re-export.
        // n.b. `max_vis` is only used in `finalize_import` to check for re-export errors.
    },
    ExternCrate(Option<Name>),
    MacroUse,
}

/// One import directive.
#[derive(Debug,Clone)]
pub struct ImportDirective<'a> {
    /// The id of the `extern crate`, `UseTree` etc that imported this `ImportDirective`.
    ///
    /// In the case where the `ImportDirective` was expanded from a "nested" use tree,
    /// this id is the id of the leaf tree. For example:
    ///
    /// ```ignore (pacify the mercilous tidy)
    /// use foo::bar::{a, b}
    /// ```
    ///
    /// If this is the import directive for `foo::bar::a`, we would have the id of the `UseTree`
    /// for `a` in this field.
    pub id: NodeId,

    /// The `id` of the "root" use-kind -- this is always the same as
    /// `id` except in the case of "nested" use trees, in which case
    /// it will be the `id` of the root use tree. e.g., in the example
    /// from `id`, this would be the id of the `use foo::bar`
    /// `UseTree` node.
    pub root_id: NodeId,

    /// Span of this use tree.
    pub span: Span,

    /// Span of the *root* use tree (see `root_id`).
    pub root_span: Span,

    pub parent: Module<'a>,
    pub module_path: Vec<Ident>,
    pub imported_module: Cell<Option<Module<'a>>>, // the resolution of `module_path`
    pub subclass: ImportDirectiveSubclass<'a>,
    pub vis: Cell<ty::Visibility>,
    pub expansion: Mark,
    pub used: Cell<bool>,
}

impl<'a> ImportDirective<'a> {
    pub fn is_glob(&self) -> bool {
        match self.subclass { ImportDirectiveSubclass::GlobImport { .. } => true, _ => false }
    }

    crate fn crate_lint(&self) -> CrateLint {
        CrateLint::UsePath { root_id: self.root_id, root_span: self.root_span }
    }
}

#[derive(Clone, Default, Debug)]
/// Records information about the resolution of a name in a namespace of a module.
pub struct NameResolution<'a> {
    /// Single imports that may define the name in the namespace.
    /// Import directives are arena-allocated, so it's ok to use pointers as keys.
    single_imports: FxHashSet<PtrKey<'a, ImportDirective<'a>>>,
    /// The least shadowable known binding for this name, or None if there are no known bindings.
    pub binding: Option<&'a NameBinding<'a>>,
    shadowed_glob: Option<&'a NameBinding<'a>>,
}

impl<'a> NameResolution<'a> {
    // Returns the binding for the name if it is known or None if it not known.
    fn binding(&self) -> Option<&'a NameBinding<'a>> {
        self.binding.and_then(|binding| {
            if !binding.is_glob_import() ||
               self.single_imports.is_empty() { Some(binding) } else { None }
        })
    }
}

impl<'a> Resolver<'a> {
    fn resolution(&self, module: Module<'a>, ident: Ident, ns: Namespace)
                  -> &'a RefCell<NameResolution<'a>> {
        *module.resolutions.borrow_mut().entry((ident.modern(), ns))
               .or_insert_with(|| self.arenas.alloc_name_resolution())
    }

    /// Attempts to resolve `ident` in namespaces `ns` of `module`.
    /// Invariant: if `record_used` is `Some`, expansion and import resolution must be complete.
    pub fn resolve_ident_in_module_unadjusted(&mut self,
                                              module: Module<'a>,
                                              ident: Ident,
                                              ns: Namespace,
                                              restricted_shadowing: bool,
                                              record_used: bool,
                                              path_span: Span)
                                              -> Result<&'a NameBinding<'a>, Determinacy> {
        self.populate_module_if_necessary(module);

        let resolution = self.resolution(module, ident, ns)
            .try_borrow_mut()
            .map_err(|_| Determined)?; // This happens when there is a cycle of imports

        if record_used {
            if let Some(binding) = resolution.binding {
                if let Some(shadowed_glob) = resolution.shadowed_glob {
                    let name = ident.name;
                    // Forbid expanded shadowing to avoid time travel.
                    if restricted_shadowing &&
                       binding.expansion != Mark::root() &&
                       ns != MacroNS && // In MacroNS, `try_define` always forbids this shadowing
                       binding.def() != shadowed_glob.def() {
                        self.ambiguity_errors.push(AmbiguityError {
                            span: path_span,
                            name,
                            lexical: false,
                            b1: binding,
                            b2: shadowed_glob,
                        });
                    }
                }
                if self.record_use(ident, ns, binding, path_span) {
                    return Ok(self.dummy_binding);
                }
                if !self.is_accessible(binding.vis) {
                    self.privacy_errors.push(PrivacyError(path_span, ident.name, binding));
                }
            }

            return resolution.binding.ok_or(Determined);
        }

        let check_usable = |this: &mut Self, binding: &'a NameBinding<'a>| {
            // `extern crate` are always usable for backwards compatibility, see issue #37020.
            let usable = this.is_accessible(binding.vis) || binding.is_extern_crate();
            if usable { Ok(binding) } else { Err(Determined) }
        };

        // Items and single imports are not shadowable, if we have one, then it's determined.
        if let Some(binding) = resolution.binding {
            if !binding.is_glob_import() {
                return check_usable(self, binding);
            }
        }

        // --- From now on we either have a glob resolution or no resolution. ---

        // Check if one of single imports can still define the name,
        // if it can then our result is not determined and can be invalidated.
        for single_import in &resolution.single_imports {
            if !self.is_accessible(single_import.vis.get()) {
                continue;
            }
            let module = unwrap_or!(single_import.imported_module.get(), return Err(Undetermined));
            let ident = match single_import.subclass {
                SingleImport { source, .. } => source,
                _ => unreachable!(),
            };
            match self.resolve_ident_in_module(module, ident, ns, false, path_span) {
                Err(Determined) => continue,
                Ok(binding)
                    if !self.is_accessible_from(binding.vis, single_import.parent) => continue,
                Ok(_) | Err(Undetermined) => return Err(Undetermined),
            }
        }

        // So we have a resolution that's from a glob import. This resolution is determined
        // if it cannot be shadowed by some new item/import expanded from a macro.
        // This happens either if there are no unexpanded macros, or expanded names cannot
        // shadow globs (that happens in macro namespace or with restricted shadowing).
        let unexpanded_macros = !module.unresolved_invocations.borrow().is_empty();
        if let Some(binding) = resolution.binding {
            if !unexpanded_macros || ns == MacroNS || restricted_shadowing {
                return check_usable(self, binding);
            } else {
                return Err(Undetermined);
            }
        }

        // --- From now on we have no resolution. ---

        // Now we are in situation when new item/import can appear only from a glob or a macro
        // expansion. With restricted shadowing names from globs and macro expansions cannot
        // shadow names from outer scopes, so we can freely fallback from module search to search
        // in outer scopes. To continue search in outer scopes we have to lie a bit and return
        // `Determined` to `resolve_lexical_macro_path_segment` even if the correct answer
        // for in-module resolution could be `Undetermined`.
        if restricted_shadowing {
            return Err(Determined);
        }

        // Check if one of unexpanded macros can still define the name,
        // if it can then our "no resolution" result is not determined and can be invalidated.
        if unexpanded_macros {
            return Err(Undetermined);
        }

        // Check if one of glob imports can still define the name,
        // if it can then our "no resolution" result is not determined and can be invalidated.
        for glob_import in module.globs.borrow().iter() {
            if !self.is_accessible(glob_import.vis.get()) {
                continue
            }
            let module = unwrap_or!(glob_import.imported_module.get(), return Err(Undetermined));
            let (orig_current_module, mut ident) = (self.current_module, ident.modern());
            match ident.span.glob_adjust(module.expansion, glob_import.span.ctxt().modern()) {
                Some(Some(def)) => self.current_module = self.macro_def_scope(def),
                Some(None) => {}
                None => continue,
            };
            let result = self.resolve_ident_in_module_unadjusted(
                module, ident, ns, false, false, path_span,
            );
            self.current_module = orig_current_module;

            match result {
                Err(Determined) => continue,
                Ok(binding)
                    if !self.is_accessible_from(binding.vis, glob_import.parent) => continue,
                Ok(_) | Err(Undetermined) => return Err(Undetermined),
            }
        }

        // No resolution and no one else can define the name - determinate error.
        Err(Determined)
    }

    // Add an import directive to the current module.
    pub fn add_import_directive(&mut self,
                                module_path: Vec<Ident>,
                                subclass: ImportDirectiveSubclass<'a>,
                                span: Span,
                                id: NodeId,
                                root_span: Span,
                                root_id: NodeId,
                                vis: ty::Visibility,
                                expansion: Mark) {
        let current_module = self.current_module;
        let directive = self.arenas.alloc_import_directive(ImportDirective {
            parent: current_module,
            module_path,
            imported_module: Cell::new(None),
            subclass,
            span,
            id,
            root_span,
            root_id,
            vis: Cell::new(vis),
            expansion,
            used: Cell::new(false),
        });

        self.indeterminate_imports.push(directive);
        match directive.subclass {
            SingleImport { target, type_ns_only, .. } => {
                self.per_ns(|this, ns| if !type_ns_only || ns == TypeNS {
                    let mut resolution = this.resolution(current_module, target, ns).borrow_mut();
                    resolution.single_imports.insert(PtrKey(directive));
                });
            }
            // We don't add prelude imports to the globs since they only affect lexical scopes,
            // which are not relevant to import resolution.
            GlobImport { is_prelude: true, .. } => {}
            GlobImport { .. } => self.current_module.globs.borrow_mut().push(directive),
            _ => unreachable!(),
        }
    }

    // Given a binding and an import directive that resolves to it,
    // return the corresponding binding defined by the import directive.
    pub fn import(&self, binding: &'a NameBinding<'a>, directive: &'a ImportDirective<'a>)
                  -> &'a NameBinding<'a> {
        let vis = if binding.pseudo_vis().is_at_least(directive.vis.get(), self) ||
                     // c.f. `PUB_USE_OF_PRIVATE_EXTERN_CRATE`
                     !directive.is_glob() && binding.is_extern_crate() {
            directive.vis.get()
        } else {
            binding.pseudo_vis()
        };

        if let GlobImport { ref max_vis, .. } = directive.subclass {
            if vis == directive.vis.get() || vis.is_at_least(max_vis.get(), self) {
                max_vis.set(vis)
            }
        }

        self.arenas.alloc_name_binding(NameBinding {
            kind: NameBindingKind::Import {
                binding,
                directive,
                used: Cell::new(false),
            },
            span: directive.span,
            vis,
            expansion: directive.expansion,
        })
    }

    // Define the name or return the existing binding if there is a collision.
    pub fn try_define(&mut self,
                      module: Module<'a>,
                      ident: Ident,
                      ns: Namespace,
                      binding: &'a NameBinding<'a>)
                      -> Result<(), &'a NameBinding<'a>> {
        self.update_resolution(module, ident, ns, |this, resolution| {
            if let Some(old_binding) = resolution.binding {
                if binding.is_glob_import() {
                    if !old_binding.is_glob_import() &&
                       !(ns == MacroNS && old_binding.expansion != Mark::root()) {
                        resolution.shadowed_glob = Some(binding);
                    } else if binding.def() != old_binding.def() {
                        resolution.binding = Some(this.ambiguity(old_binding, binding));
                    } else if !old_binding.vis.is_at_least(binding.vis, &*this) {
                        // We are glob-importing the same item but with greater visibility.
                        resolution.binding = Some(binding);
                    }
                } else if old_binding.is_glob_import() {
                    if ns == MacroNS && binding.expansion != Mark::root() &&
                       binding.def() != old_binding.def() {
                        resolution.binding = Some(this.ambiguity(binding, old_binding));
                    } else {
                        resolution.binding = Some(binding);
                        resolution.shadowed_glob = Some(old_binding);
                    }
                } else {
                    return Err(old_binding);
                }
            } else {
                resolution.binding = Some(binding);
            }

            Ok(())
        })
    }

    pub fn ambiguity(&self, b1: &'a NameBinding<'a>, b2: &'a NameBinding<'a>)
                     -> &'a NameBinding<'a> {
        self.arenas.alloc_name_binding(NameBinding {
            kind: NameBindingKind::Ambiguity { b1, b2 },
            vis: if b1.vis.is_at_least(b2.vis, self) { b1.vis } else { b2.vis },
            span: b1.span,
            expansion: Mark::root(),
        })
    }

    // Use `f` to mutate the resolution of the name in the module.
    // If the resolution becomes a success, define it in the module's glob importers.
    fn update_resolution<T, F>(&mut self, module: Module<'a>, ident: Ident, ns: Namespace, f: F)
                               -> T
        where F: FnOnce(&mut Resolver<'a>, &mut NameResolution<'a>) -> T
    {
        // Ensure that `resolution` isn't borrowed when defining in the module's glob importers,
        // during which the resolution might end up getting re-defined via a glob cycle.
        let (binding, t) = {
            let resolution = &mut *self.resolution(module, ident, ns).borrow_mut();
            let old_binding = resolution.binding();

            let t = f(self, resolution);

            match resolution.binding() {
                _ if old_binding.is_some() => return t,
                None => return t,
                Some(binding) => match old_binding {
                    Some(old_binding) if ptr::eq(old_binding, binding) => return t,
                    _ => (binding, t),
                }
            }
        };

        // Define `binding` in `module`s glob importers.
        for directive in module.glob_importers.borrow_mut().iter() {
            let mut ident = ident.modern();
            let scope = match ident.span.reverse_glob_adjust(module.expansion,
                                                             directive.span.ctxt().modern()) {
                Some(Some(def)) => self.macro_def_scope(def),
                Some(None) => directive.parent,
                None => continue,
            };
            if self.is_accessible_from(binding.vis, scope) {
                let imported_binding = self.import(binding, directive);
                let _ = self.try_define(directive.parent, ident, ns, imported_binding);
            }
        }

        t
    }

    // Define a "dummy" resolution containing a Def::Err as a placeholder for a
    // failed resolution
    fn import_dummy_binding(&mut self, directive: &'a ImportDirective<'a>) {
        if let SingleImport { target, .. } = directive.subclass {
            let dummy_binding = self.dummy_binding;
            let dummy_binding = self.import(dummy_binding, directive);
            self.per_ns(|this, ns| {
                let _ = this.try_define(directive.parent, target, ns, dummy_binding);
            });
        }
    }
}

pub struct ImportResolver<'a, 'b: 'a> {
    pub resolver: &'a mut Resolver<'b>,
}

impl<'a, 'b: 'a> ::std::ops::Deref for ImportResolver<'a, 'b> {
    type Target = Resolver<'b>;
    fn deref(&self) -> &Resolver<'b> {
        self.resolver
    }
}

impl<'a, 'b: 'a> ::std::ops::DerefMut for ImportResolver<'a, 'b> {
    fn deref_mut(&mut self) -> &mut Resolver<'b> {
        self.resolver
    }
}

impl<'a, 'b: 'a> ty::DefIdTree for &'a ImportResolver<'a, 'b> {
    fn parent(self, id: DefId) -> Option<DefId> {
        self.resolver.parent(id)
    }
}

impl<'a, 'b:'a> ImportResolver<'a, 'b> {
    // Import resolution
    //
    // This is a fixed-point algorithm. We resolve imports until our efforts
    // are stymied by an unresolved import; then we bail out of the current
    // module and continue. We terminate successfully once no more imports
    // remain or unsuccessfully when no forward progress in resolving imports
    // is made.

    /// Resolves all imports for the crate. This method performs the fixed-
    /// point iteration.
    pub fn resolve_imports(&mut self) {
        let mut prev_num_indeterminates = self.indeterminate_imports.len() + 1;
        while self.indeterminate_imports.len() < prev_num_indeterminates {
            prev_num_indeterminates = self.indeterminate_imports.len();
            for import in mem::replace(&mut self.indeterminate_imports, Vec::new()) {
                match self.resolve_import(&import) {
                    true => self.determined_imports.push(import),
                    false => self.indeterminate_imports.push(import),
                }
            }
        }
    }

    pub fn finalize_imports(&mut self) {
        for module in self.arenas.local_modules().iter() {
            self.finalize_resolutions_in(module);
        }

        let mut errors = false;
        let mut seen_spans = FxHashSet();
        for i in 0 .. self.determined_imports.len() {
            let import = self.determined_imports[i];
            if let Some((span, err)) = self.finalize_import(import) {
                errors = true;

                if let SingleImport { source, ref result, .. } = import.subclass {
                    if source.name == "self" {
                        // Silence `unresolved import` error if E0429 is already emitted
                        match result.value_ns.get() {
                            Err(Determined) => continue,
                            _ => {},
                        }
                    }
                }

                // If the error is a single failed import then create a "fake" import
                // resolution for it so that later resolve stages won't complain.
                self.import_dummy_binding(import);
                if !seen_spans.contains(&span) {
                    let path = import_path_to_string(&import.module_path[..],
                                                     &import.subclass,
                                                     span);
                    let error = ResolutionError::UnresolvedImport(Some((span, &path, &err)));
                    resolve_error(self.resolver, span, error);
                    seen_spans.insert(span);
                }
            }
        }

        // Report unresolved imports only if no hard error was already reported
        // to avoid generating multiple errors on the same import.
        if !errors {
            if let Some(import) = self.indeterminate_imports.iter().next() {
                let error = ResolutionError::UnresolvedImport(None);
                resolve_error(self.resolver, import.span, error);
            }
        }
    }

    /// Attempts to resolve the given import, returning true if its resolution is determined.
    /// If successful, the resolved bindings are written into the module.
    fn resolve_import(&mut self, directive: &'b ImportDirective<'b>) -> bool {
        debug!("(resolving import for module) resolving import `{}::...` in `{}`",
               names_to_string(&directive.module_path[..]),
               module_to_string(self.current_module).unwrap_or("???".to_string()));

        self.current_module = directive.parent;

        let module = if let Some(module) = directive.imported_module.get() {
            module
        } else {
            let vis = directive.vis.get();
            // For better failure detection, pretend that the import will not define any names
            // while resolving its module path.
            directive.vis.set(ty::Visibility::Invisible);
            let result = self.resolve_path(&directive.module_path[..], None, false,
                                           directive.span, directive.crate_lint());
            directive.vis.set(vis);

            match result {
                PathResult::Module(module) => module,
                PathResult::Indeterminate => return false,
                _ => return true,
            }
        };

        directive.imported_module.set(Some(module));
        let (source, target, result, type_ns_only) = match directive.subclass {
            SingleImport { source, target, ref result, type_ns_only } =>
                (source, target, result, type_ns_only),
            GlobImport { .. } => {
                self.resolve_glob_import(directive);
                return true;
            }
            _ => unreachable!(),
        };

        let mut indeterminate = false;
        self.per_ns(|this, ns| if !type_ns_only || ns == TypeNS {
            if let Err(Undetermined) = result[ns].get() {
                result[ns].set(this.resolve_ident_in_module(module,
                                                            source,
                                                            ns,
                                                            false,
                                                            directive.span));
            } else {
                return
            };

            let parent = directive.parent;
            match result[ns].get() {
                Err(Undetermined) => indeterminate = true,
                Err(Determined) => {
                    this.update_resolution(parent, target, ns, |_, resolution| {
                        resolution.single_imports.remove(&PtrKey(directive));
                    });
                }
                Ok(binding) if !binding.is_importable() => {
                    let msg = format!("`{}` is not directly importable", target);
                    struct_span_err!(this.session, directive.span, E0253, "{}", &msg)
                        .span_label(directive.span, "cannot be imported directly")
                        .emit();
                    // Do not import this illegal binding. Import a dummy binding and pretend
                    // everything is fine
                    this.import_dummy_binding(directive);
                }
                Ok(binding) => {
                    let imported_binding = this.import(binding, directive);
                    let conflict = this.try_define(parent, target, ns, imported_binding);
                    if let Err(old_binding) = conflict {
                        this.report_conflict(parent, target, ns, imported_binding, old_binding);
                    }
                }
            }
        });

        !indeterminate
    }

    // If appropriate, returns an error to report.
    fn finalize_import(&mut self, directive: &'b ImportDirective<'b>) -> Option<(Span, String)> {
        self.current_module = directive.parent;
        let ImportDirective { ref module_path, span, .. } = *directive;
        let mut warn_if_binding_comes_from_local_crate = false;

        // FIXME: Last path segment is treated specially in import resolution, so extern crate
        // mode for absolute paths needs some special support for single-segment imports.
        if module_path.len() == 1 && (module_path[0].name == keywords::CrateRoot.name() ||
                                      module_path[0].name == keywords::Extern.name()) {
            let is_extern = module_path[0].name == keywords::Extern.name() ||
                            (self.session.features_untracked().extern_absolute_paths &&
                             self.session.rust_2018());
            match directive.subclass {
                GlobImport { .. } if is_extern => {
                    return Some((directive.span,
                                 "cannot glob-import all possible crates".to_string()));
                }
                GlobImport { .. } if self.session.features_untracked().extern_absolute_paths => {
                    self.lint_path_starts_with_module(
                        directive.root_id,
                        directive.root_span,
                    );
                }
                SingleImport { source, target, .. } => {
                    let crate_root = if source.name == keywords::Crate.name() &&
                                        module_path[0].name != keywords::Extern.name() {
                        if target.name == keywords::Crate.name() {
                            return Some((directive.span,
                                         "crate root imports need to be explicitly named: \
                                          `use crate as name;`".to_string()));
                        } else {
                            Some(self.resolve_crate_root(source))
                        }
                    } else if is_extern && !source.is_path_segment_keyword() {
                        let crate_id =
                            self.resolver.crate_loader.process_use_extern(
                                source.name,
                                directive.span,
                                directive.id,
                                &self.resolver.definitions,
                            );
                        let crate_root =
                            self.get_module(DefId { krate: crate_id, index: CRATE_DEF_INDEX });
                        self.populate_module_if_necessary(crate_root);
                        Some(crate_root)
                    } else {
                        warn_if_binding_comes_from_local_crate = true;
                        None
                    };

                    if let Some(crate_root) = crate_root {
                        let binding = (crate_root, ty::Visibility::Public, directive.span,
                                       directive.expansion).to_name_binding(self.arenas);
                        let binding = self.arenas.alloc_name_binding(NameBinding {
                            kind: NameBindingKind::Import {
                                binding,
                                directive,
                                used: Cell::new(false),
                            },
                            vis: directive.vis.get(),
                            span: directive.span,
                            expansion: directive.expansion,
                        });
                        let _ = self.try_define(directive.parent, target, TypeNS, binding);
                        return None;
                    }
                }
                _ => {}
            }
        }

        let module_result = self.resolve_path(
            &module_path,
            None,
            true,
            span,
            directive.crate_lint(),
        );
        let module = match module_result {
            PathResult::Module(module) => module,
            PathResult::Failed(span, msg, false) => {
                resolve_error(self, span, ResolutionError::FailedToResolve(&msg));
                return None;
            }
            PathResult::Failed(span, msg, true) => {
                let (mut self_path, mut self_result) = (module_path.clone(), None);
                let is_special = |ident: Ident| ident.is_path_segment_keyword() &&
                                                ident.name != keywords::CrateRoot.name();
                if !self_path.is_empty() && !is_special(self_path[0]) &&
                   !(self_path.len() > 1 && is_special(self_path[1])) {
                    self_path[0].name = keywords::SelfValue.name();
                    self_result = Some(self.resolve_path(&self_path, None, false,
                                                         span, CrateLint::No));
                }
                return if let Some(PathResult::Module(..)) = self_result {
                    Some((span, format!("Did you mean `{}`?", names_to_string(&self_path[..]))))
                } else {
                    Some((span, msg))
                };
            },
            _ => return None,
        };

        let (ident, result, type_ns_only) = match directive.subclass {
            SingleImport { source, ref result, type_ns_only, .. } => (source, result, type_ns_only),
            GlobImport { .. } if module.def_id() == directive.parent.def_id() => {
                // Importing a module into itself is not allowed.
                return Some((directive.span,
                             "Cannot glob-import a module into itself.".to_string()));
            }
            GlobImport { is_prelude, ref max_vis } => {
                if !is_prelude &&
                   max_vis.get() != ty::Visibility::Invisible && // Allow empty globs.
                   !max_vis.get().is_at_least(directive.vis.get(), &*self) {
                    let msg = "A non-empty glob must import something with the glob's visibility";
                    self.session.span_err(directive.span, msg);
                }
                return None;
            }
            _ => unreachable!(),
        };

        let mut all_ns_err = true;
        self.per_ns(|this, ns| if !type_ns_only || ns == TypeNS {
            if let Ok(binding) = result[ns].get() {
                all_ns_err = false;
                if this.record_use(ident, ns, binding, directive.span) {
                    this.resolution(module, ident, ns).borrow_mut().binding =
                        Some(this.dummy_binding);
                }
            }
        });

        if all_ns_err {
            let mut all_ns_failed = true;
            self.per_ns(|this, ns| if !type_ns_only || ns == TypeNS {
                match this.resolve_ident_in_module(module, ident, ns, true, span) {
                    Ok(_) => all_ns_failed = false,
                    _ => {}
                }
            });

            return if all_ns_failed {
                let resolutions = module.resolutions.borrow();
                let names = resolutions.iter().filter_map(|(&(ref i, _), resolution)| {
                    if *i == ident { return None; } // Never suggest the same name
                    match *resolution.borrow() {
                        NameResolution { binding: Some(name_binding), .. } => {
                            match name_binding.kind {
                                NameBindingKind::Import { binding, .. } => {
                                    match binding.kind {
                                        // Never suggest the name that has binding error
                                        // i.e. the name that cannot be previously resolved
                                        NameBindingKind::Def(Def::Err) => return None,
                                        _ => Some(&i.name),
                                    }
                                },
                                _ => Some(&i.name),
                            }
                        },
                        NameResolution { ref single_imports, .. }
                            if single_imports.is_empty() => None,
                        _ => Some(&i.name),
                    }
                });
                let lev_suggestion =
                    match find_best_match_for_name(names, &ident.as_str(), None) {
                        Some(name) => format!(". Did you mean to use `{}`?", name),
                        None => "".to_owned(),
                    };
                let module_str = module_to_string(module);
                let msg = if let Some(module_str) = module_str {
                    format!("no `{}` in `{}`{}", ident, module_str, lev_suggestion)
                } else {
                    format!("no `{}` in the root{}", ident, lev_suggestion)
                };
                Some((span, msg))
            } else {
                // `resolve_ident_in_module` reported a privacy error.
                self.import_dummy_binding(directive);
                None
            }
        }

        let mut reexport_error = None;
        let mut any_successful_reexport = false;
        self.per_ns(|this, ns| {
            if let Ok(binding) = result[ns].get() {
                let vis = directive.vis.get();
                if !binding.pseudo_vis().is_at_least(vis, &*this) {
                    reexport_error = Some((ns, binding));
                } else {
                    any_successful_reexport = true;
                }
            }
        });

        // All namespaces must be re-exported with extra visibility for an error to occur.
        if !any_successful_reexport {
            let (ns, binding) = reexport_error.unwrap();
            if ns == TypeNS && binding.is_extern_crate() {
                let msg = format!("extern crate `{}` is private, and cannot be \
                                   re-exported (error E0365), consider declaring with \
                                   `pub`",
                                   ident);
                self.session.buffer_lint(PUB_USE_OF_PRIVATE_EXTERN_CRATE,
                                         directive.id,
                                         directive.span,
                                         &msg);
            } else if ns == TypeNS {
                struct_span_err!(self.session, directive.span, E0365,
                                 "`{}` is private, and cannot be re-exported", ident)
                    .span_label(directive.span, format!("re-export of private `{}`", ident))
                    .note(&format!("consider declaring type or module `{}` with `pub`", ident))
                    .emit();
            } else {
                let msg = format!("`{}` is private, and cannot be re-exported", ident);
                let note_msg =
                    format!("consider marking `{}` as `pub` in the imported module", ident);
                struct_span_err!(self.session, directive.span, E0364, "{}", &msg)
                    .span_note(directive.span, &note_msg)
                    .emit();
            }
        }

        if warn_if_binding_comes_from_local_crate {
            let mut warned = false;
            self.per_ns(|this, ns| {
                let binding = match result[ns].get().ok() {
                    Some(b) => b,
                    None => return
                };
                if let NameBindingKind::Import { directive: d, .. } = binding.kind {
                    if let ImportDirectiveSubclass::ExternCrate(..) = d.subclass {
                        return
                    }
                }
                if warned {
                    return
                }
                warned = true;
                this.lint_path_starts_with_module(
                    directive.root_id,
                    directive.root_span,
                );
            });
        }

        // Record what this import resolves to for later uses in documentation,
        // this may resolve to either a value or a type, but for documentation
        // purposes it's good enough to just favor one over the other.
        self.per_ns(|this, ns| if let Some(binding) = result[ns].get().ok() {
            let import = this.import_map.entry(directive.id).or_default();
            import[ns] = Some(PathResolution::new(binding.def()));
        });

        debug!("(resolving single import) successfully resolved import");
        None
    }

    fn resolve_glob_import(&mut self, directive: &'b ImportDirective<'b>) {
        let module = directive.imported_module.get().unwrap();
        self.populate_module_if_necessary(module);

        if let Some(Def::Trait(_)) = module.def() {
            self.session.span_err(directive.span, "items in traits are not importable.");
            return;
        } else if module.def_id() == directive.parent.def_id()  {
            return;
        } else if let GlobImport { is_prelude: true, .. } = directive.subclass {
            self.prelude = Some(module);
            return;
        }

        // Add to module's glob_importers
        module.glob_importers.borrow_mut().push(directive);

        // Ensure that `resolutions` isn't borrowed during `try_define`,
        // since it might get updated via a glob cycle.
        let bindings = module.resolutions.borrow().iter().filter_map(|(&ident, resolution)| {
            resolution.borrow().binding().map(|binding| (ident, binding))
        }).collect::<Vec<_>>();
        for ((mut ident, ns), binding) in bindings {
            let scope = match ident.span.reverse_glob_adjust(module.expansion,
                                                             directive.span.ctxt().modern()) {
                Some(Some(def)) => self.macro_def_scope(def),
                Some(None) => self.current_module,
                None => continue,
            };
            if self.is_accessible_from(binding.pseudo_vis(), scope) {
                let imported_binding = self.import(binding, directive);
                let _ = self.try_define(directive.parent, ident, ns, imported_binding);
            }
        }

        // Record the destination of this import
        self.record_def(directive.id, PathResolution::new(module.def().unwrap()));
    }

    // Miscellaneous post-processing, including recording re-exports,
    // reporting conflicts, and reporting unresolved imports.
    fn finalize_resolutions_in(&mut self, module: Module<'b>) {
        // Since import resolution is finished, globs will not define any more names.
        *module.globs.borrow_mut() = Vec::new();

        let mut reexports = Vec::new();
        let mut exported_macro_names = FxHashMap();
        if ptr::eq(module, self.graph_root) {
            let macro_exports = mem::replace(&mut self.macro_exports, Vec::new());
            for export in macro_exports.into_iter().rev() {
                if let Some(later_span) = exported_macro_names.insert(export.ident.modern(),
                                                                      export.span) {
                    self.session.buffer_lint_with_diagnostic(
                        DUPLICATE_MACRO_EXPORTS,
                        CRATE_NODE_ID,
                        later_span,
                        &format!("a macro named `{}` has already been exported", export.ident),
                        BuiltinLintDiagnostics::DuplicatedMacroExports(
                            export.ident, export.span, later_span));
                } else {
                    reexports.push(export);
                }
            }
        }

        for (&(ident, ns), resolution) in module.resolutions.borrow().iter() {
            let resolution = &mut *resolution.borrow_mut();
            let binding = match resolution.binding {
                Some(binding) => binding,
                None => continue,
            };

            if binding.is_import() || binding.is_macro_def() {
                let def = binding.def();
                if def != Def::Err {
                    if !def.def_id().is_local() {
                        self.cstore.export_macros_untracked(def.def_id().krate);
                    }
                    if let Def::Macro(..) = def {
                        if let Some(&span) = exported_macro_names.get(&ident.modern()) {
                            let msg =
                                format!("a macro named `{}` has already been exported", ident);
                            self.session.struct_span_err(span, &msg)
                                .span_label(span, format!("`{}` already exported", ident))
                                .span_note(binding.span, "previous macro export here")
                                .emit();
                        }
                    }
                    reexports.push(Export {
                        ident: ident.modern(),
                        def: def,
                        span: binding.span,
                        vis: binding.vis,
                    });
                }
            }

            match binding.kind {
                NameBindingKind::Import { binding: orig_binding, directive, .. } => {
                    if ns == TypeNS && orig_binding.is_variant() &&
                        !orig_binding.vis.is_at_least(binding.vis, &*self) {
                            let msg = match directive.subclass {
                                ImportDirectiveSubclass::SingleImport { .. } => {
                                    format!("variant `{}` is private and cannot be re-exported",
                                            ident)
                                },
                                ImportDirectiveSubclass::GlobImport { .. } => {
                                    let msg = "enum is private and its variants \
                                               cannot be re-exported".to_owned();
                                    let error_id = (DiagnosticMessageId::ErrorId(0), // no code?!
                                                    Some(binding.span),
                                                    msg.clone());
                                    let fresh = self.session.one_time_diagnostics
                                        .borrow_mut().insert(error_id);
                                    if !fresh {
                                        continue;
                                    }
                                    msg
                                },
                                ref s @ _ => bug!("unexpected import subclass {:?}", s)
                            };
                            let mut err = self.session.struct_span_err(binding.span, &msg);

                            let imported_module = directive.imported_module.get()
                                .expect("module should exist");
                            let resolutions = imported_module.parent.expect("parent should exist")
                                .resolutions.borrow();
                            let enum_path_segment_index = directive.module_path.len() - 1;
                            let enum_ident = directive.module_path[enum_path_segment_index];

                            let enum_resolution = resolutions.get(&(enum_ident, TypeNS))
                                .expect("resolution should exist");
                            let enum_span = enum_resolution.borrow()
                                .binding.expect("binding should exist")
                                .span;
                            let enum_def_span = self.session.codemap().def_span(enum_span);
                            let enum_def_snippet = self.session.codemap()
                                .span_to_snippet(enum_def_span).expect("snippet should exist");
                            // potentially need to strip extant `crate`/`pub(path)` for suggestion
                            let after_vis_index = enum_def_snippet.find("enum")
                                .expect("`enum` keyword should exist in snippet");
                            let suggestion = format!("pub {}",
                                                     &enum_def_snippet[after_vis_index..]);

                            self.session
                                .diag_span_suggestion_once(&mut err,
                                                           DiagnosticMessageId::ErrorId(0),
                                                           enum_def_span,
                                                           "consider making the enum public",
                                                           suggestion);
                            err.emit();
                    }
                }
                _ => {}
            }
        }

        if reexports.len() > 0 {
            if let Some(def_id) = module.def_id() {
                self.export_map.insert(def_id, reexports);
            }
        }
    }
}

fn import_path_to_string(names: &[Ident],
                         subclass: &ImportDirectiveSubclass,
                         span: Span) -> String {
    let pos = names.iter()
        .position(|p| span == p.span && p.name != keywords::CrateRoot.name());
    let global = !names.is_empty() && names[0].name == keywords::CrateRoot.name();
    if let Some(pos) = pos {
        let names = if global { &names[1..pos + 1] } else { &names[..pos + 1] };
        names_to_string(names)
    } else {
        let names = if global { &names[1..] } else { names };
        if names.is_empty() {
            import_directive_subclass_to_string(subclass)
        } else {
            format!("{}::{}",
                    names_to_string(names),
                    import_directive_subclass_to_string(subclass))
        }
    }
}

fn import_directive_subclass_to_string(subclass: &ImportDirectiveSubclass) -> String {
    match *subclass {
        SingleImport { source, .. } => source.to_string(),
        GlobImport { .. } => "*".to_string(),
        ExternCrate(_) => "<extern crate>".to_string(),
        MacroUse => "#[macro_use]".to_string(),
    }
}
