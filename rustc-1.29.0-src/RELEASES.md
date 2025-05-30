Version 1.29.0 (2018-09-13)
==========================

Compiler
--------
- [Bumped minimum LLVM version to 5.0.][51899]
- [Added `powerpc64le-unknown-linux-musl` target.][51619]
- [Added `aarch64-unknown-hermit` and `x86_64-unknown-hermit` targets.][52861]

Libraries
---------
- [`Once::call_once` now no longer requires `Once` to be `'static`.][52239]
- [`BuildHasherDefault` now implements `PartialEq` and `Eq`.][52402]
- [`Box<CStr>`, `Box<OsStr>`, and `Box<Path>` now implement `Clone`.][51912]
- [Implemented `PartialEq<&str>` for `OsString` and `PartialEq<OsString>`
  for `&str`.][51178]
- [`Cell<T>` now allows `T` to be unsized.][50494]
- [`SocketAddr` is now stable on Redox.][52656]

Stabilized APIs
---------------
- [`Arc::downcast`]
- [`Iterator::flatten`]
- [`Rc::downcast`]

Cargo
-----
- [Cargo can silently fix some bad lockfiles ][cargo/5831] You can use
  `--locked` to disable this behaviour.
- [`cargo-install` will now allow you to cross compile an install
  using `--target`][cargo/5614]
- [Added the `cargo-fix` subcommand to automatically move project code from
  2015 edition to 2018.][cargo/5723]

Misc
----
- [`rustdoc` now has the `--cap-lints` option which demotes all lints above
  the specified level to that level.][52354] For example `--cap-lints warn`
  will demote `deny` and `forbid` lints to `warn`.
- [`rustc` and `rustdoc` will now have the exit code of `1` if compilation
  fails, and `101` if there is a panic.][52197]
- [A preview of clippy has been made available through rustup.][51122]
  You can install the preview with `rustup component add clippy-preview`

Compatibility Notes
-------------------
- [`str::{slice_unchecked, slice_unchecked_mut}` are now deprecated.][51807]
  Use `str::get_unchecked(begin..end)` instead.
- [`std::env::home_dir` is now deprecated for its unintuitive behaviour.][51656]
  Consider using the `home_dir` function from
  https://crates.io/crates/dirs instead.
- [`rustc` will no longer silently ignore invalid data in target spec.][52330]

[52861]: https://github.com/rust-lang/rust/pull/52861/
[52656]: https://github.com/rust-lang/rust/pull/52656/
[52239]: https://github.com/rust-lang/rust/pull/52239/
[52330]: https://github.com/rust-lang/rust/pull/52330/
[52354]: https://github.com/rust-lang/rust/pull/52354/
[52402]: https://github.com/rust-lang/rust/pull/52402/
[52103]: https://github.com/rust-lang/rust/pull/52103/
[52197]: https://github.com/rust-lang/rust/pull/52197/
[51807]: https://github.com/rust-lang/rust/pull/51807/
[51899]: https://github.com/rust-lang/rust/pull/51899/
[51912]: https://github.com/rust-lang/rust/pull/51912/
[51511]: https://github.com/rust-lang/rust/pull/51511/
[51619]: https://github.com/rust-lang/rust/pull/51619/
[51656]: https://github.com/rust-lang/rust/pull/51656/
[51178]: https://github.com/rust-lang/rust/pull/51178/
[51122]: https://github.com/rust-lang/rust/pull/51122
[50494]: https://github.com/rust-lang/rust/pull/50494/
[cargo/5614]: https://github.com/rust-lang/cargo/pull/5614/
[cargo/5723]: https://github.com/rust-lang/cargo/pull/5723/
[cargo/5831]: https://github.com/rust-lang/cargo/pull/5831/
[`Arc::downcast`]: https://doc.rust-lang.org/std/sync/struct.Arc.html#method.downcast
[`Iterator::flatten`]: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.flatten
[`Rc::downcast`]: https://doc.rust-lang.org/std/rc/struct.Rc.html#method.downcast


Version 1.28.0 (2018-08-02)
===========================

Language
--------
- [The `#[repr(transparent)]` attribute is now stable.][51562] This attribute
  allows a Rust newtype wrapper (`struct NewType<T>(T);`) to be represented as
  the inner type across Foreign Function Interface (FFI) boundaries.
- [The keywords `pure`, `sizeof`, `alignof`, and `offsetof` have been unreserved
  and can now be used as identifiers.][51196]
- [The `GlobalAlloc` trait and `#[global_allocator]` attribute are now
  stable.][51241] This will allow users to specify a global allocator for
  their program.
- [Unit test functions marked with the `#[test]` attribute can now return
  `Result<(), E: Debug>` in addition to `()`.][51298]
- [The `lifetime` specifier for `macro_rules!` is now stable.][50385] This
  allows macros to easily target lifetimes.

Compiler
--------
- [The `s` and `z` optimisation levels are now stable.][50265] These optimisations
  prioritise making smaller binary sizes. `z` is the same as `s` with the
  exception that it does not vectorise loops, which typically results in an even
  smaller binary.
- [The short error format is now stable.][49546] Specified with
  `--error-format=short` this option will provide a more compressed output of
  rust error messages.
- [Added a lint warning when you have duplicated `macro_export`s.][50143]
- [Reduced the number of allocations in the macro parser.][50855] This can
  improve compile times of macro heavy crates on average by 5%.

Libraries
---------
- [Implemented `Default` for `&mut str`.][51306]
- [Implemented `From<bool>` for all integer and unsigned number types.][50554]
- [Implemented `Extend` for `()`.][50234]
- [The `Debug` implementation of `time::Duration` should now be more easily
  human readable.][50364] Previously a `Duration` of one second would printed as
  `Duration { secs: 1, nanos: 0 }` and will now be printed as `1s`.
- [Implemented `From<&String>` for `Cow<str>`, `From<&Vec<T>>` for `Cow<[T]>`,
  `From<Cow<CStr>>` for `CString`, `From<CString>, From<CStr>, From<&CString>`
  for `Cow<CStr>`, `From<OsString>, From<OsStr>, From<&OsString>` for
  `Cow<OsStr>`, `From<&PathBuf>` for `Cow<Path>`, and `From<Cow<Path>>`
  for `PathBuf`.][50170]
- [Implemented `Shl` and `Shr` for `Wrapping<u128>`
  and `Wrapping<i128>`.][50465]
- [`DirEntry::metadata` now uses `fstatat` instead of `lstat` when
  possible.][51050] This can provide up to a 40% speed increase.
- [Improved error messages when using `format!`.][50610]

Stabilized APIs
---------------
- [`Iterator::step_by`]
- [`Path::ancestors`]
- [`SystemTime::UNIX_EPOCH`]
- [`alloc::GlobalAlloc`]
- [`alloc::Layout`]
- [`alloc::LayoutErr`]
- [`alloc::System`]
- [`alloc::alloc`]
- [`alloc::alloc_zeroed`]
- [`alloc::dealloc`]
- [`alloc::realloc`]
- [`alloc::handle_alloc_error`]
- [`btree_map::Entry::or_default`]
- [`fmt::Alignment`]
- [`hash_map::Entry::or_default`]
- [`iter::repeat_with`]
- [`num::NonZeroUsize`]
- [`num::NonZeroU128`]
- [`num::NonZeroU16`]
- [`num::NonZeroU32`]
- [`num::NonZeroU64`]
- [`num::NonZeroU8`]
- [`ops::RangeBounds`]
- [`slice::SliceIndex`]
- [`slice::from_mut`]
- [`slice::from_ref`]
- [`{Any + Send + Sync}::downcast_mut`]
- [`{Any + Send + Sync}::downcast_ref`]
- [`{Any + Send + Sync}::is`]

Cargo
-----
- [Cargo will now no longer allow you to publish crates with build scripts that
  modify the `src` directory.][cargo/5584] The `src` directory in a crate should be
  considered to be immutable.

Misc
----
- [The `suggestion_applicability` field in `rustc`'s json output is now
  stable.][50486] This will allow dev tools to check whether a code suggestion
  would apply to them.

Compatibility Notes
-------------------
- [Rust will consider trait objects with duplicated constraints to be the same
  type as without the duplicated constraint.][51276] For example the below code will
  now fail to compile.
  ```rust
  trait Trait {}

  impl Trait + Send {
      fn test(&self) { println!("one"); } //~ ERROR duplicate definitions with name `test`
  }

  impl Trait + Send + Send {
      fn test(&self) { println!("two"); }
  }
  ```

[49546]: https://github.com/rust-lang/rust/pull/49546/
[50143]: https://github.com/rust-lang/rust/pull/50143/
[50170]: https://github.com/rust-lang/rust/pull/50170/
[50234]: https://github.com/rust-lang/rust/pull/50234/
[50265]: https://github.com/rust-lang/rust/pull/50265/
[50364]: https://github.com/rust-lang/rust/pull/50364/
[50385]: https://github.com/rust-lang/rust/pull/50385/
[50465]: https://github.com/rust-lang/rust/pull/50465/
[50486]: https://github.com/rust-lang/rust/pull/50486/
[50554]: https://github.com/rust-lang/rust/pull/50554/
[50610]: https://github.com/rust-lang/rust/pull/50610/
[50855]: https://github.com/rust-lang/rust/pull/50855/
[51050]: https://github.com/rust-lang/rust/pull/51050/
[51196]: https://github.com/rust-lang/rust/pull/51196/
[51200]: https://github.com/rust-lang/rust/pull/51200/
[51241]: https://github.com/rust-lang/rust/pull/51241/
[51276]: https://github.com/rust-lang/rust/pull/51276/
[51298]: https://github.com/rust-lang/rust/pull/51298/
[51306]: https://github.com/rust-lang/rust/pull/51306/
[51562]: https://github.com/rust-lang/rust/pull/51562/
[cargo/5584]: https://github.com/rust-lang/cargo/pull/5584/
[`Iterator::step_by`]: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.step_by
[`Path::ancestors`]: https://doc.rust-lang.org/std/path/struct.Path.html#method.ancestors
[`SystemTime::UNIX_EPOCH`]: https://doc.rust-lang.org/std/time/struct.SystemTime.html#associatedconstant.UNIX_EPOCH
[`alloc::GlobalAlloc`]: https://doc.rust-lang.org/std/alloc/trait.GlobalAlloc.html
[`alloc::Layout`]: https://doc.rust-lang.org/std/alloc/struct.Layout.html
[`alloc::LayoutErr`]: https://doc.rust-lang.org/std/alloc/struct.LayoutErr.html
[`alloc::System`]: https://doc.rust-lang.org/std/alloc/struct.System.html
[`alloc::alloc`]: https://doc.rust-lang.org/std/alloc/fn.alloc.html
[`alloc::alloc_zeroed`]: https://doc.rust-lang.org/std/alloc/fn.alloc_zeroed.html
[`alloc::dealloc`]: https://doc.rust-lang.org/std/alloc/fn.dealloc.html
[`alloc::realloc`]: https://doc.rust-lang.org/std/alloc/fn.realloc.html
[`alloc::handle_alloc_error`]: https://doc.rust-lang.org/std/alloc/fn.handle_alloc_error.html
[`btree_map::Entry::or_default`]: https://doc.rust-lang.org/std/collections/btree_map/enum.Entry.html#method.or_default
[`fmt::Alignment`]: https://doc.rust-lang.org/std/fmt/enum.Alignment.html
[`hash_map::Entry::or_default`]: https://doc.rust-lang.org/std/collections/hash_map/enum.Entry.html#method.or_default
[`iter::repeat_with`]: https://doc.rust-lang.org/std/iter/fn.repeat_with.html
[`num::NonZeroUsize`]: https://doc.rust-lang.org/std/num/struct.NonZeroUsize.html
[`num::NonZeroU128`]: https://doc.rust-lang.org/std/num/struct.NonZeroU128.html
[`num::NonZeroU16`]: https://doc.rust-lang.org/std/num/struct.NonZeroU16.html
[`num::NonZeroU32`]: https://doc.rust-lang.org/std/num/struct.NonZeroU32.html
[`num::NonZeroU64`]: https://doc.rust-lang.org/std/num/struct.NonZeroU64.html
[`num::NonZeroU8`]: https://doc.rust-lang.org/std/num/struct.NonZeroU8.html
[`ops::RangeBounds`]: https://doc.rust-lang.org/std/ops/trait.RangeBounds.html
[`slice::SliceIndex`]: https://doc.rust-lang.org/std/slice/trait.SliceIndex.html
[`slice::from_mut`]: https://doc.rust-lang.org/std/slice/fn.from_mut.html
[`slice::from_ref`]: https://doc.rust-lang.org/std/slice/fn.from_ref.html
[`{Any + Send + Sync}::downcast_mut`]: https://doc.rust-lang.org/std/any/trait.Any.html#method.downcast_mut-2
[`{Any + Send + Sync}::downcast_ref`]: https://doc.rust-lang.org/std/any/trait.Any.html#method.downcast_ref-2
[`{Any + Send + Sync}::is`]: https://doc.rust-lang.org/std/any/trait.Any.html#method.is-2

Version 1.27.2 (2018-07-20)
===========================

Compatibility Notes
-------------------

- The borrow checker was fixed to avoid potential unsoundness when using
  match ergonomics: [#52213][52213].

[52213]: https://github.com/rust-lang/rust/issues/52213

Version 1.27.1 (2018-07-10)
===========================

Security Notes
--------------

- rustdoc would execute plugins in the /tmp/rustdoc/plugins directory
  when running, which enabled executing code as some other user on a
  given machine. This release fixes that vulnerability; you can read
  more about this on the [blog][rustdoc-sec]. The associated CVE is [CVE-2018-1000622].

  Thank you to Red Hat for responsibily disclosing this vulnerability to us.

Compatibility Notes
-------------------

- The borrow checker was fixed to avoid an additional potential unsoundness when using
  match ergonomics: [#51415][51415], [#49534][49534].

[51415]: https://github.com/rust-lang/rust/issues/51415
[49534]: https://github.com/rust-lang/rust/issues/49534
[rustdoc-sec]: https://blog.rust-lang.org/2018/07/06/security-advisory-for-rustdoc.html
[CVE-2018-1000622]: https://cve.mitre.org/cgi-bin/cvename.cgi?name=%20CVE-2018-1000622

Version 1.27.0 (2018-06-21)
==========================

Language
--------
- [Removed 'proc' from the reserved keywords list.][49699] This allows `proc` to
  be used as an identifier.
- [The dyn syntax is now available.][49968] This syntax is equivalent to the
  bare `Trait` syntax, and should make it clearer when being used in tandem with
  `impl Trait`. Since it is equivalent to the following syntax:
  `&Trait == &dyn Trait`, `&mut Trait == &mut dyn Trait`, and
  `Box<Trait> == Box<dyn Trait>`.
- [Attributes on generic parameters such as types and lifetimes are
  now stable.][48851] e.g.
  `fn foo<#[lifetime_attr] 'a, #[type_attr] T: 'a>() {}`
- [The `#[must_use]` attribute can now also be used on functions as well as
  types.][48925] It provides a lint that by default warns users when the
  value returned by a function has not been used.

Compiler
--------
- [Added the `armv5te-unknown-linux-musl` target.][50423]

Libraries
---------
- [SIMD (Single Instruction Multiple Data) on x86/x86_64 is now stable.][49664]
  This includes [`arch::x86`] & [`arch::x86_64`] modules which contain
  SIMD intrinsics, a new macro called `is_x86_feature_detected!`, the
  `#[target_feature(enable="")]` attribute, and adding `target_feature = ""` to
  the `cfg` attribute.
- [A lot of methods for `[u8]`, `f32`, and `f64` previously only available in
  std are now available in core.][49896]
- [The generic `Rhs` type parameter on `ops::{Shl, ShlAssign, Shr}` now defaults
  to `Self`.][49630]
- [`std::str::replace` now has the `#[must_use]` attribute][50177] to clarify
  that the operation isn't done in place.
- [`Clone::clone`, `Iterator::collect`, and `ToOwned::to_owned` now have
  the `#[must_use]` attribute][49533] to warn about unused potentially
  expensive allocations.

Stabilized APIs
---------------
- [`DoubleEndedIterator::rfind`]
- [`DoubleEndedIterator::rfold`]
- [`DoubleEndedIterator::try_rfold`]
- [`Duration::from_micros`]
- [`Duration::from_nanos`]
- [`Duration::subsec_micros`]
- [`Duration::subsec_millis`]
- [`HashMap::remove_entry`]
- [`Iterator::try_fold`]
- [`Iterator::try_for_each`]
- [`NonNull::cast`]
- [`Option::filter`]
- [`String::replace_range`]
- [`Take::set_limit`]
- [`hint::unreachable_unchecked`]
- [`os::unix::process::parent_id`]
- [`ptr::swap_nonoverlapping`]
- [`slice::rsplit_mut`]
- [`slice::rsplit`]
- [`slice::swap_with_slice`]

Cargo
-----
- [`cargo-metadata` now includes `authors`, `categories`, `keywords`,
  `readme`, and `repository` fields.][cargo/5386]
- [`cargo-metadata` now includes a package's `metadata` table.][cargo/5360]
- [Added the `--target-dir` optional argument.][cargo/5393] This allows you to specify
  a different directory than `target` for placing compilation artifacts.
- [Cargo will be adding automatic target inference for binaries, benchmarks,
  examples, and tests in the Rust 2018 edition.][cargo/5335] If your project specifies
  specific targets e.g. using `[[bin]]` and have other binaries in locations
  where cargo would infer a binary, Cargo will produce a warning. You can
  disable this feature ahead of time by setting any of the following `autobins`,
  `autobenches`, `autoexamples`, `autotests` to false.
- [Cargo will now cache compiler information.][cargo/5359] This can be disabled by
  setting `CARGO_CACHE_RUSTC_INFO=0` in your environment.

Misc
----
- [Added “The Rustc book” into the official documentation.][49707]
  [“The Rustc book”] documents and teaches how to use the rustc compiler.
- [All books available on `doc.rust-lang.org` are now searchable.][49623]

Compatibility Notes
-------------------
- [Calling a `CharExt` or `StrExt` method directly on core will no longer
  work.][49896] e.g. `::core::prelude::v1::StrExt::is_empty("")` will not
  compile, `"".is_empty()` will still compile.
- [`Debug` output on `atomic::{AtomicBool, AtomicIsize, AtomicPtr, AtomicUsize}`
  will only print the inner type.][48553] e.g.
  `print!("{:?}", AtomicBool::new(true))` will print `true`
  not `AtomicBool(true)`.
- [The maximum number for `repr(align(N))` is now 2²⁹.][50378] Previously you
  could enter higher numbers but they were not supported by LLVM. Up to 512MB
  alignment should cover all use cases.

[48553]: https://github.com/rust-lang/rust/pull/48553/
[48851]: https://github.com/rust-lang/rust/pull/48851/
[48925]: https://github.com/rust-lang/rust/pull/48925/
[49533]: https://github.com/rust-lang/rust/pull/49533/
[49623]: https://github.com/rust-lang/rust/pull/49623/
[49630]: https://github.com/rust-lang/rust/pull/49630/
[49664]: https://github.com/rust-lang/rust/pull/49664/
[49699]: https://github.com/rust-lang/rust/pull/49699/
[49707]: https://github.com/rust-lang/rust/pull/49707/
[49719]: https://github.com/rust-lang/rust/pull/49719/
[49896]: https://github.com/rust-lang/rust/pull/49896/
[49968]: https://github.com/rust-lang/rust/pull/49968/
[50177]: https://github.com/rust-lang/rust/pull/50177/
[50378]: https://github.com/rust-lang/rust/pull/50378/
[50398]: https://github.com/rust-lang/rust/pull/50398/
[50423]: https://github.com/rust-lang/rust/pull/50423/
[cargo/5203]: https://github.com/rust-lang/cargo/pull/5203/
[cargo/5335]: https://github.com/rust-lang/cargo/pull/5335/
[cargo/5359]: https://github.com/rust-lang/cargo/pull/5359/
[cargo/5360]: https://github.com/rust-lang/cargo/pull/5360/
[cargo/5386]: https://github.com/rust-lang/cargo/pull/5386/
[cargo/5393]: https://github.com/rust-lang/cargo/pull/5393/
[`DoubleEndedIterator::rfind`]: https://doc.rust-lang.org/std/iter/trait.DoubleEndedIterator.html#method.rfind
[`DoubleEndedIterator::rfold`]: https://doc.rust-lang.org/std/iter/trait.DoubleEndedIterator.html#method.rfold
[`DoubleEndedIterator::try_rfold`]: https://doc.rust-lang.org/std/iter/trait.DoubleEndedIterator.html#method.try_rfold
[`Duration::from_micros`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.from_micros
[`Duration::from_nanos`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.from_nanos
[`Duration::subsec_micros`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.subsec_micros
[`Duration::subsec_millis`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.subsec_millis
[`HashMap::remove_entry`]: https://doc.rust-lang.org/std/collections/struct.HashMap.html#method.remove_entry
[`Iterator::try_fold`]: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.try_fold
[`Iterator::try_for_each`]: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.try_for_each
[`NonNull::cast`]: https://doc.rust-lang.org/std/ptr/struct.NonNull.html#method.cast
[`Option::filter`]: https://doc.rust-lang.org/std/option/enum.Option.html#method.filter
[`String::replace_range`]: https://doc.rust-lang.org/std/string/struct.String.html#method.replace_range
[`Take::set_limit`]: https://doc.rust-lang.org/std/io/struct.Take.html#method.set_limit
[`hint::unreachable_unchecked`]: https://doc.rust-lang.org/std/hint/fn.unreachable_unchecked.html
[`os::unix::process::parent_id`]: https://doc.rust-lang.org/std/os/unix/process/fn.parent_id.html
[`process::id`]: https://doc.rust-lang.org/std/process/fn.id.html
[`ptr::swap_nonoverlapping`]: https://doc.rust-lang.org/std/ptr/fn.swap_nonoverlapping.html
[`slice::rsplit_mut`]: https://doc.rust-lang.org/std/primitive.slice.html#method.rsplit_mut
[`slice::rsplit`]: https://doc.rust-lang.org/std/primitive.slice.html#method.rsplit
[`slice::swap_with_slice`]: https://doc.rust-lang.org/std/primitive.slice.html#method.swap_with_slice
[`arch::x86_64`]: https://doc.rust-lang.org/std/arch/x86_64/index.html
[`arch::x86`]: https://doc.rust-lang.org/std/arch/x86/index.html
[“The Rustc book”]: https://doc.rust-lang.org/rustc


Version 1.26.2 (2018-06-05)
==========================

Compatibility Notes
-------------------

- [The borrow checker was fixed to avoid unsoundness when using match ergonomics][51117]

[51117]: https://github.com/rust-lang/rust/issues/51117


Version 1.26.1 (2018-05-29)
==========================

Tools
-----

- [RLS now works on Windows][50646]
- [Rustfmt stopped badly formatting text in some cases][rustfmt/2695]


Compatibility Notes
--------

- [`fn main() -> impl Trait` no longer works for non-Termination
  trait][50656]
  This reverts an accidental stabilization.
- [`NaN > NaN` no longer returns true in const-fn contexts][50812]
- [Prohibit using turbofish for `impl Trait` in method arguments][50950]

[50646]: https://github.com/rust-lang/rust/issues/50646
[50656]: https://github.com/rust-lang/rust/pull/50656
[50812]: https://github.com/rust-lang/rust/pull/50812
[50950]: https://github.com/rust-lang/rust/issues/50950
[rustfmt/2695]: https://github.com/rust-lang-nursery/rustfmt/issues/2695

Version 1.26.0 (2018-05-10)
==========================

Language
--------
- [Closures now implement `Copy` and/or `Clone` if all captured variables
  implement either or both traits.][49299]
- [The inclusive range syntax e.g. `for x in 0..=10` is now stable.][47813]
- [The `'_` lifetime is now stable. The underscore lifetime can be used anywhere where a
  lifetime can be elided.][49458]
- [`impl Trait` is now stable allowing you to have abstract types in returns
   or in function parameters.][49255] e.g. `fn foo() -> impl Iterator<Item=u8>` or
  `fn open(path: impl AsRef<Path>)`.
- [Pattern matching will now automatically apply dereferences.][49394]
- [128-bit integers in the form of `u128` and `i128` are now stable.][49101]
- [`main` can now return `Result<(), E: Debug>`][49162] in addition to `()`.
- [A lot of operations are now available in a const context.][46882] E.g. You
  can now index into constant arrays, reference and dereference into constants,
  and use Tuple struct constructors.
- [Fixed entry slice patterns are now stable.][48516] e.g.
  ```rust
  let points = [1, 2, 3, 4];
  match points {
      [1, 2, 3, 4] => println!("All points were sequential."),
      _ => println!("Not all points were sequential."),
  }
  ```


Compiler
--------
- [LLD is now used as the default linker for `wasm32-unknown-unknown`.][48125]
- [Fixed exponential projection complexity on nested types.][48296]
  This can provide up to a ~12% reduction in compile times for certain crates.
- [Added the `--remap-path-prefix` option to rustc.][48359] Allowing you
  to remap path prefixes outputted by the compiler.
- [Added `powerpc-unknown-netbsd` target.][48281]

Libraries
---------
- [Implemented `From<u16> for usize` & `From<{u8, i16}> for isize`.][49305]
- [Added hexadecimal formatting for integers with fmt::Debug][48978]
  e.g. `assert!(format!("{:02x?}", b"Foo\0") == "[46, 6f, 6f, 00]")`
- [Implemented `Default, Hash` for `cmp::Reverse`.][48628]
- [Optimized `str::repeat` being 8x faster in large cases.][48657]
- [`ascii::escape_default` is now available in libcore.][48735]
- [Trailing commas are now supported in std and core macros.][48056]
- [Implemented `Copy, Clone` for `cmp::Reverse`][47379]
- [Implemented `Clone` for `char::{ToLowercase, ToUppercase}`.][48629]

Stabilized APIs
---------------
- [`*const T::add`]
- [`*const T::copy_to_nonoverlapping`]
- [`*const T::copy_to`]
- [`*const T::read_unaligned`]
- [`*const T::read_volatile`]
- [`*const T::read`]
- [`*const T::sub`]
- [`*const T::wrapping_add`]
- [`*const T::wrapping_sub`]
- [`*mut T::add`]
- [`*mut T::copy_to_nonoverlapping`]
- [`*mut T::copy_to`]
- [`*mut T::read_unaligned`]
- [`*mut T::read_volatile`]
- [`*mut T::read`]
- [`*mut T::replace`]
- [`*mut T::sub`]
- [`*mut T::swap`]
- [`*mut T::wrapping_add`]
- [`*mut T::wrapping_sub`]
- [`*mut T::write_bytes`]
- [`*mut T::write_unaligned`]
- [`*mut T::write_volatile`]
- [`*mut T::write`]
- [`Box::leak`]
- [`FromUtf8Error::as_bytes`]
- [`LocalKey::try_with`]
- [`Option::cloned`]
- [`btree_map::Entry::and_modify`]
- [`fs::read_to_string`]
- [`fs::read`]
- [`fs::write`]
- [`hash_map::Entry::and_modify`]
- [`iter::FusedIterator`]
- [`ops::RangeInclusive`]
- [`ops::RangeToInclusive`]
- [`process::id`]
- [`slice::rotate_left`]
- [`slice::rotate_right`]
- [`String::retain`]


Cargo
-----
- [Cargo will now output path to custom commands when `-v` is
  passed with `--list`][cargo/5041]
- [The Cargo binary version is now the same as the Rust version][cargo/5083]

Misc
----
- [The second edition of "The Rust Programming Language" book is now recommended
  over the first.][48404]

Compatibility Notes
-------------------

- [aliasing a `Fn` trait as `dyn` no longer works.][48481] E.g. the following
  syntax is now invalid.
  ```
  use std::ops::Fn as dyn;
  fn g(_: Box<dyn(std::fmt::Debug)>) {}
  ```
- [The result of dereferences are no longer promoted to `'static`.][47408]
  e.g.
  ```rust
  fn main() {
      const PAIR: &(i32, i32) = &(0, 1);
      let _reversed_pair: &'static _ = &(PAIR.1, PAIR.0); // Doesn't work
  }
  ```
- [Deprecate `AsciiExt` trait in favor of inherent methods.][49109]
- [`".e0"` will now no longer parse as `0.0` and will instead cause
  an error.][48235]
- [Removed hoedown from rustdoc.][48274]
- [Bounds on higher-kinded lifetimes a hard error.][48326]

[46882]: https://github.com/rust-lang/rust/pull/46882
[47379]: https://github.com/rust-lang/rust/pull/47379
[47408]: https://github.com/rust-lang/rust/pull/47408
[47813]: https://github.com/rust-lang/rust/pull/47813
[48056]: https://github.com/rust-lang/rust/pull/48056
[48125]: https://github.com/rust-lang/rust/pull/48125
[48166]: https://github.com/rust-lang/rust/pull/48166
[48235]: https://github.com/rust-lang/rust/pull/48235
[48274]: https://github.com/rust-lang/rust/pull/48274
[48281]: https://github.com/rust-lang/rust/pull/48281
[48296]: https://github.com/rust-lang/rust/pull/48296
[48326]: https://github.com/rust-lang/rust/pull/48326
[48359]: https://github.com/rust-lang/rust/pull/48359
[48404]: https://github.com/rust-lang/rust/pull/48404
[48481]: https://github.com/rust-lang/rust/pull/48481
[48516]: https://github.com/rust-lang/rust/pull/48516
[48628]: https://github.com/rust-lang/rust/pull/48628
[48629]: https://github.com/rust-lang/rust/pull/48629
[48657]: https://github.com/rust-lang/rust/pull/48657
[48735]: https://github.com/rust-lang/rust/pull/48735
[48978]: https://github.com/rust-lang/rust/pull/48978
[49101]: https://github.com/rust-lang/rust/pull/49101
[49109]: https://github.com/rust-lang/rust/pull/49109
[49121]: https://github.com/rust-lang/rust/pull/49121
[49162]: https://github.com/rust-lang/rust/pull/49162
[49184]: https://github.com/rust-lang/rust/pull/49184
[49234]: https://github.com/rust-lang/rust/pull/49234
[49255]: https://github.com/rust-lang/rust/pull/49255
[49299]: https://github.com/rust-lang/rust/pull/49299
[49305]: https://github.com/rust-lang/rust/pull/49305
[49394]: https://github.com/rust-lang/rust/pull/49394
[49458]: https://github.com/rust-lang/rust/pull/49458
[`*const T::add`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.add
[`*const T::copy_to_nonoverlapping`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.copy_to_nonoverlapping
[`*const T::copy_to`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.copy_to
[`*const T::read_unaligned`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.read_unaligned
[`*const T::read_volatile`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.read_volatile
[`*const T::read`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.read
[`*const T::sub`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.sub
[`*const T::wrapping_add`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.wrapping_add
[`*const T::wrapping_sub`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.wrapping_sub
[`*mut T::add`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.add-1
[`*mut T::copy_to_nonoverlapping`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.copy_to_nonoverlapping-1
[`*mut T::copy_to`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.copy_to-1
[`*mut T::read_unaligned`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.read_unaligned-1
[`*mut T::read_volatile`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.read_volatile-1
[`*mut T::read`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.read-1
[`*mut T::replace`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.replace
[`*mut T::sub`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.sub-1
[`*mut T::swap`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.swap
[`*mut T::wrapping_add`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.wrapping_add-1
[`*mut T::wrapping_sub`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.wrapping_sub-1
[`*mut T::write_bytes`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.write_bytes
[`*mut T::write_unaligned`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.write_unaligned
[`*mut T::write_volatile`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.write_volatile
[`*mut T::write`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.write
[`Box::leak`]: https://doc.rust-lang.org/std/boxed/struct.Box.html#method.leak
[`FromUtf8Error::as_bytes`]: https://doc.rust-lang.org/std/string/struct.FromUtf8Error.html#method.as_bytes
[`LocalKey::try_with`]: https://doc.rust-lang.org/std/thread/struct.LocalKey.html#method.try_with
[`Option::cloned`]: https://doc.rust-lang.org/std/option/enum.Option.html#method.cloned
[`btree_map::Entry::and_modify`]: https://doc.rust-lang.org/std/collections/btree_map/enum.Entry.html#method.and_modify
[`fs::read_to_string`]: https://doc.rust-lang.org/std/fs/fn.read_to_string.html
[`fs::read`]: https://doc.rust-lang.org/std/fs/fn.read.html
[`fs::write`]: https://doc.rust-lang.org/std/fs/fn.write.html
[`hash_map::Entry::and_modify`]: https://doc.rust-lang.org/std/collections/hash_map/enum.Entry.html#method.and_modify
[`iter::FusedIterator`]: https://doc.rust-lang.org/std/iter/trait.FusedIterator.html
[`ops::RangeInclusive`]: https://doc.rust-lang.org/std/ops/struct.RangeInclusive.html
[`ops::RangeToInclusive`]: https://doc.rust-lang.org/std/ops/struct.RangeToInclusive.html
[`process::id`]: https://doc.rust-lang.org/std/process/fn.id.html
[`slice::rotate_left`]: https://doc.rust-lang.org/std/primitive.slice.html#method.rotate_left
[`slice::rotate_right`]: https://doc.rust-lang.org/std/primitive.slice.html#method.rotate_right
[`String::retain`]: https://doc.rust-lang.org/std/string/struct.String.html#method.retain
[cargo/5041]: https://github.com/rust-lang/cargo/pull/5041
[cargo/5083]: https://github.com/rust-lang/cargo/pull/5083


Version 1.25.0 (2018-03-29)
==========================

Language
--------
- [The `#[repr(align(x))]` attribute is now stable.][47006] [RFC 1358]
- [You can now use nested groups of imports.][47948]
  e.g. `use std::{fs::File, io::Read, path::{Path, PathBuf}};`
- [You can now have `|` at the start of a match arm.][47947] e.g.
```rust
enum Foo { A, B, C }

fn main() {
    let x = Foo::A;
    match x {
        | Foo::A
        | Foo::B => println!("AB"),
        | Foo::C => println!("C"),
    }
}
```

Compiler
--------
- [Upgraded to LLVM 6.][47828]
- [Added `-C lto=val` option.][47521]
- [Added `i586-unknown-linux-musl` target][47282]

Libraries
---------
- [Impl Send for `process::Command` on Unix.][47760]
- [Impl PartialEq and Eq for `ParseCharError`.][47790]
- [`UnsafeCell::into_inner` is now safe.][47204]
- [Implement libstd for CloudABI.][47268]
- [`Float::{from_bits, to_bits}` is now available in libcore.][46931]
- [Implement `AsRef<Path>` for Component][46985]
- [Implemented `Write` for `Cursor<&mut Vec<u8>>`][46830]
- [Moved `Duration` to libcore.][46666]

Stabilized APIs
---------------
- [`Location::column`]
- [`ptr::NonNull`]

The following functions can now be used in a constant expression.
eg. `static MINUTE: Duration = Duration::from_secs(60);`
- [`Duration::new`][47300]
- [`Duration::from_secs`][47300]
- [`Duration::from_millis`][47300]

Cargo
-----
- [`cargo new` no longer removes `rust` or `rs` prefixs/suffixs.][cargo/5013]
- [`cargo new` now defaults to creating a binary crate, instead of a
  library crate.][cargo/5029]

Misc
----
- [Rust by example is now shipped with new releases][46196]

Compatibility Notes
-------------------
- [Deprecated `net::lookup_host`.][47510]
- [`rustdoc` has switched to pulldown as the default markdown renderer.][47398]
- The borrow checker was sometimes incorrectly permitting overlapping borrows
  around indexing operations (see [#47349][47349]). This has been fixed (which also
  enabled some correct code that used to cause errors (e.g. [#33903][33903] and [#46095][46095]).
- [Removed deprecated unstable attribute `#[simd]`.][47251]

[33903]: https://github.com/rust-lang/rust/pull/33903
[47947]: https://github.com/rust-lang/rust/pull/47947
[47948]: https://github.com/rust-lang/rust/pull/47948
[47760]: https://github.com/rust-lang/rust/pull/47760
[47790]: https://github.com/rust-lang/rust/pull/47790
[47828]: https://github.com/rust-lang/rust/pull/47828
[47398]: https://github.com/rust-lang/rust/pull/47398
[47510]: https://github.com/rust-lang/rust/pull/47510
[47521]: https://github.com/rust-lang/rust/pull/47521
[47204]: https://github.com/rust-lang/rust/pull/47204
[47251]: https://github.com/rust-lang/rust/pull/47251
[47268]: https://github.com/rust-lang/rust/pull/47268
[47282]: https://github.com/rust-lang/rust/pull/47282
[47300]: https://github.com/rust-lang/rust/pull/47300
[47349]: https://github.com/rust-lang/rust/pull/47349
[46931]: https://github.com/rust-lang/rust/pull/46931
[46985]: https://github.com/rust-lang/rust/pull/46985
[47006]: https://github.com/rust-lang/rust/pull/47006
[46830]: https://github.com/rust-lang/rust/pull/46830
[46095]: https://github.com/rust-lang/rust/pull/46095
[46666]: https://github.com/rust-lang/rust/pull/46666
[46196]: https://github.com/rust-lang/rust/pull/46196
[cargo/5013]: https://github.com/rust-lang/cargo/pull/5013
[cargo/5029]: https://github.com/rust-lang/cargo/pull/5029
[RFC 1358]: https://github.com/rust-lang/rfcs/pull/1358
[`Location::column`]: https://doc.rust-lang.org/std/panic/struct.Location.html#method.column
[`ptr::NonNull`]: https://doc.rust-lang.org/std/ptr/struct.NonNull.html


Version 1.24.1 (2018-03-01)
==========================

 - [Do not abort when unwinding through FFI][48251]
 - [Emit UTF-16 files for linker arguments on Windows][48318]
 - [Make the error index generator work again][48308]
 - [Cargo will warn on Windows 7 if an update is needed][cargo/5069].

[48251]: https://github.com/rust-lang/rust/issues/48251
[48308]: https://github.com/rust-lang/rust/issues/48308
[48318]: https://github.com/rust-lang/rust/issues/48318
[cargo/5069]: https://github.com/rust-lang/cargo/pull/5069


Version 1.24.0 (2018-02-15)
==========================

Language
--------
- [External `sysv64` ffi is now available.][46528]
  eg. `extern "sysv64" fn foo () {}`

Compiler
--------
- [rustc now uses 16 codegen units by default for release builds.][46910]
  For the fastest builds, utilize `codegen-units=1`.
- [Added `armv4t-unknown-linux-gnueabi` target.][47018]
- [Add `aarch64-unknown-openbsd` support][46760]

Libraries
---------
- [`str::find::<char>` now uses memchr.][46735] This should lead to a 10x
  improvement in performance in the majority of cases.
- [`OsStr`'s `Debug` implementation is now lossless and consistent
  with Windows.][46798]
- [`time::{SystemTime, Instant}` now implement `Hash`.][46828]
- [impl `From<bool>` for `AtomicBool`][46293]
- [impl `From<{CString, &CStr}>` for `{Arc<CStr>, Rc<CStr>}`][45990]
- [impl `From<{OsString, &OsStr}>` for `{Arc<OsStr>, Rc<OsStr>}`][45990]
- [impl `From<{PathBuf, &Path}>` for `{Arc<Path>, Rc<Path>}`][45990]
- [float::from_bits now just uses transmute.][46012] This provides
  some optimisations from LLVM.
- [Copied `AsciiExt` methods onto `char`][46077]
- [Remove `T: Sized` requirement on `ptr::is_null()`][46094]
- [impl `From<RecvError>` for `{TryRecvError, RecvTimeoutError}`][45506]
- [Optimised `f32::{min, max}` to generate more efficient x86 assembly][47080]
- [`[u8]::contains` now uses memchr which provides a 3x speed improvement][46713]

Stabilized APIs
---------------
- [`RefCell::replace`]
- [`RefCell::swap`]
- [`atomic::spin_loop_hint`]

The following functions can now be used in a constant expression.
eg. `let buffer: [u8; size_of::<usize>()];`, `static COUNTER: AtomicUsize = AtomicUsize::new(1);`

- [`AtomicBool::new`][46287]
- [`AtomicUsize::new`][46287]
- [`AtomicIsize::new`][46287]
- [`AtomicPtr::new`][46287]
- [`Cell::new`][46287]
- [`{integer}::min_value`][46287]
- [`{integer}::max_value`][46287]
- [`mem::size_of`][46287]
- [`mem::align_of`][46287]
- [`ptr::null`][46287]
- [`ptr::null_mut`][46287]
- [`RefCell::new`][46287]
- [`UnsafeCell::new`][46287]

Cargo
-----
- [Added a `workspace.default-members` config that
  overrides implied `--all` in virtual workspaces.][cargo/4743]
- [Enable incremental by default on development builds.][cargo/4817] Also added
  configuration keys to `Cargo.toml` and `.cargo/config` to disable on a
  per-project or global basis respectively.

Misc
----

Compatibility Notes
-------------------
- [Floating point types `Debug` impl now always prints a decimal point.][46831]
- [`Ipv6Addr` now rejects superfluous `::`'s in IPv6 addresses][46671] This is
  in accordance with IETF RFC 4291 §2.2.
- [Unwinding will no longer go past FFI boundaries, and will instead abort.][46833]
- [`Formatter::flags` method is now deprecated.][46284] The `sign_plus`,
  `sign_minus`, `alternate`, and `sign_aware_zero_pad` should be used instead.
- [Leading zeros in tuple struct members is now an error][47084]
- [`column!()` macro is one-based instead of zero-based][46977]
- [`fmt::Arguments` can no longer be shared across threads][45198]
- [Access to `#[repr(packed)]` struct fields is now unsafe][44884]
- [Cargo sets a different working directory for the compiler][cargo/4788]

[44884]: https://github.com/rust-lang/rust/pull/44884
[45198]: https://github.com/rust-lang/rust/pull/45198
[45506]: https://github.com/rust-lang/rust/pull/45506
[45904]: https://github.com/rust-lang/rust/pull/45904
[45990]: https://github.com/rust-lang/rust/pull/45990
[46012]: https://github.com/rust-lang/rust/pull/46012
[46077]: https://github.com/rust-lang/rust/pull/46077
[46094]: https://github.com/rust-lang/rust/pull/46094
[46284]: https://github.com/rust-lang/rust/pull/46284
[46287]: https://github.com/rust-lang/rust/pull/46287
[46293]: https://github.com/rust-lang/rust/pull/46293
[46528]: https://github.com/rust-lang/rust/pull/46528
[46671]: https://github.com/rust-lang/rust/pull/46671
[46713]: https://github.com/rust-lang/rust/pull/46713
[46735]: https://github.com/rust-lang/rust/pull/46735
[46749]: https://github.com/rust-lang/rust/pull/46749
[46760]: https://github.com/rust-lang/rust/pull/46760
[46798]: https://github.com/rust-lang/rust/pull/46798
[46828]: https://github.com/rust-lang/rust/pull/46828
[46831]: https://github.com/rust-lang/rust/pull/46831
[46833]: https://github.com/rust-lang/rust/pull/46833
[46910]: https://github.com/rust-lang/rust/pull/46910
[46977]: https://github.com/rust-lang/rust/pull/46977
[47018]: https://github.com/rust-lang/rust/pull/47018
[47080]: https://github.com/rust-lang/rust/pull/47080
[47084]: https://github.com/rust-lang/rust/pull/47084
[cargo/4743]: https://github.com/rust-lang/cargo/pull/4743
[cargo/4788]: https://github.com/rust-lang/cargo/pull/4788
[cargo/4817]: https://github.com/rust-lang/cargo/pull/4817
[`RefCell::replace`]: https://doc.rust-lang.org/std/cell/struct.RefCell.html#method.replace
[`RefCell::swap`]: https://doc.rust-lang.org/std/cell/struct.RefCell.html#method.swap
[`atomic::spin_loop_hint`]: https://doc.rust-lang.org/std/sync/atomic/fn.spin_loop_hint.html


Version 1.23.0 (2018-01-04)
==========================

Language
--------
- [Arbitrary `auto` traits are now permitted in trait objects.][45772]
- [rustc now uses subtyping on the left hand side of binary operations.][45435]
  Which should fix some confusing errors in some operations.

Compiler
--------
- [Enabled `TrapUnreachable` in LLVM which should mitigate the impact of
  undefined behaviour.][45920]
- [rustc now suggests renaming import if names clash.][45660]
- [Display errors/warnings correctly when there are zero-width or
  wide characters.][45711]
- [rustc now avoids unnecessary copies of arguments that are
  simple bindings][45380] This should improve memory usage on average by 5-10%.
- [Updated musl used to build musl rustc to 1.1.17][45393]

Libraries
---------
- [Allow a trailing comma in `assert_eq/ne` macro][45887]
- [Implement Hash for raw pointers to unsized types][45483]
- [impl `From<*mut T>` for `AtomicPtr<T>`][45610]
- [impl `From<usize/isize>` for `AtomicUsize/AtomicIsize`.][45610]
- [Removed the `T: Sync` requirement for `RwLock<T>: Send`][45267]
- [Removed `T: Sized` requirement for `{<*const T>, <*mut T>}::as_ref`
  and `<*mut T>::as_mut`][44932]
- [Optimized `Thread::{park, unpark}` implementation][45524]
- [Improved `SliceExt::binary_search` performance.][45333]
- [impl `FromIterator<()>` for `()`][45379]
- [Copied `AsciiExt` trait methods to primitive types.][44042] Use of `AsciiExt`
  is now deprecated.

Stabilized APIs
---------------

Cargo
-----
- [Cargo now supports uninstallation of multiple packages][cargo/4561]
  eg. `cargo uninstall foo bar` uninstalls `foo` and `bar`.
- [Added unit test checking to `cargo check`][cargo/4592]
- [Cargo now lets you install a specific version
  using `cargo install --version`][cargo/4637]

Misc
----
- [Releases now ship with the Cargo book documentation.][45692]
- [rustdoc now prints rendering warnings on every run.][45324]

Compatibility Notes
-------------------
- [Changes have been made to type equality to make it more correct,
  in rare cases this could break some code.][45853] [Tracking issue for
  further information][45852]
- [`char::escape_debug` now uses Unicode 10 over 9.][45571]
- [Upgraded Android SDK to 27, and NDK to r15c.][45580] This drops support for
  Android 9, the minimum supported version is Android 14.
- [Bumped the minimum LLVM to 3.9][45326]

[44042]: https://github.com/rust-lang/rust/pull/44042
[44932]: https://github.com/rust-lang/rust/pull/44932
[45267]: https://github.com/rust-lang/rust/pull/45267
[45324]: https://github.com/rust-lang/rust/pull/45324
[45326]: https://github.com/rust-lang/rust/pull/45326
[45333]: https://github.com/rust-lang/rust/pull/45333
[45379]: https://github.com/rust-lang/rust/pull/45379
[45380]: https://github.com/rust-lang/rust/pull/45380
[45393]: https://github.com/rust-lang/rust/pull/45393
[45435]: https://github.com/rust-lang/rust/pull/45435
[45483]: https://github.com/rust-lang/rust/pull/45483
[45524]: https://github.com/rust-lang/rust/pull/45524
[45571]: https://github.com/rust-lang/rust/pull/45571
[45580]: https://github.com/rust-lang/rust/pull/45580
[45610]: https://github.com/rust-lang/rust/pull/45610
[45660]: https://github.com/rust-lang/rust/pull/45660
[45692]: https://github.com/rust-lang/rust/pull/45692
[45711]: https://github.com/rust-lang/rust/pull/45711
[45772]: https://github.com/rust-lang/rust/pull/45772
[45852]: https://github.com/rust-lang/rust/issues/45852
[45853]: https://github.com/rust-lang/rust/pull/45853
[45887]: https://github.com/rust-lang/rust/pull/45887
[45920]: https://github.com/rust-lang/rust/pull/45920
[cargo/4561]: https://github.com/rust-lang/cargo/pull/4561
[cargo/4592]: https://github.com/rust-lang/cargo/pull/4592
[cargo/4637]: https://github.com/rust-lang/cargo/pull/4637


Version 1.22.1 (2017-11-22)
==========================

- [Update Cargo to fix an issue with macOS 10.13 "High Sierra"][46183]

[46183]: https://github.com/rust-lang/rust/pull/46183

Version 1.22.0 (2017-11-22)
==========================

Language
--------
- [`non_snake_case` lint now allows extern no-mangle functions][44966]
- [Now accepts underscores in unicode escapes][43716]
- [`T op= &T` now works for numeric types.][44287] eg. `let mut x = 2; x += &8;`
- [types that impl `Drop` are now allowed in `const` and `static` types][44456]

Compiler
--------
- [rustc now defaults to having 16 codegen units at debug on supported platforms.][45064]
- [rustc will no longer inline in codegen units when compiling for debug][45075]
  This should decrease compile times for debug builds.
- [strict memory alignment now enabled on ARMv6][45094]
- [Remove support for the PNaCl target `le32-unknown-nacl`][45041]

Libraries
---------
- [Allow atomic operations up to 32 bits
  on `armv5te_unknown_linux_gnueabi`][44978]
- [`Box<Error>` now impls `From<Cow<str>>`][44466]
- [`std::mem::Discriminant` is now guaranteed to be `Send + Sync`][45095]
- [`fs::copy` now returns the length of the main stream on NTFS.][44895]
- [Properly detect overflow in `Instant += Duration`.][44220]
- [impl `Hasher` for `{&mut Hasher, Box<Hasher>}`][44015]
- [impl `fmt::Debug` for `SplitWhitespace`.][44303]
- [`Option<T>` now impls `Try`][42526] This allows for using `?` with `Option` types.

Stabilized APIs
---------------

Cargo
-----
- [Cargo will now build multi file examples in subdirectories of the `examples`
  folder that have a `main.rs` file.][cargo/4496]
- [Changed `[root]` to `[package]` in `Cargo.lock`][cargo/4571] Packages with
  the old format will continue to work and can be updated with `cargo update`.
- [Now supports vendoring git repositories][cargo/3992]

Misc
----
- [`libbacktrace` is now available on Apple platforms.][44251]
- [Stabilised the `compile_fail` attribute for code fences in doc-comments.][43949]
  This now lets you specify that a given code example will fail to compile.

Compatibility Notes
-------------------
- [The minimum Android version that rustc can build for has been bumped
  to `4.0` from `2.3`][45656]
- [Allowing `T op= &T` for numeric types has broken some type
  inference cases][45480]


[42526]: https://github.com/rust-lang/rust/pull/42526
[43017]: https://github.com/rust-lang/rust/pull/43017
[43716]: https://github.com/rust-lang/rust/pull/43716
[43949]: https://github.com/rust-lang/rust/pull/43949
[44015]: https://github.com/rust-lang/rust/pull/44015
[44220]: https://github.com/rust-lang/rust/pull/44220
[44251]: https://github.com/rust-lang/rust/pull/44251
[44287]: https://github.com/rust-lang/rust/pull/44287
[44303]: https://github.com/rust-lang/rust/pull/44303
[44456]: https://github.com/rust-lang/rust/pull/44456
[44466]: https://github.com/rust-lang/rust/pull/44466
[44895]: https://github.com/rust-lang/rust/pull/44895
[44966]: https://github.com/rust-lang/rust/pull/44966
[44978]: https://github.com/rust-lang/rust/pull/44978
[45041]: https://github.com/rust-lang/rust/pull/45041
[45064]: https://github.com/rust-lang/rust/pull/45064
[45075]: https://github.com/rust-lang/rust/pull/45075
[45094]: https://github.com/rust-lang/rust/pull/45094
[45095]: https://github.com/rust-lang/rust/pull/45095
[45480]: https://github.com/rust-lang/rust/issues/45480
[45656]: https://github.com/rust-lang/rust/pull/45656
[cargo/3992]: https://github.com/rust-lang/cargo/pull/3992
[cargo/4496]: https://github.com/rust-lang/cargo/pull/4496
[cargo/4571]: https://github.com/rust-lang/cargo/pull/4571






Version 1.21.0 (2017-10-12)
==========================

Language
--------
- [You can now use static references for literals.][43838]
  Example:
  ```rust
  fn main() {
      let x: &'static u32 = &0;
  }
  ```
- [Relaxed path syntax. Optional `::` before `<` is now allowed in all contexts.][43540]
  Example:
  ```rust
  my_macro!(Vec<i32>::new); // Always worked
  my_macro!(Vec::<i32>::new); // Now works
  ```

Compiler
--------
- [Upgraded jemalloc to 4.5.0][43911]
- [Enabled unwinding panics on Redox][43917]
- [Now runs LLVM in parallel during translation phase.][43506]
  This should reduce peak memory usage.

Libraries
---------
- [Generate builtin impls for `Clone` for all arrays and tuples that
  are `T: Clone`][43690]
- [`Stdin`, `Stdout`, and `Stderr` now implement `AsRawFd`.][43459]
- [`Rc` and `Arc` now implement `From<&[T]> where T: Clone`, `From<str>`,
  `From<String>`, `From<Box<T>> where T: ?Sized`, and `From<Vec<T>>`.][42565]

Stabilized APIs
---------------

[`std::mem::discriminant`]

Cargo
-----
- [You can now call `cargo install` with multiple package names][cargo/4216]
- [Cargo commands inside a virtual workspace will now implicitly
  pass `--all`][cargo/4335]
- [Added a `[patch]` section to `Cargo.toml` to handle
  prepublication dependencies][cargo/4123] [RFC 1969]
- [`include` & `exclude` fields in `Cargo.toml` now accept gitignore
  like patterns][cargo/4270]
- [Added the `--all-targets` option][cargo/4400]
- [Using required dependencies as a feature is now deprecated and emits
  a warning][cargo/4364]


Misc
----
- [Cargo docs are moving][43916]
  to [doc.rust-lang.org/cargo](https://doc.rust-lang.org/cargo)
- [The rustdoc book is now available][43863]
  at [doc.rust-lang.org/rustdoc](https://doc.rust-lang.org/rustdoc)
- [Added a preview of RLS has been made available through rustup][44204]
  Install with `rustup component add rls-preview`
- [`std::os` documentation for Unix, Linux, and Windows now appears on doc.rust-lang.org][43348]
  Previously only showed `std::os::unix`.

Compatibility Notes
-------------------
- [Changes in method matching against higher-ranked types][43880] This may cause
  breakage in subtyping corner cases. [A more in-depth explanation is available.][info/43880]
- [rustc's JSON error output's byte position start at top of file.][42973]
  Was previously relative to the rustc's internal `CodeMap` struct which
  required the unstable library `libsyntax` to correctly use.
- [`unused_results` lint no longer ignores booleans][43728]

[42565]: https://github.com/rust-lang/rust/pull/42565
[42973]: https://github.com/rust-lang/rust/pull/42973
[43348]: https://github.com/rust-lang/rust/pull/43348
[43459]: https://github.com/rust-lang/rust/pull/43459
[43506]: https://github.com/rust-lang/rust/pull/43506
[43540]: https://github.com/rust-lang/rust/pull/43540
[43690]: https://github.com/rust-lang/rust/pull/43690
[43728]: https://github.com/rust-lang/rust/pull/43728
[43838]: https://github.com/rust-lang/rust/pull/43838
[43863]: https://github.com/rust-lang/rust/pull/43863
[43880]: https://github.com/rust-lang/rust/pull/43880
[43911]: https://github.com/rust-lang/rust/pull/43911
[43916]: https://github.com/rust-lang/rust/pull/43916
[43917]: https://github.com/rust-lang/rust/pull/43917
[44204]: https://github.com/rust-lang/rust/pull/44204
[cargo/4123]: https://github.com/rust-lang/cargo/pull/4123
[cargo/4216]: https://github.com/rust-lang/cargo/pull/4216
[cargo/4270]: https://github.com/rust-lang/cargo/pull/4270
[cargo/4335]: https://github.com/rust-lang/cargo/pull/4335
[cargo/4364]: https://github.com/rust-lang/cargo/pull/4364
[cargo/4400]: https://github.com/rust-lang/cargo/pull/4400
[RFC 1969]: https://github.com/rust-lang/rfcs/pull/1969
[info/43880]: https://github.com/rust-lang/rust/issues/44224#issuecomment-330058902
[`std::mem::discriminant`]: https://doc.rust-lang.org/std/mem/fn.discriminant.html

Version 1.20.0 (2017-08-31)
===========================

Language
--------
- [Associated constants are now stabilised.][42809]
- [A lot of macro bugs are now fixed.][42913]

Compiler
--------

- [Struct fields are now properly coerced to the expected field type.][42807]
- [Enabled wasm LLVM backend][42571] WASM can now be built with the
  `wasm32-experimental-emscripten` target.
- [Changed some of the error messages to be more helpful.][42033]
- [Add support for RELRO(RELocation Read-Only) for platforms that support
  it.][43170]
- [rustc now reports the total number of errors on compilation failure][43015]
  previously this was only the number of errors in the pass that failed.
- [Expansion in rustc has been sped up 29x.][42533]
- [added `msp430-none-elf` target.][43099]
- [rustc will now suggest one-argument enum variant to fix type mismatch when
  applicable][43178]
- [Fixes backtraces on Redox][43228]
- [rustc now identifies different versions of same crate when absolute paths of
  different types match in an error message.][42826]

Libraries
---------


- [Relaxed Debug constraints on `{HashMap,BTreeMap}::{Keys,Values}`.][42854]
- [Impl `PartialEq`, `Eq`, `PartialOrd`, `Ord`, `Debug`, `Hash` for unsized
  tuples.][43011]
- [Impl `fmt::{Display, Debug}` for `Ref`, `RefMut`, `MutexGuard`,
  `RwLockReadGuard`, `RwLockWriteGuard`][42822]
- [Impl `Clone` for `DefaultHasher`.][42799]
- [Impl `Sync` for `SyncSender`.][42397]
- [Impl `FromStr` for `char`][42271]
- [Fixed how `{f32, f64}::{is_sign_negative, is_sign_positive}` handles
  NaN.][42431]
- [allow messages in the `unimplemented!()` macro.][42155]
  ie. `unimplemented!("Waiting for 1.21 to be stable")`
- [`pub(restricted)` is now supported in the `thread_local!` macro.][43185]
- [Upgrade to Unicode 10.0.0][42999]
- [Reimplemented `{f32, f64}::{min, max}` in Rust instead of using CMath.][42430]
- [Skip the main thread's manual stack guard on Linux][43072]
- [Iterator::nth for `ops::{Range, RangeFrom}` is now done in O(1) time][43077]
- [`#[repr(align(N))]` attribute max number is now 2^31 - 1.][43097] This was
  previously 2^15.
- [`{OsStr, Path}::Display` now avoids allocations where possible][42613]

Stabilized APIs
---------------

- [`CStr::into_c_string`]
- [`CString::as_c_str`]
- [`CString::into_boxed_c_str`]
- [`Chain::get_mut`]
- [`Chain::get_ref`]
- [`Chain::into_inner`]
- [`Option::get_or_insert_with`]
- [`Option::get_or_insert`]
- [`OsStr::into_os_string`]
- [`OsString::into_boxed_os_str`]
- [`Take::get_mut`]
- [`Take::get_ref`]
- [`Utf8Error::error_len`]
- [`char::EscapeDebug`]
- [`char::escape_debug`]
- [`compile_error!`]
- [`f32::from_bits`]
- [`f32::to_bits`]
- [`f64::from_bits`]
- [`f64::to_bits`]
- [`mem::ManuallyDrop`]
- [`slice::sort_unstable_by_key`]
- [`slice::sort_unstable_by`]
- [`slice::sort_unstable`]
- [`str::from_boxed_utf8_unchecked`]
- [`str::as_bytes_mut`]
- [`str::as_bytes_mut`]
- [`str::from_utf8_mut`]
- [`str::from_utf8_unchecked_mut`]
- [`str::get_mut`]
- [`str::get_unchecked_mut`]
- [`str::get_unchecked`]
- [`str::get`]
- [`str::into_boxed_bytes`]


Cargo
-----
- [Cargo API token location moved from `~/.cargo/config` to
  `~/.cargo/credentials`.][cargo/3978]
- [Cargo will now build `main.rs` binaries that are in sub-directories of
  `src/bin`.][cargo/4214] ie. Having `src/bin/server/main.rs` and
  `src/bin/client/main.rs` generates `target/debug/server` and `target/debug/client`
- [You can now specify version of a binary when installed through
  `cargo install` using `--vers`.][cargo/4229]
- [Added `--no-fail-fast` flag to cargo to run all benchmarks regardless of
  failure.][cargo/4248]
- [Changed the convention around which file is the crate root.][cargo/4259]
- [The `include`/`exclude` property in `Cargo.toml` now accepts gitignore paths
  instead of glob patterns][cargo/4270]. Glob patterns are now deprecated.

Compatibility Notes
-------------------

- [Functions with `'static` in their return types will now not be as usable as
  if they were using lifetime parameters instead.][42417]
- [The reimplementation of `{f32, f64}::is_sign_{negative, positive}` now
  takes the sign of NaN into account where previously didn't.][42430]

[42033]: https://github.com/rust-lang/rust/pull/42033
[42155]: https://github.com/rust-lang/rust/pull/42155
[42271]: https://github.com/rust-lang/rust/pull/42271
[42397]: https://github.com/rust-lang/rust/pull/42397
[42417]: https://github.com/rust-lang/rust/pull/42417
[42430]: https://github.com/rust-lang/rust/pull/42430
[42431]: https://github.com/rust-lang/rust/pull/42431
[42533]: https://github.com/rust-lang/rust/pull/42533
[42571]: https://github.com/rust-lang/rust/pull/42571
[42613]: https://github.com/rust-lang/rust/pull/42613
[42799]: https://github.com/rust-lang/rust/pull/42799
[42807]: https://github.com/rust-lang/rust/pull/42807
[42809]: https://github.com/rust-lang/rust/pull/42809
[42822]: https://github.com/rust-lang/rust/pull/42822
[42826]: https://github.com/rust-lang/rust/pull/42826
[42854]: https://github.com/rust-lang/rust/pull/42854
[42913]: https://github.com/rust-lang/rust/pull/42913
[42999]: https://github.com/rust-lang/rust/pull/42999
[43011]: https://github.com/rust-lang/rust/pull/43011
[43015]: https://github.com/rust-lang/rust/pull/43015
[43072]: https://github.com/rust-lang/rust/pull/43072
[43077]: https://github.com/rust-lang/rust/pull/43077
[43097]: https://github.com/rust-lang/rust/pull/43097
[43099]: https://github.com/rust-lang/rust/pull/43099
[43170]: https://github.com/rust-lang/rust/pull/43170
[43178]: https://github.com/rust-lang/rust/pull/43178
[43185]: https://github.com/rust-lang/rust/pull/43185
[43228]: https://github.com/rust-lang/rust/pull/43228
[cargo/3978]: https://github.com/rust-lang/cargo/pull/3978
[cargo/4214]: https://github.com/rust-lang/cargo/pull/4214
[cargo/4229]: https://github.com/rust-lang/cargo/pull/4229
[cargo/4248]: https://github.com/rust-lang/cargo/pull/4248
[cargo/4259]: https://github.com/rust-lang/cargo/pull/4259
[cargo/4270]: https://github.com/rust-lang/cargo/pull/4270
[`CStr::into_c_string`]: https://doc.rust-lang.org/std/ffi/struct.CStr.html#method.into_c_string
[`CString::as_c_str`]: https://doc.rust-lang.org/std/ffi/struct.CString.html#method.as_c_str
[`CString::into_boxed_c_str`]: https://doc.rust-lang.org/std/ffi/struct.CString.html#method.into_boxed_c_str
[`Chain::get_mut`]: https://doc.rust-lang.org/std/io/struct.Chain.html#method.get_mut
[`Chain::get_ref`]: https://doc.rust-lang.org/std/io/struct.Chain.html#method.get_ref
[`Chain::into_inner`]: https://doc.rust-lang.org/std/io/struct.Chain.html#method.into_inner
[`Option::get_or_insert_with`]: https://doc.rust-lang.org/std/option/enum.Option.html#method.get_or_insert_with
[`Option::get_or_insert`]: https://doc.rust-lang.org/std/option/enum.Option.html#method.get_or_insert
[`OsStr::into_os_string`]: https://doc.rust-lang.org/std/ffi/struct.OsStr.html#method.into_os_string
[`OsString::into_boxed_os_str`]: https://doc.rust-lang.org/std/ffi/struct.OsString.html#method.into_boxed_os_str
[`Take::get_mut`]: https://doc.rust-lang.org/std/io/struct.Take.html#method.get_mut
[`Take::get_ref`]: https://doc.rust-lang.org/std/io/struct.Take.html#method.get_ref
[`Utf8Error::error_len`]: https://doc.rust-lang.org/std/str/struct.Utf8Error.html#method.error_len
[`char::EscapeDebug`]: https://doc.rust-lang.org/std/char/struct.EscapeDebug.html
[`char::escape_debug`]: https://doc.rust-lang.org/std/primitive.char.html#method.escape_debug
[`compile_error!`]: https://doc.rust-lang.org/std/macro.compile_error.html
[`f32::from_bits`]: https://doc.rust-lang.org/std/primitive.f32.html#method.from_bits
[`f32::to_bits`]: https://doc.rust-lang.org/std/primitive.f32.html#method.to_bits
[`f64::from_bits`]: https://doc.rust-lang.org/std/primitive.f64.html#method.from_bits
[`f64::to_bits`]: https://doc.rust-lang.org/std/primitive.f64.html#method.to_bits
[`mem::ManuallyDrop`]: https://doc.rust-lang.org/std/mem/union.ManuallyDrop.html
[`slice::sort_unstable_by_key`]: https://doc.rust-lang.org/std/primitive.slice.html#method.sort_unstable_by_key
[`slice::sort_unstable_by`]: https://doc.rust-lang.org/std/primitive.slice.html#method.sort_unstable_by
[`slice::sort_unstable`]: https://doc.rust-lang.org/std/primitive.slice.html#method.sort_unstable
[`str::from_boxed_utf8_unchecked`]: https://doc.rust-lang.org/std/str/fn.from_boxed_utf8_unchecked.html
[`str::as_bytes_mut`]: https://doc.rust-lang.org/std/primitive.str.html#method.as_bytes_mut
[`str::from_utf8_mut`]: https://doc.rust-lang.org/std/str/fn.from_utf8_mut.html
[`str::from_utf8_unchecked_mut`]: https://doc.rust-lang.org/std/str/fn.from_utf8_unchecked_mut.html
[`str::get_mut`]: https://doc.rust-lang.org/std/primitive.str.html#method.get_mut
[`str::get_unchecked_mut`]: https://doc.rust-lang.org/std/primitive.str.html#method.get_unchecked_mut
[`str::get_unchecked`]: https://doc.rust-lang.org/std/primitive.str.html#method.get_unchecked
[`str::get`]: https://doc.rust-lang.org/std/primitive.str.html#method.get
[`str::into_boxed_bytes`]: https://doc.rust-lang.org/std/primitive.str.html#method.into_boxed_bytes


Version 1.19.0 (2017-07-20)
===========================

Language
--------

- [Numeric fields can now be used for creating tuple structs.][41145] [RFC 1506]
  For example `struct Point(u32, u32); let x = Point { 0: 7, 1: 0 };`.
- [Macro recursion limit increased to 1024 from 64.][41676]
- [Added lint for detecting unused macros.][41907]
- [`loop` can now return a value with `break`.][42016] [RFC 1624]
  For example: `let x = loop { break 7; };`
- [C compatible `union`s are now available.][42068] [RFC 1444] They can only
  contain `Copy` types and cannot have a `Drop` implementation.
  Example: `union Foo { bar: u8, baz: usize }`
- [Non capturing closures can now be coerced into `fn`s,][42162] [RFC 1558]
  Example: `let foo: fn(u8) -> u8 = |v: u8| { v };`

Compiler
--------

- [Add support for bootstrapping the Rust compiler toolchain on Android.][41370]
- [Change `arm-linux-androideabi` to correspond to the `armeabi`
  official ABI.][41656] If you wish to continue targeting the `armeabi-v7a` ABI
  you should use `--target armv7-linux-androideabi`.
- [Fixed ICE when removing a source file between compilation sessions.][41873]
- [Minor optimisation of string operations.][42037]
- [Compiler error message is now `aborting due to previous error(s)` instead of
  `aborting due to N previous errors`][42150] This was previously inaccurate and
  would only count certain kinds of errors.
- [The compiler now supports Visual Studio 2017][42225]
- [The compiler is now built against LLVM 4.0.1 by default][42948]
- [Added a lot][42264] of [new error codes][42302]
- [Added `target-feature=+crt-static` option][37406] [RFC 1721] Which allows
  libraries with C Run-time Libraries(CRT) to be statically linked.
- [Fixed various ARM codegen bugs][42740]

Libraries
---------

- [`String` now implements `FromIterator<Cow<'a, str>>` and
  `Extend<Cow<'a, str>>`][41449]
- [`Vec` now implements `From<&mut [T]>`][41530]
- [`Box<[u8]>` now implements `From<Box<str>>`][41258]
- [`SplitWhitespace` now implements `Clone`][41659]
- [`[u8]::reverse` is now 5x faster and `[u16]::reverse` is now
  1.5x faster][41764]
- [`eprint!` and `eprintln!` macros added to prelude.][41192] Same as the `print!`
  macros, but for printing to stderr.

Stabilized APIs
---------------

- [`OsString::shrink_to_fit`]
- [`cmp::Reverse`]
- [`Command::envs`]
- [`thread::ThreadId`]

Cargo
-----

- [Build scripts can now add environment variables to the environment
  the crate is being compiled in.
  Example: `println!("cargo:rustc-env=FOO=bar");`][cargo/3929]
- [Subcommands now replace the current process rather than spawning a new
  child process][cargo/3970]
- [Workspace members can now accept glob file patterns][cargo/3979]
- [Added `--all` flag to the `cargo bench` subcommand to run benchmarks of all
  the members in a given workspace.][cargo/3988]
- [Updated `libssh2-sys` to 0.2.6][cargo/4008]
- [Target directory path is now in the cargo metadata][cargo/4022]
- [Cargo no longer checks out a local working directory for the
  crates.io index][cargo/4026] This should provide smaller file size for the
  registry, and improve cloning times, especially on Windows machines.
- [Added an `--exclude` option for excluding certain packages when using the
  `--all` option][cargo/4031]
- [Cargo will now automatically retry when receiving a 5xx error
  from crates.io][cargo/4032]
- [The `--features` option now accepts multiple comma or space
  delimited values.][cargo/4084]
- [Added support for custom target specific runners][cargo/3954]

Misc
----

- [Added `rust-windbg.cmd`][39983] for loading rust `.natvis` files in the
  Windows Debugger.
- [Rust will now release XZ compressed packages][rust-installer/57]
- [rustup will now prefer to download rust packages with
  XZ compression][rustup/1100] over GZip packages.
- [Added the ability to escape `#` in rust documentation][41785] By adding
  additional `#`'s ie. `##` is now `#`

Compatibility Notes
-------------------

- [`MutexGuard<T>` may only be `Sync` if `T` is `Sync`.][41624]
- [`-Z` flags are now no longer allowed to be used on the stable
  compiler.][41751] This has been a warning for a year previous to this.
- [As a result of the `-Z` flag change, the `cargo-check` plugin no
  longer works][42844]. Users should migrate to the built-in `check`
  command, which has been available since 1.16.
- [Ending a float literal with `._` is now a hard error.
  Example: `42._` .][41946]
- [Any use of a private `extern crate` outside of its module is now a
  hard error.][36886] This was previously a warning.
- [`use ::self::foo;` is now a hard error.][36888] `self` paths are always
  relative while the `::` prefix makes a path absolute, but was ignored and the
  path was relative regardless.
- [Floating point constants in match patterns is now a hard error][36890]
  This was previously a warning.
- [Struct or enum constants that don't derive `PartialEq` & `Eq` used
  match patterns is now a hard error][36891] This was previously a warning.
- [Lifetimes named `'_` are no longer allowed.][36892] This was previously
  a warning.
- [From the pound escape, lines consisting of multiple `#`s are
  now visible][41785]
- [It is an error to re-export private enum variants][42460]. This is
  known to break a number of crates that depend on an older version of
  mustache.
- [On Windows, if `VCINSTALLDIR` is set incorrectly, `rustc` will try
  to use it to find the linker, and the build will fail where it did
  not previously][42607]

[36886]: https://github.com/rust-lang/rust/issues/36886
[36888]: https://github.com/rust-lang/rust/issues/36888
[36890]: https://github.com/rust-lang/rust/issues/36890
[36891]: https://github.com/rust-lang/rust/issues/36891
[36892]: https://github.com/rust-lang/rust/issues/36892
[37406]: https://github.com/rust-lang/rust/issues/37406
[39983]: https://github.com/rust-lang/rust/pull/39983
[41145]: https://github.com/rust-lang/rust/pull/41145
[41192]: https://github.com/rust-lang/rust/pull/41192
[41258]: https://github.com/rust-lang/rust/pull/41258
[41370]: https://github.com/rust-lang/rust/pull/41370
[41449]: https://github.com/rust-lang/rust/pull/41449
[41530]: https://github.com/rust-lang/rust/pull/41530
[41624]: https://github.com/rust-lang/rust/pull/41624
[41656]: https://github.com/rust-lang/rust/pull/41656
[41659]: https://github.com/rust-lang/rust/pull/41659
[41676]: https://github.com/rust-lang/rust/pull/41676
[41751]: https://github.com/rust-lang/rust/pull/41751
[41764]: https://github.com/rust-lang/rust/pull/41764
[41785]: https://github.com/rust-lang/rust/pull/41785
[41873]: https://github.com/rust-lang/rust/pull/41873
[41907]: https://github.com/rust-lang/rust/pull/41907
[41946]: https://github.com/rust-lang/rust/pull/41946
[42016]: https://github.com/rust-lang/rust/pull/42016
[42037]: https://github.com/rust-lang/rust/pull/42037
[42068]: https://github.com/rust-lang/rust/pull/42068
[42150]: https://github.com/rust-lang/rust/pull/42150
[42162]: https://github.com/rust-lang/rust/pull/42162
[42225]: https://github.com/rust-lang/rust/pull/42225
[42264]: https://github.com/rust-lang/rust/pull/42264
[42302]: https://github.com/rust-lang/rust/pull/42302
[42460]: https://github.com/rust-lang/rust/issues/42460
[42607]: https://github.com/rust-lang/rust/issues/42607
[42740]: https://github.com/rust-lang/rust/pull/42740
[42844]: https://github.com/rust-lang/rust/issues/42844
[42948]: https://github.com/rust-lang/rust/pull/42948
[RFC 1444]: https://github.com/rust-lang/rfcs/pull/1444
[RFC 1506]: https://github.com/rust-lang/rfcs/pull/1506
[RFC 1558]: https://github.com/rust-lang/rfcs/pull/1558
[RFC 1624]: https://github.com/rust-lang/rfcs/pull/1624
[RFC 1721]: https://github.com/rust-lang/rfcs/pull/1721
[`Command::envs`]: https://doc.rust-lang.org/std/process/struct.Command.html#method.envs
[`OsString::shrink_to_fit`]: https://doc.rust-lang.org/std/ffi/struct.OsString.html#method.shrink_to_fit
[`cmp::Reverse`]: https://doc.rust-lang.org/std/cmp/struct.Reverse.html
[`thread::ThreadId`]: https://doc.rust-lang.org/std/thread/struct.ThreadId.html
[cargo/3929]: https://github.com/rust-lang/cargo/pull/3929
[cargo/3954]: https://github.com/rust-lang/cargo/pull/3954
[cargo/3970]: https://github.com/rust-lang/cargo/pull/3970
[cargo/3979]: https://github.com/rust-lang/cargo/pull/3979
[cargo/3988]: https://github.com/rust-lang/cargo/pull/3988
[cargo/4008]: https://github.com/rust-lang/cargo/pull/4008
[cargo/4022]: https://github.com/rust-lang/cargo/pull/4022
[cargo/4026]: https://github.com/rust-lang/cargo/pull/4026
[cargo/4031]: https://github.com/rust-lang/cargo/pull/4031
[cargo/4032]: https://github.com/rust-lang/cargo/pull/4032
[cargo/4084]: https://github.com/rust-lang/cargo/pull/4084
[rust-installer/57]: https://github.com/rust-lang/rust-installer/pull/57
[rustup/1100]: https://github.com/rust-lang-nursery/rustup.rs/pull/1100


Version 1.18.0 (2017-06-08)
===========================

Language
--------

- [Stabilize pub(restricted)][40556] `pub` can now accept a module path to
  make the item visible to just that module tree. Also accepts the keyword
  `crate` to make something public to the whole crate but not users of the
  library. Example: `pub(crate) mod utils;`. [RFC 1422].
- [Stabilize `#![windows_subsystem]` attribute][40870] conservative exposure of the
  `/SUBSYSTEM` linker flag on Windows platforms. [RFC 1665].
- [Refactor of trait object type parsing][40043] Now `ty` in macros can accept
  types like `Write + Send`, trailing `+` are now supported in trait objects,
  and better error reporting for trait objects starting with `?Sized`.
- [0e+10 is now a valid floating point literal][40589]
- [Now warns if you bind a lifetime parameter to 'static][40734]
- [Tuples, Enum variant fields, and structs with no `repr` attribute or with
  `#[repr(Rust)]` are reordered to minimize padding and produce a smaller
  representation in some cases.][40377]

Compiler
--------

- [rustc can now emit mir with `--emit mir`][39891]
- [Improved LLVM IR for trivial functions][40367]
- [Added explanation for E0090(Wrong number of lifetimes are supplied)][40723]
- [rustc compilation is now 15%-20% faster][41469] Thanks to optimisation
  opportunities found through profiling
- [Improved backtrace formatting when panicking][38165]

Libraries
---------

- [Specialized `Vec::from_iter` being passed `vec::IntoIter`][40731] if the
  iterator hasn't been advanced the original `Vec` is reassembled with no actual
  iteration or reallocation.
- [Simplified HashMap Bucket interface][40561] provides performance
  improvements for iterating and cloning.
- [Specialize Vec::from_elem to use calloc][40409]
- [Fixed Race condition in fs::create_dir_all][39799]
- [No longer caching stdio on Windows][40516]
- [Optimized insertion sort in slice][40807] insertion sort in some cases
  2.50%~ faster and in one case now 12.50% faster.
- [Optimized `AtomicBool::fetch_nand`][41143]

Stabilized APIs
---------------

- [`Child::try_wait`]
- [`HashMap::retain`]
- [`HashSet::retain`]
- [`PeekMut::pop`]
- [`TcpStream::peek`]
- [`UdpSocket::peek`]
- [`UdpSocket::peek_from`]

Cargo
-----

- [Added partial Pijul support][cargo/3842] Pijul is a version control system in Rust.
  You can now create new cargo projects with Pijul using `cargo new --vcs pijul`
- [Now always emits build script warnings for crates that fail to build][cargo/3847]
- [Added Android build support][cargo/3885]
- [Added `--bins` and `--tests` flags][cargo/3901] now you can build all programs
  of a certain type, for example `cargo build --bins` will build all
  binaries.
- [Added support for haiku][cargo/3952]

Misc
----

- [rustdoc can now use pulldown-cmark with the `--enable-commonmark` flag][40338]
- [Added rust-windbg script for better debugging on Windows][39983]
- [Rust now uses the official cross compiler for NetBSD][40612]
- [rustdoc now accepts `#` at the start of files][40828]
- [Fixed jemalloc support for musl][41168]

Compatibility Notes
-------------------

- [Changes to how the `0` flag works in format!][40241] Padding zeroes are now
  always placed after the sign if it exists and before the digits. With the `#`
  flag the zeroes are placed after the prefix and before the digits.
- [Due to the struct field optimisation][40377], using `transmute` on structs
  that have no `repr` attribute or `#[repr(Rust)]` will no longer work. This has
  always been undefined behavior, but is now more likely to break in practice.
- [The refactor of trait object type parsing][40043] fixed a bug where `+` was
  receiving the wrong priority parsing things like `&for<'a> Tr<'a> + Send` as
  `&(for<'a> Tr<'a> + Send)` instead of `(&for<'a> Tr<'a>) + Send`
- [Overlapping inherent `impl`s are now a hard error][40728]
- [`PartialOrd` and `Ord` must agree on the ordering.][41270]
- [`rustc main.rs -o out --emit=asm,llvm-ir`][41085] Now will output
  `out.asm` and `out.ll` instead of only one of the filetypes.
- [ calling a function that returns `Self` will no longer work][41805] when
  the size of `Self` cannot be statically determined.
- [rustc now builds with a "pthreads" flavour of MinGW for Windows GNU][40805]
  this has caused a few regressions namely:

  - Changed the link order of local static/dynamic libraries (respecting the
    order on given rather than having the compiler reorder).
  - Changed how MinGW is linked, native code linked to dynamic libraries
    may require manually linking to the gcc support library (for the native
    code itself)

[38165]: https://github.com/rust-lang/rust/pull/38165
[39799]: https://github.com/rust-lang/rust/pull/39799
[39891]: https://github.com/rust-lang/rust/pull/39891
[39983]: https://github.com/rust-lang/rust/pull/39983
[40043]: https://github.com/rust-lang/rust/pull/40043
[40241]: https://github.com/rust-lang/rust/pull/40241
[40338]: https://github.com/rust-lang/rust/pull/40338
[40367]: https://github.com/rust-lang/rust/pull/40367
[40377]: https://github.com/rust-lang/rust/pull/40377
[40409]: https://github.com/rust-lang/rust/pull/40409
[40516]: https://github.com/rust-lang/rust/pull/40516
[40556]: https://github.com/rust-lang/rust/pull/40556
[40561]: https://github.com/rust-lang/rust/pull/40561
[40589]: https://github.com/rust-lang/rust/pull/40589
[40612]: https://github.com/rust-lang/rust/pull/40612
[40723]: https://github.com/rust-lang/rust/pull/40723
[40728]: https://github.com/rust-lang/rust/pull/40728
[40731]: https://github.com/rust-lang/rust/pull/40731
[40734]: https://github.com/rust-lang/rust/pull/40734
[40805]: https://github.com/rust-lang/rust/pull/40805
[40807]: https://github.com/rust-lang/rust/pull/40807
[40828]: https://github.com/rust-lang/rust/pull/40828
[40870]: https://github.com/rust-lang/rust/pull/40870
[41085]: https://github.com/rust-lang/rust/pull/41085
[41143]: https://github.com/rust-lang/rust/pull/41143
[41168]: https://github.com/rust-lang/rust/pull/41168
[41270]: https://github.com/rust-lang/rust/issues/41270
[41469]: https://github.com/rust-lang/rust/pull/41469
[41805]: https://github.com/rust-lang/rust/issues/41805
[RFC 1422]: https://github.com/rust-lang/rfcs/blob/master/text/1422-pub-restricted.md
[RFC 1665]: https://github.com/rust-lang/rfcs/blob/master/text/1665-windows-subsystem.md
[`Child::try_wait`]: https://doc.rust-lang.org/std/process/struct.Child.html#method.try_wait
[`HashMap::retain`]: https://doc.rust-lang.org/std/collections/struct.HashMap.html#method.retain
[`HashSet::retain`]: https://doc.rust-lang.org/std/collections/struct.HashSet.html#method.retain
[`PeekMut::pop`]: https://doc.rust-lang.org/std/collections/binary_heap/struct.PeekMut.html#method.pop
[`TcpStream::peek`]: https://doc.rust-lang.org/std/net/struct.TcpStream.html#method.peek
[`UdpSocket::peek_from`]: https://doc.rust-lang.org/std/net/struct.UdpSocket.html#method.peek_from
[`UdpSocket::peek`]: https://doc.rust-lang.org/std/net/struct.UdpSocket.html#method.peek
[cargo/3842]: https://github.com/rust-lang/cargo/pull/3842
[cargo/3847]: https://github.com/rust-lang/cargo/pull/3847
[cargo/3885]: https://github.com/rust-lang/cargo/pull/3885
[cargo/3901]: https://github.com/rust-lang/cargo/pull/3901
[cargo/3952]: https://github.com/rust-lang/cargo/pull/3952


Version 1.17.0 (2017-04-27)
===========================

Language
--------

* [The lifetime of statics and consts defaults to `'static`][39265]. [RFC 1623]
* [Fields of structs may be initialized without duplicating the field/variable
  names][39761]. [RFC 1682]
* [`Self` may be included in the `where` clause of `impls`][38864]. [RFC 1647]
* [When coercing to an unsized type lifetimes must be equal][40319]. That is,
  there is no subtyping between `T` and `U` when `T: Unsize<U>`. For example,
  coercing `&mut [&'a X; N]` to `&mut [&'b X]` requires `'a` be equal to
  `'b`. Soundness fix.
* [Values passed to the indexing operator, `[]`, automatically coerce][40166]
* [Static variables may contain references to other statics][40027]

Compiler
--------

* [Exit quickly on only `--emit dep-info`][40336]
* [Make `-C relocation-model` more correctly determine whether the linker
  creates a position-independent executable][40245]
* [Add `-C overflow-checks` to directly control whether integer overflow
  panics][40037]
* [The rustc type checker now checks items on demand instead of in a single
  in-order pass][40008]. This is mostly an internal refactoring in support of
  future work, including incremental type checking, but also resolves [RFC
  1647], allowing `Self` to appear in `impl` `where` clauses.
* [Optimize vtable loads][39995]
* [Turn off vectorization for Emscripten targets][39990]
* [Provide suggestions for unknown macros imported with `use`][39953]
* [Fix ICEs in path resolution][39939]
* [Strip exception handling code on Emscripten when `panic=abort`][39193]
* [Add clearer error message using `&str + &str`][39116]

Stabilized APIs
---------------

* [`Arc::into_raw`]
* [`Arc::from_raw`]
* [`Arc::ptr_eq`]
* [`Rc::into_raw`]
* [`Rc::from_raw`]
* [`Rc::ptr_eq`]
* [`Ordering::then`]
* [`Ordering::then_with`]
* [`BTreeMap::range`]
* [`BTreeMap::range_mut`]
* [`collections::Bound`]
* [`process::abort`]
* [`ptr::read_unaligned`]
* [`ptr::write_unaligned`]
* [`Result::expect_err`]
* [`Cell::swap`]
* [`Cell::replace`]
* [`Cell::into_inner`]
* [`Cell::take`]

Libraries
---------

* [`BTreeMap` and `BTreeSet` can iterate over ranges][27787]
* [`Cell` can store non-`Copy` types][39793]. [RFC 1651]
* [`String` implements `FromIterator<&char>`][40028]
* `Box` [implements][40009] a number of new conversions:
  `From<Box<str>> for String`,
  `From<Box<[T]>> for Vec<T>`,
  `From<Box<CStr>> for CString`,
  `From<Box<OsStr>> for OsString`,
  `From<Box<Path>> for PathBuf`,
  `Into<Box<str>> for String`,
  `Into<Box<[T]>> for Vec<T>`,
  `Into<Box<CStr>> for CString`,
  `Into<Box<OsStr>> for OsString`,
  `Into<Box<Path>> for PathBuf`,
  `Default for Box<str>`,
  `Default for Box<CStr>`,
  `Default for Box<OsStr>`,
  `From<&CStr> for Box<CStr>`,
  `From<&OsStr> for Box<OsStr>`,
  `From<&Path> for Box<Path>`
* [`ffi::FromBytesWithNulError` implements `Error` and `Display`][39960]
* [Specialize `PartialOrd<A> for [A] where A: Ord`][39642]
* [Slightly optimize `slice::sort`][39538]
* [Add `ToString` trait specialization for `Cow<'a, str>` and `String`][39440]
* [`Box<[T]>` implements `From<&[T]> where T: Copy`,
  `Box<str>` implements `From<&str>`][39438]
* [`IpAddr` implements `From` for various arrays. `SocketAddr` implements
  `From<(I, u16)> where I: Into<IpAddr>`][39372]
* [`format!` estimates the needed capacity before writing a string][39356]
* [Support unprivileged symlink creation in Windows][38921]
* [`PathBuf` implements `Default`][38764]
* [Implement `PartialEq<[A]>` for `VecDeque<A>`][38661]
* [`HashMap` resizes adaptively][38368] to guard against DOS attacks
  and poor hash functions.

Cargo
-----

* [Add `cargo check --all`][cargo/3731]
* [Add an option to ignore SSL revocation checking][cargo/3699]
* [Add `cargo run --package`][cargo/3691]
* [Add `required_features`][cargo/3667]
* [Assume `build.rs` is a build script][cargo/3664]
* [Find workspace via `workspace_root` link in containing member][cargo/3562]

Misc
----

* [Documentation is rendered with mdbook instead of the obsolete, in-tree
  `rustbook`][39633]
* [The "Unstable Book" documents nightly-only features][ubook]
* [Improve the style of the sidebar in rustdoc output][40265]
* [Configure build correctly on 64-bit CPU's with the armhf ABI][40261]
* [Fix MSP430 breakage due to `i128`][40257]
* [Preliminary Solaris/SPARCv9 support][39903]
* [`rustc` is linked statically on Windows MSVC targets][39837], allowing it to
  run without installing the MSVC runtime.
* [`rustdoc --test` includes file names in test names][39788]
* This release includes builds of `std` for `sparc64-unknown-linux-gnu`,
  `aarch64-unknown-linux-fuchsia`, and `x86_64-unknown-linux-fuchsia`.
* [Initial support for `aarch64-unknown-freebsd`][39491]
* [Initial support for `i686-unknown-netbsd`][39426]
* [This release no longer includes the old makefile build system][39431]. Rust
  is built with a custom build system, written in Rust, and with Cargo.
* [Add Debug implementations for libcollection structs][39002]
* [`TypeId` implements `PartialOrd` and `Ord`][38981]
* [`--test-threads=0` produces an error][38945]
* [`rustup` installs documentation by default][40526]
* [The Rust source includes NatVis visualizations][39843]. These can be used by
  WinDbg and Visual Studio to improve the debugging experience.

Compatibility Notes
-------------------

* [Rust 1.17 does not correctly detect the MSVC 2017 linker][38584]. As a
  workaround, either use MSVC 2015 or run vcvars.bat.
* [When coercing to an unsized type lifetimes must be equal][40319]. That is,
  disallow subtyping between `T` and `U` when `T: Unsize<U>`, e.g. coercing
  `&mut [&'a X; N]` to `&mut [&'b X]` requires `'a` be equal to `'b`. Soundness
  fix.
* [`format!` and `Display::to_string` panic if an underlying formatting
  implementation returns an error][40117]. Previously the error was silently
  ignored. It is incorrect for `write_fmt` to return an error when writing
  to a string.
* [In-tree crates are verified to be unstable][39851]. Previously, some minor
  crates were marked stable and could be accessed from the stable toolchain.
* [Rust git source no longer includes vendored crates][39728]. Those that need
  to build with vendored crates should build from release tarballs.
* [Fix inert attributes from `proc_macro_derives`][39572]
* [During crate resolution, rustc prefers a crate in the sysroot if two crates
  are otherwise identical][39518]. Unlikely to be encountered outside the Rust
  build system.
* [Fixed bugs around how type inference interacts with dead-code][39485]. The
  existing code generally ignores the type of dead-code unless a type-hint is
  provided; this can cause surprising inference interactions particularly around
  defaulting. The new code uniformly ignores the result type of dead-code.
* [Tuple-struct constructors with private fields are no longer visible][38932]
* [Lifetime parameters that do not appear in the arguments are now considered
  early-bound][38897], resolving a soundness bug (#[32330]). The
  `hr_lifetime_in_assoc_type` future-compatibility lint has been in effect since
  April of 2016.
* [rustdoc: fix doctests with non-feature crate attributes][38161]
* [Make transmuting from fn item types to pointer-sized types a hard
  error][34198]

[27787]: https://github.com/rust-lang/rust/issues/27787
[32330]: https://github.com/rust-lang/rust/issues/32330
[34198]: https://github.com/rust-lang/rust/pull/34198
[38161]: https://github.com/rust-lang/rust/pull/38161
[38368]: https://github.com/rust-lang/rust/pull/38368
[38584]: https://github.com/rust-lang/rust/issues/38584
[38661]: https://github.com/rust-lang/rust/pull/38661
[38764]: https://github.com/rust-lang/rust/pull/38764
[38864]: https://github.com/rust-lang/rust/issues/38864
[38897]: https://github.com/rust-lang/rust/pull/38897
[38921]: https://github.com/rust-lang/rust/pull/38921
[38932]: https://github.com/rust-lang/rust/pull/38932
[38945]: https://github.com/rust-lang/rust/pull/38945
[38981]: https://github.com/rust-lang/rust/pull/38981
[39002]: https://github.com/rust-lang/rust/pull/39002
[39116]: https://github.com/rust-lang/rust/pull/39116
[39193]: https://github.com/rust-lang/rust/pull/39193
[39265]: https://github.com/rust-lang/rust/pull/39265
[39356]: https://github.com/rust-lang/rust/pull/39356
[39372]: https://github.com/rust-lang/rust/pull/39372
[39426]: https://github.com/rust-lang/rust/pull/39426
[39431]: https://github.com/rust-lang/rust/pull/39431
[39438]: https://github.com/rust-lang/rust/pull/39438
[39440]: https://github.com/rust-lang/rust/pull/39440
[39485]: https://github.com/rust-lang/rust/pull/39485
[39491]: https://github.com/rust-lang/rust/pull/39491
[39518]: https://github.com/rust-lang/rust/pull/39518
[39538]: https://github.com/rust-lang/rust/pull/39538
[39572]: https://github.com/rust-lang/rust/pull/39572
[39633]: https://github.com/rust-lang/rust/pull/39633
[39642]: https://github.com/rust-lang/rust/pull/39642
[39728]: https://github.com/rust-lang/rust/pull/39728
[39761]: https://github.com/rust-lang/rust/pull/39761
[39788]: https://github.com/rust-lang/rust/pull/39788
[39793]: https://github.com/rust-lang/rust/pull/39793
[39837]: https://github.com/rust-lang/rust/pull/39837
[39843]: https://github.com/rust-lang/rust/pull/39843
[39851]: https://github.com/rust-lang/rust/pull/39851
[39903]: https://github.com/rust-lang/rust/pull/39903
[39939]: https://github.com/rust-lang/rust/pull/39939
[39953]: https://github.com/rust-lang/rust/pull/39953
[39960]: https://github.com/rust-lang/rust/pull/39960
[39990]: https://github.com/rust-lang/rust/pull/39990
[39995]: https://github.com/rust-lang/rust/pull/39995
[40008]: https://github.com/rust-lang/rust/pull/40008
[40009]: https://github.com/rust-lang/rust/pull/40009
[40027]: https://github.com/rust-lang/rust/pull/40027
[40028]: https://github.com/rust-lang/rust/pull/40028
[40037]: https://github.com/rust-lang/rust/pull/40037
[40117]: https://github.com/rust-lang/rust/pull/40117
[40166]: https://github.com/rust-lang/rust/pull/40166
[40245]: https://github.com/rust-lang/rust/pull/40245
[40257]: https://github.com/rust-lang/rust/pull/40257
[40261]: https://github.com/rust-lang/rust/pull/40261
[40265]: https://github.com/rust-lang/rust/pull/40265
[40319]: https://github.com/rust-lang/rust/pull/40319
[40336]: https://github.com/rust-lang/rust/pull/40336
[40526]: https://github.com/rust-lang/rust/pull/40526
[RFC 1623]: https://github.com/rust-lang/rfcs/blob/master/text/1623-static.md
[RFC 1647]: https://github.com/rust-lang/rfcs/blob/master/text/1647-allow-self-in-where-clauses.md
[RFC 1651]: https://github.com/rust-lang/rfcs/blob/master/text/1651-movecell.md
[RFC 1682]: https://github.com/rust-lang/rfcs/blob/master/text/1682-field-init-shorthand.md
[`Arc::from_raw`]: https://doc.rust-lang.org/std/sync/struct.Arc.html#method.from_raw
[`Arc::into_raw`]: https://doc.rust-lang.org/std/sync/struct.Arc.html#method.into_raw
[`Arc::ptr_eq`]: https://doc.rust-lang.org/std/sync/struct.Arc.html#method.ptr_eq
[`BTreeMap::range_mut`]: https://doc.rust-lang.org/std/collections/btree_map/struct.BTreeMap.html#method.range_mut
[`BTreeMap::range`]: https://doc.rust-lang.org/std/collections/btree_map/struct.BTreeMap.html#method.range
[`Cell::into_inner`]: https://doc.rust-lang.org/std/cell/struct.Cell.html#method.into_inner
[`Cell::replace`]: https://doc.rust-lang.org/std/cell/struct.Cell.html#method.replace
[`Cell::swap`]: https://doc.rust-lang.org/std/cell/struct.Cell.html#method.swap
[`Cell::take`]: https://doc.rust-lang.org/std/cell/struct.Cell.html#method.take
[`Ordering::then_with`]: https://doc.rust-lang.org/std/cmp/enum.Ordering.html#method.then_with
[`Ordering::then`]: https://doc.rust-lang.org/std/cmp/enum.Ordering.html#method.then
[`Rc::from_raw`]: https://doc.rust-lang.org/std/rc/struct.Rc.html#method.from_raw
[`Rc::into_raw`]: https://doc.rust-lang.org/std/rc/struct.Rc.html#method.into_raw
[`Rc::ptr_eq`]: https://doc.rust-lang.org/std/rc/struct.Rc.html#method.ptr_eq
[`Result::expect_err`]: https://doc.rust-lang.org/std/result/enum.Result.html#method.expect_err
[`collections::Bound`]: https://doc.rust-lang.org/std/collections/enum.Bound.html
[`process::abort`]: https://doc.rust-lang.org/std/process/fn.abort.html
[`ptr::read_unaligned`]: https://doc.rust-lang.org/std/ptr/fn.read_unaligned.html
[`ptr::write_unaligned`]: https://doc.rust-lang.org/std/ptr/fn.write_unaligned.html
[cargo/3562]: https://github.com/rust-lang/cargo/pull/3562
[cargo/3664]: https://github.com/rust-lang/cargo/pull/3664
[cargo/3667]: https://github.com/rust-lang/cargo/pull/3667
[cargo/3691]: https://github.com/rust-lang/cargo/pull/3691
[cargo/3699]: https://github.com/rust-lang/cargo/pull/3699
[cargo/3731]: https://github.com/rust-lang/cargo/pull/3731
[mdbook]: https://crates.io/crates/mdbook
[ubook]: https://doc.rust-lang.org/unstable-book/


Version 1.16.0 (2017-03-16)
===========================

Language
--------

* [The compiler's `dead_code` lint now accounts for type aliases][38051].
* [Uninhabitable enums (those without any variants) no longer permit wildcard
  match patterns][38069]
* [Clean up semantics of `self` in an import list][38313]
* [`Self` may appear in `impl` headers][38920]
* [`Self` may appear in struct expressions][39282]

Compiler
--------

* [`rustc` now supports `--emit=metadata`, which causes rustc to emit
  a `.rmeta` file containing only crate metadata][38571]. This can be
  used by tools like the Rust Language Service to perform
  metadata-only builds.
* [Levenshtein based typo suggestions now work in most places, while
  previously they worked only for fields and sometimes for local
  variables][38927]. Together with the overhaul of "no
  resolution"/"unexpected resolution" errors (#[38154]) they result in
  large and systematic improvement in resolution diagnostics.
* [Fix `transmute::<T, U>` where `T` requires a bigger alignment than
  `U`][38670]
* [rustc: use -Xlinker when specifying an rpath with ',' in it][38798]
* [`rustc` no longer attempts to provide "consider using an explicit
  lifetime" suggestions][37057]. They were inaccurate.

Stabilized APIs
---------------

* [`VecDeque::truncate`]
* [`VecDeque::resize`]
* [`String::insert_str`]
* [`Duration::checked_add`]
* [`Duration::checked_sub`]
* [`Duration::checked_div`]
* [`Duration::checked_mul`]
* [`str::replacen`]
* [`str::repeat`]
* [`SocketAddr::is_ipv4`]
* [`SocketAddr::is_ipv6`]
* [`IpAddr::is_ipv4`]
* [`IpAddr::is_ipv6`]
* [`Vec::dedup_by`]
* [`Vec::dedup_by_key`]
* [`Result::unwrap_or_default`]
* [`<*const T>::wrapping_offset`]
* [`<*mut T>::wrapping_offset`]
* `CommandExt::creation_flags`
* [`File::set_permissions`]
* [`String::split_off`]

Libraries
---------

* [`[T]::binary_search` and `[T]::binary_search_by_key` now take
  their argument by `Borrow` parameter][37761]
* [All public types in std implement `Debug`][38006]
* [`IpAddr` implements `From<Ipv4Addr>` and `From<Ipv6Addr>`][38327]
* [`Ipv6Addr` implements `From<[u16; 8]>`][38131]
* [Ctrl-Z returns from `Stdin.read()` when reading from the console on
  Windows][38274]
* [std: Fix partial writes in `LineWriter`][38062]
* [std: Clamp max read/write sizes on Unix][38062]
* [Use more specific panic message for `&str` slicing errors][38066]
* [`TcpListener::set_only_v6` is deprecated][38304]. This
  functionality cannot be achieved in std currently.
* [`writeln!`, like `println!`, now accepts a form with no string
  or formatting arguments, to just print a newline][38469]
* [Implement `iter::Sum` and `iter::Product` for `Result`][38580]
* [Reduce the size of static data in `std_unicode::tables`][38781]
* [`char::EscapeDebug`, `EscapeDefault`, `EscapeUnicode`,
  `CaseMappingIter`, `ToLowercase`, `ToUppercase`, implement
  `Display`][38909]
* [`Duration` implements `Sum`][38712]
* [`String` implements `ToSocketAddrs`][39048]

Cargo
-----

* [The `cargo check` command does a type check of a project without
  building it][cargo/3296]
* [crates.io will display CI badges from Travis and AppVeyor, if
  specified in Cargo.toml][cargo/3546]
* [crates.io will display categories listed in Cargo.toml][cargo/3301]
* [Compilation profiles accept integer values for `debug`, in addition
  to `true` and `false`. These are passed to `rustc` as the value to
  `-C debuginfo`][cargo/3534]
* [Implement `cargo --version --verbose`][cargo/3604]
* [All builds now output 'dep-info' build dependencies compatible with
  make and ninja][cargo/3557]
* [Build all workspace members with `build --all`][cargo/3511]
* [Document all workspace members with `doc --all`][cargo/3515]
* [Path deps outside workspace are not members][cargo/3443]

Misc
----

* [`rustdoc` has a `--sysroot` argument that, like `rustc`, specifies
  the path to the Rust implementation][38589]
* [The `armv7-linux-androideabi` target no longer enables NEON
  extensions, per Google's ABI guide][38413]
* [The stock standard library can be compiled for Redox OS][38401]
* [Rust has initial SPARC support][38726]. Tier 3. No builds
  available.
* [Rust has experimental support for Nvidia PTX][38559]. Tier 3. No
  builds available.
* [Fix backtraces on i686-pc-windows-gnu by disabling FPO][39379]

Compatibility Notes
-------------------

* [Uninhabitable enums (those without any variants) no longer permit wildcard
  match patterns][38069]
* In this release, references to uninhabited types can not be
  pattern-matched. This was accidentally allowed in 1.15.
* [The compiler's `dead_code` lint now accounts for type aliases][38051].
* [Ctrl-Z returns from `Stdin.read()` when reading from the console on
  Windows][38274]
* [Clean up semantics of `self` in an import list][38313]
* Reimplemented lifetime elision. This change was almost entirely compatible
  with existing code, but it did close a number of small bugs and loopholes,
  as well as being more accepting in some other [cases][41105].

[37057]: https://github.com/rust-lang/rust/pull/37057
[37761]: https://github.com/rust-lang/rust/pull/37761
[38006]: https://github.com/rust-lang/rust/pull/38006
[38051]: https://github.com/rust-lang/rust/pull/38051
[38062]: https://github.com/rust-lang/rust/pull/38062
[38062]: https://github.com/rust-lang/rust/pull/38622
[38066]: https://github.com/rust-lang/rust/pull/38066
[38069]: https://github.com/rust-lang/rust/pull/38069
[38131]: https://github.com/rust-lang/rust/pull/38131
[38154]: https://github.com/rust-lang/rust/pull/38154
[38274]: https://github.com/rust-lang/rust/pull/38274
[38304]: https://github.com/rust-lang/rust/pull/38304
[38313]: https://github.com/rust-lang/rust/pull/38313
[38314]: https://github.com/rust-lang/rust/pull/38314
[38327]: https://github.com/rust-lang/rust/pull/38327
[38401]: https://github.com/rust-lang/rust/pull/38401
[38413]: https://github.com/rust-lang/rust/pull/38413
[38469]: https://github.com/rust-lang/rust/pull/38469
[38559]: https://github.com/rust-lang/rust/pull/38559
[38571]: https://github.com/rust-lang/rust/pull/38571
[38580]: https://github.com/rust-lang/rust/pull/38580
[38589]: https://github.com/rust-lang/rust/pull/38589
[38670]: https://github.com/rust-lang/rust/pull/38670
[38712]: https://github.com/rust-lang/rust/pull/38712
[38726]: https://github.com/rust-lang/rust/pull/38726
[38781]: https://github.com/rust-lang/rust/pull/38781
[38798]: https://github.com/rust-lang/rust/pull/38798
[38909]: https://github.com/rust-lang/rust/pull/38909
[38920]: https://github.com/rust-lang/rust/pull/38920
[38927]: https://github.com/rust-lang/rust/pull/38927
[39048]: https://github.com/rust-lang/rust/pull/39048
[39282]: https://github.com/rust-lang/rust/pull/39282
[39379]: https://github.com/rust-lang/rust/pull/39379
[41105]: https://github.com/rust-lang/rust/issues/41105
[`<*const T>::wrapping_offset`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.wrapping_offset
[`<*mut T>::wrapping_offset`]: https://doc.rust-lang.org/std/primitive.pointer.html#method.wrapping_offset
[`Duration::checked_add`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.checked_add
[`Duration::checked_div`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.checked_div
[`Duration::checked_mul`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.checked_mul
[`Duration::checked_sub`]: https://doc.rust-lang.org/std/time/struct.Duration.html#method.checked_sub
[`File::set_permissions`]: https://doc.rust-lang.org/std/fs/struct.File.html#method.set_permissions
[`IpAddr::is_ipv4`]: https://doc.rust-lang.org/std/net/enum.IpAddr.html#method.is_ipv4
[`IpAddr::is_ipv6`]: https://doc.rust-lang.org/std/net/enum.IpAddr.html#method.is_ipv6
[`Result::unwrap_or_default`]: https://doc.rust-lang.org/std/result/enum.Result.html#method.unwrap_or_default
[`SocketAddr::is_ipv4`]: https://doc.rust-lang.org/std/net/enum.SocketAddr.html#method.is_ipv4
[`SocketAddr::is_ipv6`]: https://doc.rust-lang.org/std/net/enum.SocketAddr.html#method.is_ipv6
[`String::insert_str`]: https://doc.rust-lang.org/std/string/struct.String.html#method.insert_str
[`String::split_off`]: https://doc.rust-lang.org/std/string/struct.String.html#method.split_off
[`Vec::dedup_by_key`]: https://doc.rust-lang.org/std/vec/struct.Vec.html#method.dedup_by_key
[`Vec::dedup_by`]: https://doc.rust-lang.org/std/vec/struct.Vec.html#method.dedup_by
[`VecDeque::resize`]:  https://doc.rust-lang.org/std/collections/vec_deque/struct.VecDeque.html#method.resize
[`VecDeque::truncate`]: https://doc.rust-lang.org/std/collections/vec_deque/struct.VecDeque.html#method.truncate
[`str::repeat`]: https://doc.rust-lang.org/std/primitive.str.html#method.repeat
[`str::replacen`]: https://doc.rust-lang.org/std/primitive.str.html#method.replacen
[cargo/3296]: https://github.com/rust-lang/cargo/pull/3296
[cargo/3301]: https://github.com/rust-lang/cargo/pull/3301
[cargo/3443]: https://github.com/rust-lang/cargo/pull/3443
[cargo/3511]: https://github.com/rust-lang/cargo/pull/3511
[cargo/3515]: https://github.com/rust-lang/cargo/pull/3515
[cargo/3534]: https://github.com/rust-lang/cargo/pull/3534
[cargo/3546]: https://github.com/rust-lang/cargo/pull/3546
[cargo/3557]: https://github.com/rust-lang/cargo/pull/3557
[cargo/3604]: https://github.com/rust-lang/cargo/pull/3604
[RFC 1623]: https://github.com/rust-lang/rfcs/blob/master/text/1623-static.md


Version 1.15.1 (2017-02-09)
===========================

* [Fix IntoIter::as_mut_slice's signature][39466]
* [Compile compiler builtins with `-fPIC` on 32-bit platforms][39523]

[39466]: https://github.com/rust-lang/rust/pull/39466
[39523]: https://github.com/rust-lang/rust/pull/39523


Version 1.15.0 (2017-02-02)
===========================

Language
--------

* Basic procedural macros allowing custom `#[derive]`, aka "macros 1.1", are
  stable. This allows popular code-generating crates like Serde and Diesel to
  work ergonomically. [RFC 1681].
* [Tuple structs may be empty. Unary and empty tuple structs may be instantiated
  with curly braces][36868]. Part of [RFC 1506].
* [A number of minor changes to name resolution have been activated][37127].
  They add up to more consistent semantics, allowing for future evolution of
  Rust macros. Specified in [RFC 1560], see its section on ["changes"] for
  details of what is different. The breaking changes here have been transitioned
  through the [`legacy_imports`] lint since 1.14, with no known regressions.
* [In `macro_rules`, `path` fragments can now be parsed as type parameter
  bounds][38279]
* [`?Sized` can be used in `where` clauses][37791]
* [There is now a limit on the size of monomorphized types and it can be
  modified with the `#![type_size_limit]` crate attribute, similarly to
  the `#![recursion_limit]` attribute][37789]

Compiler
--------

* [On Windows, the compiler will apply dllimport attributes when linking to
  extern functions][37973]. Additional attributes and flags can control which
  library kind is linked and its name. [RFC 1717].
* [Rust-ABI symbols are no longer exported from cdylibs][38117]
* [The `--test` flag works with procedural macro crates][38107]
* [Fix `extern "aapcs" fn` ABI][37814]
* [The `-C no-stack-check` flag is deprecated][37636]. It does nothing.
* [The `format!` expander recognizes incorrect `printf` and shell-style
  formatting directives and suggests the correct format][37613].
* [Only report one error for all unused imports in an import list][37456]

Compiler Performance
--------------------

* [Avoid unnecessary `mk_ty` calls in `Ty::super_fold_with`][37705]
* [Avoid more unnecessary `mk_ty` calls in `Ty::super_fold_with`][37979]
* [Don't clone in `UnificationTable::probe`][37848]
* [Remove `scope_auxiliary` to cut RSS by 10%][37764]
* [Use small vectors in type walker][37760]
* [Macro expansion performance was improved][37701]
* [Change `HirVec<P<T>>` to `HirVec<T>` in `hir::Expr`][37642]
* [Replace FNV with a faster hash function][37229]

Stabilized APIs
---------------

* [`std::iter::Iterator::min_by`]
* [`std::iter::Iterator::max_by`]
* [`std::os::*::fs::FileExt`]
* [`std::sync::atomic::Atomic*::get_mut`]
* [`std::sync::atomic::Atomic*::into_inner`]
* [`std::vec::IntoIter::as_slice`]
* [`std::vec::IntoIter::as_mut_slice`]
* [`std::sync::mpsc::Receiver::try_iter`]
* [`std::os::unix::process::CommandExt::before_exec`]
* [`std::rc::Rc::strong_count`]
* [`std::rc::Rc::weak_count`]
* [`std::sync::Arc::strong_count`]
* [`std::sync::Arc::weak_count`]
* [`std::char::encode_utf8`]
* [`std::char::encode_utf16`]
* [`std::cell::Ref::clone`]
* [`std::io::Take::into_inner`]

Libraries
---------

* [The standard sorting algorithm has been rewritten for dramatic performance
  improvements][38192]. It is a hybrid merge sort, drawing influences from
  Timsort. Previously it was a naive merge sort.
* [`Iterator::nth` no longer has a `Sized` bound][38134]
* [`Extend<&T>` is specialized for `Vec` where `T: Copy`][38182] to improve
  performance.
* [`chars().count()` is much faster][37888] and so are [`chars().last()`
  and `char_indices().last()`][37882]
* [Fix ARM Objective-C ABI in `std::env::args`][38146]
* [Chinese characters display correctly in `fmt::Debug`][37855]
* [Derive `Default` for `Duration`][37699]
* [Support creation of anonymous pipes on WinXP/2k][37677]
* [`mpsc::RecvTimeoutError` implements `Error`][37527]
* [Don't pass overlapped handles to processes][38835]

Cargo
-----

* [In this release, Cargo build scripts no longer have access to the `OUT_DIR`
  environment variable at build time via `env!("OUT_DIR")`][cargo/3368]. They
  should instead check the variable at runtime with `std::env`. That the value
  was set at build time was a bug, and incorrect when cross-compiling. This
  change is known to cause breakage.
* [Add `--all` flag to `cargo test`][cargo/3221]
* [Compile statically against the MSVC CRT][cargo/3363]
* [Mix feature flags into fingerprint/metadata shorthash][cargo/3102]
* [Link OpenSSL statically on OSX][cargo/3311]
* [Apply new fingerprinting to build dir outputs][cargo/3310]
* [Test for bad path overrides with summaries][cargo/3336]
* [Require `cargo install --vers` to take a semver version][cargo/3338]
* [Fix retrying crate downloads for network errors][cargo/3348]
* [Implement string lookup for `build.rustflags` config key][cargo/3356]
* [Emit more info on --message-format=json][cargo/3319]
* [Assume `build.rs` in the same directory as `Cargo.toml` is a build script][cargo/3361]
* [Don't ignore errors in workspace manifest][cargo/3409]
* [Fix `--message-format JSON` when rustc emits non-JSON warnings][cargo/3410]

Tooling
-------

* [Test runners (binaries built with `--test`) now support a `--list` argument
  that lists the tests it contains][38185]
* [Test runners now support a `--exact` argument that makes the test filter
  match exactly, instead of matching only a substring of the test name][38181]
* [rustdoc supports a `--playground-url` flag][37763]
* [rustdoc provides more details about `#[should_panic]` errors][37749]

Misc
----

* [The Rust build system is now written in Rust][37817]. The Makefiles may
  continue to be used in this release by passing `--disable-rustbuild` to the
  configure script, but they will be deleted soon. Note that the new build
  system uses a different on-disk layout that will likely affect any scripts
  building Rust.
* [Rust supports i686-unknown-openbsd][38086]. Tier 3 support. No testing or
  releases.
* [Rust supports the MSP430][37627]. Tier 3 support. No testing or releases.
* [Rust supports the ARMv5TE architecture][37615]. Tier 3 support. No testing or
  releases.

Compatibility Notes
-------------------

* [A number of minor changes to name resolution have been activated][37127].
  They add up to more consistent semantics, allowing for future evolution of
  Rust macros. Specified in [RFC 1560], see its section on ["changes"] for
  details of what is different. The breaking changes here have been transitioned
  through the [`legacy_imports`] lint since 1.14, with no known regressions.
* [In this release, Cargo build scripts no longer have access to the `OUT_DIR`
  environment variable at build time via `env!("OUT_DIR")`][cargo/3368]. They
  should instead check the variable at runtime with `std::env`. That the value
  was set at build time was a bug, and incorrect when cross-compiling. This
  change is known to cause breakage.
* [Higher-ranked lifetimes are no longer allowed to appear _only_ in associated
  types][33685]. The [`hr_lifetime_in_assoc_type` lint] has been a warning since
  1.10 and is now an error by default. It will become a hard error in the near
  future.
* [The semantics relating modules to file system directories are changing in
  minor ways][37602]. This is captured in the new `legacy_directory_ownership`
  lint, which is a warning in this release, and will become a hard error in the
  future.
* [Rust-ABI symbols are no longer exported from cdylibs][38117]
* [Once `Peekable` peeks a `None` it will return that `None` without re-querying
  the underlying iterator][37834]

["changes"]: https://github.com/rust-lang/rfcs/blob/master/text/1560-name-resolution.md#changes-to-name-resolution-rules
[33685]: https://github.com/rust-lang/rust/issues/33685
[36868]: https://github.com/rust-lang/rust/pull/36868
[37127]: https://github.com/rust-lang/rust/pull/37127
[37229]: https://github.com/rust-lang/rust/pull/37229
[37456]: https://github.com/rust-lang/rust/pull/37456
[37527]: https://github.com/rust-lang/rust/pull/37527
[37602]: https://github.com/rust-lang/rust/pull/37602
[37613]: https://github.com/rust-lang/rust/pull/37613
[37615]: https://github.com/rust-lang/rust/pull/37615
[37636]: https://github.com/rust-lang/rust/pull/37636
[37627]: https://github.com/rust-lang/rust/pull/37627
[37642]: https://github.com/rust-lang/rust/pull/37642
[37677]: https://github.com/rust-lang/rust/pull/37677
[37699]: https://github.com/rust-lang/rust/pull/37699
[37701]: https://github.com/rust-lang/rust/pull/37701
[37705]: https://github.com/rust-lang/rust/pull/37705
[37749]: https://github.com/rust-lang/rust/pull/37749
[37760]: https://github.com/rust-lang/rust/pull/37760
[37763]: https://github.com/rust-lang/rust/pull/37763
[37764]: https://github.com/rust-lang/rust/pull/37764
[37789]: https://github.com/rust-lang/rust/pull/37789
[37791]: https://github.com/rust-lang/rust/pull/37791
[37814]: https://github.com/rust-lang/rust/pull/37814
[37817]: https://github.com/rust-lang/rust/pull/37817
[37834]: https://github.com/rust-lang/rust/pull/37834
[37848]: https://github.com/rust-lang/rust/pull/37848
[37855]: https://github.com/rust-lang/rust/pull/37855
[37882]: https://github.com/rust-lang/rust/pull/37882
[37888]: https://github.com/rust-lang/rust/pull/37888
[37973]: https://github.com/rust-lang/rust/pull/37973
[37979]: https://github.com/rust-lang/rust/pull/37979
[38086]: https://github.com/rust-lang/rust/pull/38086
[38107]: https://github.com/rust-lang/rust/pull/38107
[38117]: https://github.com/rust-lang/rust/pull/38117
[38134]: https://github.com/rust-lang/rust/pull/38134
[38146]: https://github.com/rust-lang/rust/pull/38146
[38181]: https://github.com/rust-lang/rust/pull/38181
[38182]: https://github.com/rust-lang/rust/pull/38182
[38185]: https://github.com/rust-lang/rust/pull/38185
[38192]: https://github.com/rust-lang/rust/pull/38192
[38279]: https://github.com/rust-lang/rust/pull/38279
[38835]: https://github.com/rust-lang/rust/pull/38835
[RFC 1492]: https://github.com/rust-lang/rfcs/blob/master/text/1492-dotdot-in-patterns.md
[RFC 1506]: https://github.com/rust-lang/rfcs/blob/master/text/1506-adt-kinds.md
[RFC 1560]: https://github.com/rust-lang/rfcs/blob/master/text/1560-name-resolution.md
[RFC 1681]: https://github.com/rust-lang/rfcs/blob/master/text/1681-macros-1.1.md
[RFC 1717]: https://github.com/rust-lang/rfcs/blob/master/text/1717-dllimport.md
[`hr_lifetime_in_assoc_type` lint]: https://github.com/rust-lang/rust/issues/33685
[`legacy_imports`]: https://github.com/rust-lang/rust/pull/38271
[cargo/3102]: https://github.com/rust-lang/cargo/pull/3102
[cargo/3221]: https://github.com/rust-lang/cargo/pull/3221
[cargo/3310]: https://github.com/rust-lang/cargo/pull/3310
[cargo/3311]: https://github.com/rust-lang/cargo/pull/3311
[cargo/3319]: https://github.com/rust-lang/cargo/pull/3319
[cargo/3336]: https://github.com/rust-lang/cargo/pull/3336
[cargo/3338]: https://github.com/rust-lang/cargo/pull/3338
[cargo/3348]: https://github.com/rust-lang/cargo/pull/3348
[cargo/3356]: https://github.com/rust-lang/cargo/pull/3356
[cargo/3361]: https://github.com/rust-lang/cargo/pull/3361
[cargo/3363]: https://github.com/rust-lang/cargo/pull/3363
[cargo/3368]: https://github.com/rust-lang/cargo/issues/3368
[cargo/3409]: https://github.com/rust-lang/cargo/pull/3409
[cargo/3410]: https://github.com/rust-lang/cargo/pull/3410
[`std::iter::Iterator::min_by`]: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.min_by
[`std::iter::Iterator::max_by`]: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.max_by
[`std::os::*::fs::FileExt`]: https://doc.rust-lang.org/std/os/unix/fs/trait.FileExt.html
[`std::sync::atomic::Atomic*::get_mut`]: https://doc.rust-lang.org/std/sync/atomic/struct.AtomicU8.html#method.get_mut
[`std::sync::atomic::Atomic*::into_inner`]: https://doc.rust-lang.org/std/sync/atomic/struct.AtomicU8.html#method.into_inner
[`std::vec::IntoIter::as_slice`]: https://doc.rust-lang.org/std/vec/struct.IntoIter.html#method.as_slice
[`std::vec::IntoIter::as_mut_slice`]: https://doc.rust-lang.org/std/vec/struct.IntoIter.html#method.as_mut_slice
[`std::sync::mpsc::Receiver::try_iter`]: https://doc.rust-lang.org/std/sync/mpsc/struct.Receiver.html#method.try_iter
[`std::os::unix::process::CommandExt::before_exec`]: https://doc.rust-lang.org/std/os/unix/process/trait.CommandExt.html#tymethod.before_exec
[`std::rc::Rc::strong_count`]: https://doc.rust-lang.org/std/rc/struct.Rc.html#method.strong_count
[`std::rc::Rc::weak_count`]: https://doc.rust-lang.org/std/rc/struct.Rc.html#method.weak_count
[`std::sync::Arc::strong_count`]: https://doc.rust-lang.org/std/sync/struct.Arc.html#method.strong_count
[`std::sync::Arc::weak_count`]: https://doc.rust-lang.org/std/sync/struct.Arc.html#method.weak_count
[`std::char::encode_utf8`]: https://doc.rust-lang.org/std/primitive.char.html#method.encode_utf8
[`std::char::encode_utf16`]: https://doc.rust-lang.org/std/primitive.char.html#method.encode_utf16
[`std::cell::Ref::clone`]: https://doc.rust-lang.org/std/cell/struct.Ref.html#method.clone
[`std::io::Take::into_inner`]: https://doc.rust-lang.org/std/io/struct.Take.html#method.into_inner


Version 1.14.0 (2016-12-22)
===========================

Language
--------

* [`..` matches multiple tuple fields in enum variants, structs
  and tuples][36843]. [RFC 1492].
* [Safe `fn` items can be coerced to `unsafe fn` pointers][37389]
* [`use *` and `use ::*` both glob-import from the crate root][37367]
* [It's now possible to call a `Vec<Box<Fn()>>` without explicit
  dereferencing][36822]

Compiler
--------

* [Mark enums with non-zero discriminant as non-zero][37224]
* [Lower-case `static mut` names are linted like other
  statics and consts][37162]
* [Fix ICE on some macros in const integer positions
   (e.g. `[u8; m!()]`)][36819]
* [Improve error message and snippet for "did you mean `x`"][36798]
* [Add a panic-strategy field to the target specification][36794]
* [Include LLVM version in `--version --verbose`][37200]

Compile-time Optimizations
--------------------------

* [Improve macro expansion performance][37569]
* [Shrink `Expr_::ExprInlineAsm`][37445]
* [Replace all uses of SHA-256 with BLAKE2b][37439]
* [Reduce the number of bytes hashed by `IchHasher`][37427]
* [Avoid more allocations when compiling html5ever][37373]
* [Use `SmallVector` in `CombineFields::instantiate`][37322]
* [Avoid some allocations in the macro parser][37318]
* [Use a faster deflate setting][37298]
* [Add `ArrayVec` and `AccumulateVec` to reduce heap allocations
  during interning of slices][37270]
* [Optimize `write_metadata`][37267]
* [Don't process obligation forest cycles when stalled][37231]
* [Avoid many `CrateConfig` clones][37161]
* [Optimize `Substs::super_fold_with`][37108]
* [Optimize `ObligationForest`'s `NodeState` handling][36993]
* [Speed up `plug_leaks`][36917]

Libraries
---------

* [`println!()`, with no arguments, prints newline][36825].
  Previously, an empty string was required to achieve the same.
* [`Wrapping` impls standard binary and unary operators, as well as
   the `Sum` and `Product` iterators][37356]
* [Implement `From<Cow<str>> for String` and `From<Cow<[T]>> for
  Vec<T>`][37326]
* [Improve `fold` performance for `chain`, `cloned`, `map`, and
  `VecDeque` iterators][37315]
* [Improve `SipHasher` performance on small values][37312]
* [Add Iterator trait TrustedLen to enable better FromIterator /
  Extend][37306]
* [Expand `.zip()` specialization to `.map()` and `.cloned()`][37230]
* [`ReadDir` implements `Debug`][37221]
* [Implement `RefUnwindSafe` for atomic types][37178]
* [Specialize `Vec::extend` to `Vec::extend_from_slice`][37094]
* [Avoid allocations in `Decoder::read_str`][37064]
* [`io::Error` implements `From<io::ErrorKind>`][37037]
* [Impl `Debug` for raw pointers to unsized data][36880]
* [Don't reuse `HashMap` random seeds][37470]
* [The internal memory layout of `HashMap` is more cache-friendly, for
  significant improvements in some operations][36692]
* [`HashMap` uses less memory on 32-bit architectures][36595]
* [Impl `Add<{str, Cow<str>}>` for `Cow<str>`][36430]

Cargo
-----

* [Expose rustc cfg values to build scripts][cargo/3243]
* [Allow cargo to work with read-only `CARGO_HOME`][cargo/3259]
* [Fix passing --features when testing multiple packages][cargo/3280]
* [Use a single profile set per workspace][cargo/3249]
* [Load `replace` sections from lock files][cargo/3220]
* [Ignore `panic` configuration for test/bench profiles][cargo/3175]

Tooling
-------

* [rustup is the recommended Rust installation method][1.14rustup]
* This release includes host (rustc) builds for Linux on MIPS, PowerPC, and
  S390x. These are [tier 2] platforms and may have major defects. Follow the
  instructions on the website to install, or add the targets to an existing
  installation with `rustup target add`. The new target triples are:
  - `mips-unknown-linux-gnu`
  - `mipsel-unknown-linux-gnu`
  - `mips64-unknown-linux-gnuabi64`
  - `mips64el-unknown-linux-gnuabi64 `
  - `powerpc-unknown-linux-gnu`
  - `powerpc64-unknown-linux-gnu`
  - `powerpc64le-unknown-linux-gnu`
  - `s390x-unknown-linux-gnu `
* This release includes target (std) builds for ARM Linux running MUSL
  libc. These are [tier 2] platforms and may have major defects. Add the
  following triples to an existing rustup installation with `rustup target add`:
  - `arm-unknown-linux-musleabi`
  - `arm-unknown-linux-musleabihf`
  - `armv7-unknown-linux-musleabihf`
* This release includes [experimental support for WebAssembly][1.14wasm], via
  the `wasm32-unknown-emscripten` target. This target is known to have major
  defects. Please test, report, and fix.
* rustup no longer installs documentation by default. Run `rustup
  component add rust-docs` to install.
* [Fix line stepping in debugger][37310]
* [Enable line number debuginfo in releases][37280]

Misc
----

* [Disable jemalloc on aarch64/powerpc/mips][37392]
* [Add support for Fuchsia OS][37313]
* [Detect local-rebuild by only MAJOR.MINOR version][37273]

Compatibility Notes
-------------------

* [A number of forward-compatibility lints used by the compiler
  to gradually introduce language changes have been converted
  to deny by default][36894]:
  - ["use of inaccessible extern crate erroneously allowed"][36886]
  - ["type parameter default erroneously allowed in invalid location"][36887]
  - ["detects super or self keywords at the beginning of global path"][36888]
  - ["two overlapping inherent impls define an item with the same name
    were erroneously allowed"][36889]
  - ["floating-point constants cannot be used in patterns"][36890]
  - ["constants of struct or enum type can only be used in a pattern if
     the struct or enum has `#[derive(PartialEq, Eq)]`"][36891]
  - ["lifetimes or labels named `'_` were erroneously allowed"][36892]
* [Prohibit patterns in trait methods without bodies][37378]
* [The atomic `Ordering` enum may not be matched exhaustively][37351]
* [Future-proofing `#[no_link]` breaks some obscure cases][37247]
* [The `$crate` macro variable is accepted in fewer locations][37213]
* [Impls specifying extra region requirements beyond the trait
  they implement are rejected][37167]
* [Enums may not be unsized][37111]. Unsized enums are intended to
  work but never have. For now they are forbidden.
* [Enforce the shadowing restrictions from RFC 1560 for today's macros][36767]

[tier 2]: https://forge.rust-lang.org/platform-support.html
[1.14rustup]: https://internals.rust-lang.org/t/beta-testing-rustup-rs/3316/204
[1.14wasm]: https://users.rust-lang.org/t/compiling-to-the-web-with-rust-and-emscripten/7627
[36430]: https://github.com/rust-lang/rust/pull/36430
[36595]: https://github.com/rust-lang/rust/pull/36595
[36595]: https://github.com/rust-lang/rust/pull/36595
[36692]: https://github.com/rust-lang/rust/pull/36692
[36767]: https://github.com/rust-lang/rust/pull/36767
[36794]: https://github.com/rust-lang/rust/pull/36794
[36798]: https://github.com/rust-lang/rust/pull/36798
[36819]: https://github.com/rust-lang/rust/pull/36819
[36822]: https://github.com/rust-lang/rust/pull/36822
[36825]: https://github.com/rust-lang/rust/pull/36825
[36843]: https://github.com/rust-lang/rust/pull/36843
[36880]: https://github.com/rust-lang/rust/pull/36880
[36886]: https://github.com/rust-lang/rust/issues/36886
[36887]: https://github.com/rust-lang/rust/issues/36887
[36888]: https://github.com/rust-lang/rust/issues/36888
[36889]: https://github.com/rust-lang/rust/issues/36889
[36890]: https://github.com/rust-lang/rust/issues/36890
[36891]: https://github.com/rust-lang/rust/issues/36891
[36892]: https://github.com/rust-lang/rust/issues/36892
[36894]: https://github.com/rust-lang/rust/pull/36894
[36917]: https://github.com/rust-lang/rust/pull/36917
[36993]: https://github.com/rust-lang/rust/pull/36993
[37037]: https://github.com/rust-lang/rust/pull/37037
[37064]: https://github.com/rust-lang/rust/pull/37064
[37094]: https://github.com/rust-lang/rust/pull/37094
[37108]: https://github.com/rust-lang/rust/pull/37108
[37111]: https://github.com/rust-lang/rust/pull/37111
[37161]: https://github.com/rust-lang/rust/pull/37161
[37162]: https://github.com/rust-lang/rust/pull/37162
[37167]: https://github.com/rust-lang/rust/pull/37167
[37178]: https://github.com/rust-lang/rust/pull/37178
[37200]: https://github.com/rust-lang/rust/pull/37200
[37213]: https://github.com/rust-lang/rust/pull/37213
[37221]: https://github.com/rust-lang/rust/pull/37221
[37224]: https://github.com/rust-lang/rust/pull/37224
[37230]: https://github.com/rust-lang/rust/pull/37230
[37231]: https://github.com/rust-lang/rust/pull/37231
[37247]: https://github.com/rust-lang/rust/pull/37247
[37267]: https://github.com/rust-lang/rust/pull/37267
[37270]: https://github.com/rust-lang/rust/pull/37270
[37273]: https://github.com/rust-lang/rust/pull/37273
[37280]: https://github.com/rust-lang/rust/pull/37280
[37298]: https://github.com/rust-lang/rust/pull/37298
[37306]: https://github.com/rust-lang/rust/pull/37306
[37310]: https://github.com/rust-lang/rust/pull/37310
[37312]: https://github.com/rust-lang/rust/pull/37312
[37313]: https://github.com/rust-lang/rust/pull/37313
[37315]: https://github.com/rust-lang/rust/pull/37315
[37318]: https://github.com/rust-lang/rust/pull/37318
[37322]: https://github.com/rust-lang/rust/pull/37322
[37326]: https://github.com/rust-lang/rust/pull/37326
[37351]: https://github.com/rust-lang/rust/pull/37351
[37356]: https://github.com/rust-lang/rust/pull/37356
[37367]: https://github.com/rust-lang/rust/pull/37367
[37373]: https://github.com/rust-lang/rust/pull/37373
[37378]: https://github.com/rust-lang/rust/pull/37378
[37389]: https://github.com/rust-lang/rust/pull/37389
[37392]: https://github.com/rust-lang/rust/pull/37392
[37427]: https://github.com/rust-lang/rust/pull/37427
[37439]: https://github.com/rust-lang/rust/pull/37439
[37445]: https://github.com/rust-lang/rust/pull/37445
[37470]: https://github.com/rust-lang/rust/pull/37470
[37569]: https://github.com/rust-lang/rust/pull/37569
[RFC 1492]: https://github.com/rust-lang/rfcs/blob/master/text/1492-dotdot-in-patterns.md
[cargo/3175]: https://github.com/rust-lang/cargo/pull/3175
[cargo/3220]: https://github.com/rust-lang/cargo/pull/3220
[cargo/3243]: https://github.com/rust-lang/cargo/pull/3243
[cargo/3249]: https://github.com/rust-lang/cargo/pull/3249
[cargo/3259]: https://github.com/rust-lang/cargo/pull/3259
[cargo/3280]: https://github.com/rust-lang/cargo/pull/3280


Version 1.13.0 (2016-11-10)
===========================

Language
--------

* [Stabilize the `?` operator][36995]. `?` is a simple way to propagate
  errors, like the `try!` macro, described in [RFC 0243].
* [Stabilize macros in type position][36014]. Described in [RFC 873].
* [Stabilize attributes on statements][36995]. Described in [RFC 0016].
* [Fix `#[derive]` for empty tuple structs/variants][35728]
* [Fix lifetime rules for 'if' conditions][36029]
* [Avoid loading and parsing unconfigured non-inline modules][36482]

Compiler
--------

* [Add the `-C link-arg` argument][36574]
* [Remove the old AST-based backend from rustc_trans][35764]
* [Don't enable NEON by default on armv7 Linux][35814]
* [Fix debug line number info for macro expansions][35238]
* [Do not emit "class method" debuginfo for types that are not
  DICompositeType][36008]
* [Warn about multiple conflicting #[repr] hints][34623]
* [When sizing DST, don't double-count nested struct prefixes][36351]
* [Default RUST_MIN_STACK to 16MiB for now][36505]
* [Improve rlib metadata format][36551]. Reduces rlib size significantly.
* [Reject macros with empty repetitions to avoid infinite loop][36721]
* [Expand macros without recursing to avoid stack overflows][36214]

Diagnostics
-----------

* [Replace macro backtraces with labeled local uses][35702]
* [Improve error message for misplaced doc comments][33922]
* [Buffer unix and lock windows to prevent message interleaving][35975]
* [Update lifetime errors to specifically note temporaries][36171]
* [Special case a few colors for Windows][36178]
* [Suggest `use self` when such an import resolves][36289]
* [Be more specific when type parameter shadows primitive type][36338]
* Many minor improvements

Compile-time Optimizations
--------------------------

* [Compute and cache HIR hashes at beginning][35854]
* [Don't hash types in loan paths][36004]
* [Cache projections in trans][35761]
* [Optimize the parser's last token handling][36527]
* [Only instantiate #[inline] functions in codegen units referencing
  them][36524]. This leads to big improvements in cases where crates export
  define many inline functions without using them directly.
* [Lazily allocate TypedArena's first chunk][36592]
* [Don't allocate during default HashSet creation][36734]

Stabilized APIs
---------------

* [`checked_abs`]
* [`wrapping_abs`]
* [`overflowing_abs`]
* [`RefCell::try_borrow`]
* [`RefCell::try_borrow_mut`]

Libraries
---------

* [Add `assert_ne!` and `debug_assert_ne!`][35074]
* [Make `vec_deque::Drain`, `hash_map::Drain`, and `hash_set::Drain`
  covariant][35354]
* [Implement `AsRef<[T]>` for `std::slice::Iter`][35559]
* [Implement `Debug` for `std::vec::IntoIter`][35707]
* [`CString`: avoid excessive growth just to 0-terminate][35871]
* [Implement `CoerceUnsized` for `{Cell, RefCell, UnsafeCell}`][35627]
* [Use arc4rand on FreeBSD][35884]
* [memrchr: Correct aligned offset computation][35969]
* [Improve Demangling of Rust Symbols][36059]
* [Use monotonic time in condition variables][35048]
* [Implement `Debug` for `std::path::{Components,Iter}`][36101]
* [Implement conversion traits for `char`][35755]
* [Fix illegal instruction caused by overflow in channel cloning][36104]
* [Zero first byte of CString on drop][36264]
* [Inherit overflow checks for sum and product][36372]
* [Add missing Eq implementations][36423]
* [Implement `Debug` for `DirEntry`][36631]
* [When `getaddrinfo` returns `EAI_SYSTEM` retrieve actual error from
  `errno`][36754]
* [`SipHasher`] is deprecated. Use [`DefaultHasher`].
* [Implement more traits for `std::io::ErrorKind`][35911]
* [Optimize BinaryHeap bounds checking][36072]
* [Work around pointer aliasing issue in `Vec::extend_from_slice`,
  `extend_with_element`][36355]
* [Fix overflow checking in unsigned pow()][34942]

Cargo
-----

* This release includes security fixes to both curl and OpenSSL.
* [Fix transitive doctests when panic=abort][cargo/3021]
* [Add --all-features flag to cargo][cargo/3038]
* [Reject path-based dependencies in `cargo package`][cargo/3060]
* [Don't parse the home directory more than once][cargo/3078]
* [Don't try to generate Cargo.lock on empty workspaces][cargo/3092]
* [Update OpenSSL to 1.0.2j][cargo/3121]
* [Add license and license_file to cargo metadata output][cargo/3110]
* [Make crates-io registry URL optional in config; ignore all changes to
  source.crates-io][cargo/3089]
* [Don't download dependencies from other platforms][cargo/3123]
* [Build transitive dev-dependencies when needed][cargo/3125]
* [Add support for per-target rustflags in .cargo/config][cargo/3157]
* [Avoid updating registry when adding existing deps][cargo/3144]
* [Warn about path overrides that won't work][cargo/3136]
* [Use workspaces during `cargo install`][cargo/3146]
* [Leak mspdbsrv.exe processes on Windows][cargo/3162]
* [Add --message-format flag][cargo/3000]
* [Pass target environment for rustdoc][cargo/3205]
* [Use `CommandExt::exec` for `cargo run` on Unix][cargo/2818]
* [Update curl and curl-sys][cargo/3241]
* [Call rustdoc test with the correct cfg flags of a package][cargo/3242]

Tooling
-------

* [rustdoc: Add the `--sysroot` argument][36586]
* [rustdoc: Fix a couple of issues with the search results][35655]
* [rustdoc: remove the `!` from macro URLs and titles][35234]
* [gdb: Fix pretty-printing special-cased Rust types][35585]
* [rustdoc: Filter more incorrect methods inherited through Deref][36266]

Misc
----

* [Remove unmaintained style guide][35124]
* [Add s390x support][36369]
* [Initial work at Haiku OS support][36727]
* [Add mips-uclibc targets][35734]
* [Crate-ify compiler-rt into compiler-builtins][35021]
* [Add rustc version info (git hash + date) to dist tarball][36213]
* Many documentation improvements

Compatibility Notes
-------------------

* [`SipHasher`] is deprecated. Use [`DefaultHasher`].
* [Deny (by default) transmuting from fn item types to pointer-sized
  types][34923]. Continuing the long transition to zero-sized fn items,
  per [RFC 401].
* [Fix `#[derive]` for empty tuple structs/variants][35728].
  Part of [RFC 1506].
* [Issue deprecation warnings for safe accesses to extern statics][36173]
* [Fix lifetime rules for 'if' conditions][36029].
* [Inherit overflow checks for sum and product][36372].
* [Forbid user-defined macros named "macro_rules"][36730].

[33922]: https://github.com/rust-lang/rust/pull/33922
[34623]: https://github.com/rust-lang/rust/pull/34623
[34923]: https://github.com/rust-lang/rust/pull/34923
[34942]: https://github.com/rust-lang/rust/pull/34942
[34982]: https://github.com/rust-lang/rust/pull/34982
[35021]: https://github.com/rust-lang/rust/pull/35021
[35048]: https://github.com/rust-lang/rust/pull/35048
[35074]: https://github.com/rust-lang/rust/pull/35074
[35124]: https://github.com/rust-lang/rust/pull/35124
[35234]: https://github.com/rust-lang/rust/pull/35234
[35238]: https://github.com/rust-lang/rust/pull/35238
[35354]: https://github.com/rust-lang/rust/pull/35354
[35559]: https://github.com/rust-lang/rust/pull/35559
[35585]: https://github.com/rust-lang/rust/pull/35585
[35627]: https://github.com/rust-lang/rust/pull/35627
[35655]: https://github.com/rust-lang/rust/pull/35655
[35702]: https://github.com/rust-lang/rust/pull/35702
[35707]: https://github.com/rust-lang/rust/pull/35707
[35728]: https://github.com/rust-lang/rust/pull/35728
[35734]: https://github.com/rust-lang/rust/pull/35734
[35755]: https://github.com/rust-lang/rust/pull/35755
[35761]: https://github.com/rust-lang/rust/pull/35761
[35764]: https://github.com/rust-lang/rust/pull/35764
[35814]: https://github.com/rust-lang/rust/pull/35814
[35854]: https://github.com/rust-lang/rust/pull/35854
[35871]: https://github.com/rust-lang/rust/pull/35871
[35884]: https://github.com/rust-lang/rust/pull/35884
[35911]: https://github.com/rust-lang/rust/pull/35911
[35969]: https://github.com/rust-lang/rust/pull/35969
[35975]: https://github.com/rust-lang/rust/pull/35975
[36004]: https://github.com/rust-lang/rust/pull/36004
[36008]: https://github.com/rust-lang/rust/pull/36008
[36014]: https://github.com/rust-lang/rust/pull/36014
[36029]: https://github.com/rust-lang/rust/pull/36029
[36059]: https://github.com/rust-lang/rust/pull/36059
[36072]: https://github.com/rust-lang/rust/pull/36072
[36101]: https://github.com/rust-lang/rust/pull/36101
[36104]: https://github.com/rust-lang/rust/pull/36104
[36171]: https://github.com/rust-lang/rust/pull/36171
[36173]: https://github.com/rust-lang/rust/pull/36173
[36178]: https://github.com/rust-lang/rust/pull/36178
[36213]: https://github.com/rust-lang/rust/pull/36213
[36214]: https://github.com/rust-lang/rust/pull/36214
[36264]: https://github.com/rust-lang/rust/pull/36264
[36266]: https://github.com/rust-lang/rust/pull/36266
[36289]: https://github.com/rust-lang/rust/pull/36289
[36338]: https://github.com/rust-lang/rust/pull/36338
[36351]: https://github.com/rust-lang/rust/pull/36351
[36355]: https://github.com/rust-lang/rust/pull/36355
[36369]: https://github.com/rust-lang/rust/pull/36369
[36372]: https://github.com/rust-lang/rust/pull/36372
[36423]: https://github.com/rust-lang/rust/pull/36423
[36482]: https://github.com/rust-lang/rust/pull/36482
[36505]: https://github.com/rust-lang/rust/pull/36505
[36524]: https://github.com/rust-lang/rust/pull/36524
[36527]: https://github.com/rust-lang/rust/pull/36527
[36551]: https://github.com/rust-lang/rust/pull/36551
[36574]: https://github.com/rust-lang/rust/pull/36574
[36586]: https://github.com/rust-lang/rust/pull/36586
[36592]: https://github.com/rust-lang/rust/pull/36592
[36631]: https://github.com/rust-lang/rust/pull/36631
[36639]: https://github.com/rust-lang/rust/pull/36639
[36721]: https://github.com/rust-lang/rust/pull/36721
[36727]: https://github.com/rust-lang/rust/pull/36727
[36730]: https://github.com/rust-lang/rust/pull/36730
[36734]: https://github.com/rust-lang/rust/pull/36734
[36754]: https://github.com/rust-lang/rust/pull/36754
[36995]: https://github.com/rust-lang/rust/pull/36995
[RFC 0016]: https://github.com/rust-lang/rfcs/blob/master/text/0016-more-attributes.md
[RFC 0243]: https://github.com/rust-lang/rfcs/blob/master/text/0243-trait-based-exception-handling.md
[RFC 1506]: https://github.com/rust-lang/rfcs/blob/master/text/1506-adt-kinds.md
[RFC 401]: https://github.com/rust-lang/rfcs/blob/master/text/0401-coercions.md
[RFC 873]: https://github.com/rust-lang/rfcs/blob/master/text/0873-type-macros.md
[cargo/2818]: https://github.com/rust-lang/cargo/pull/2818
[cargo/3000]: https://github.com/rust-lang/cargo/pull/3000
[cargo/3021]: https://github.com/rust-lang/cargo/pull/3021
[cargo/3038]: https://github.com/rust-lang/cargo/pull/3038
[cargo/3060]: https://github.com/rust-lang/cargo/pull/3060
[cargo/3078]: https://github.com/rust-lang/cargo/pull/3078
[cargo/3089]: https://github.com/rust-lang/cargo/pull/3089
[cargo/3092]: https://github.com/rust-lang/cargo/pull/3092
[cargo/3110]: https://github.com/rust-lang/cargo/pull/3110
[cargo/3121]: https://github.com/rust-lang/cargo/pull/3121
[cargo/3123]: https://github.com/rust-lang/cargo/pull/3123
[cargo/3125]: https://github.com/rust-lang/cargo/pull/3125
[cargo/3136]: https://github.com/rust-lang/cargo/pull/3136
[cargo/3144]: https://github.com/rust-lang/cargo/pull/3144
[cargo/3146]: https://github.com/rust-lang/cargo/pull/3146
[cargo/3157]: https://github.com/rust-lang/cargo/pull/3157
[cargo/3162]: https://github.com/rust-lang/cargo/pull/3162
[cargo/3205]: https://github.com/rust-lang/cargo/pull/3205
[cargo/3241]: https://github.com/rust-lang/cargo/pull/3241
[cargo/3242]: https://github.com/rust-lang/cargo/pull/3242
[rustup]: https://www.rustup.rs
[`checked_abs`]: https://doc.rust-lang.org/std/primitive.i32.html#method.checked_abs
[`wrapping_abs`]: https://doc.rust-lang.org/std/primitive.i32.html#method.wrapping_abs
[`overflowing_abs`]: https://doc.rust-lang.org/std/primitive.i32.html#method.overflowing_abs
[`RefCell::try_borrow`]: https://doc.rust-lang.org/std/cell/struct.RefCell.html#method.try_borrow
[`RefCell::try_borrow_mut`]: https://doc.rust-lang.org/std/cell/struct.RefCell.html#method.try_borrow_mut
[`SipHasher`]: https://doc.rust-lang.org/std/hash/struct.SipHasher.html
[`DefaultHasher`]: https://doc.rust-lang.org/std/collections/hash_map/struct.DefaultHasher.html


Version 1.12.1 (2016-10-20)
===========================

Regression Fixes
----------------

* [ICE: 'rustc' panicked at 'assertion failed: concrete_substs.is_normalized_for_trans()' #36381][36381]
* [Confusion with double negation and booleans][36856]
* [rustc 1.12.0 fails with SIGSEGV in release mode (syn crate 0.8.0)][36875]
* [Rustc 1.12.0 Windows build of `ethcore` crate fails with LLVM error][36924]
* [1.12.0: High memory usage when linking in release mode with debug info][36926]
* [Corrupted memory after updated to 1.12][36936]
* ["Let NullaryConstructor = something;" causes internal compiler error: "tried to overwrite interned AdtDef"][37026]
* [Fix ICE: inject bitcast if types mismatch for invokes/calls/stores][37112]
* [debuginfo: Handle spread_arg case in MIR-trans in a more stable way.][37153]

[36381]: https://github.com/rust-lang/rust/issues/36381
[36856]: https://github.com/rust-lang/rust/issues/36856
[36875]: https://github.com/rust-lang/rust/issues/36875
[36924]: https://github.com/rust-lang/rust/issues/36924
[36926]: https://github.com/rust-lang/rust/issues/36926
[36936]: https://github.com/rust-lang/rust/issues/36936
[37026]: https://github.com/rust-lang/rust/issues/37026
[37112]: https://github.com/rust-lang/rust/issues/37112
[37153]: https://github.com/rust-lang/rust/issues/37153


Version 1.12.0 (2016-09-29)
===========================

Highlights
----------

* [`rustc` translates code to LLVM IR via its own "middle" IR (MIR)](https://github.com/rust-lang/rust/pull/34096).
  This translation pass is far simpler than the previous AST->LLVM pass, and
  creates opportunities to perform new optimizations directly on the MIR. It
  was previously described [on the Rust blog](https://blog.rust-lang.org/2016/04/19/MIR.html).
* [`rustc` presents a new, more readable error format, along with
  machine-readable JSON error output for use by IDEs](https://github.com/rust-lang/rust/pull/35401).
  Most common editors supporting Rust have been updated to work with it. It was
  previously described [on the Rust blog](https://blog.rust-lang.org/2016/08/10/Shape-of-errors-to-come.html).

Compiler
--------

* [`rustc` translates code to LLVM IR via its own "middle" IR (MIR)](https://github.com/rust-lang/rust/pull/34096).
  This translation pass is far simpler than the previous AST->LLVM pass, and
  creates opportunities to perform new optimizations directly on the MIR. It
  was previously described [on the Rust blog](https://blog.rust-lang.org/2016/04/19/MIR.html).
* [Print the Rust target name, not the LLVM target name, with
  `--print target-list`](https://github.com/rust-lang/rust/pull/35489)
* [The computation of `TypeId` is correct in some cases where it was previously
  producing inconsistent results](https://github.com/rust-lang/rust/pull/35267)
* [The `mips-unknown-linux-gnu` target uses hardware floating point by default](https://github.com/rust-lang/rust/pull/34910)
* [The `rustc` arguments, `--print target-cpus`, `--print target-features`,
  `--print relocation-models`, and `--print code-models` print the available
  options to the `-C target-cpu`, `-C target-feature`, `-C relocation-model` and
  `-C code-model` code generation arguments](https://github.com/rust-lang/rust/pull/34845)
* [`rustc` supports three new MUSL targets on ARM: `arm-unknown-linux-musleabi`,
  `arm-unknown-linux-musleabihf`, and `armv7-unknown-linux-musleabihf`](https://github.com/rust-lang/rust/pull/35060).
  These targets produce statically-linked binaries. There are no binary release
  builds yet though.

Diagnostics
-----------

* [`rustc` presents a new, more readable error format, along with
  machine-readable JSON error output for use by IDEs](https://github.com/rust-lang/rust/pull/35401).
  Most common editors supporting Rust have been updated to work with it. It was
  previously described [on the Rust blog](https://blog.rust-lang.org/2016/08/10/Shape-of-errors-to-come.html).
* [In error descriptions, references are now described in plain English,
  instead of as "&-ptr"](https://github.com/rust-lang/rust/pull/35611)
* [In error type descriptions, unknown numeric types are named `{integer}` or
  `{float}` instead of `_`](https://github.com/rust-lang/rust/pull/35080)
* [`rustc` emits a clearer error when inner attributes follow a doc comment](https://github.com/rust-lang/rust/pull/34676)

Language
--------

* [`macro_rules!` invocations can be made within `macro_rules!` invocations](https://github.com/rust-lang/rust/pull/34925)
* [`macro_rules!` meta-variables are hygienic](https://github.com/rust-lang/rust/pull/35453)
* [`macro_rules!` `tt` matchers can be reparsed correctly, making them much more
  useful](https://github.com/rust-lang/rust/pull/34908)
* [`macro_rules!` `stmt` matchers correctly consume the entire contents when
  inside non-braces invocations](https://github.com/rust-lang/rust/pull/34886)
* [Semicolons are properly required as statement delimiters inside
  `macro_rules!` invocations](https://github.com/rust-lang/rust/pull/34660)
* [`cfg_attr` works on `path` attributes](https://github.com/rust-lang/rust/pull/34546)

Stabilized APIs
---------------

* [`Cell::as_ptr`](https://doc.rust-lang.org/std/cell/struct.Cell.html#method.as_ptr)
* [`RefCell::as_ptr`](https://doc.rust-lang.org/std/cell/struct.RefCell.html#method.as_ptr)
* [`IpAddr::is_unspecified`](https://doc.rust-lang.org/std/net/enum.IpAddr.html#method.is_unspecified)
* [`IpAddr::is_loopback`](https://doc.rust-lang.org/std/net/enum.IpAddr.html#method.is_loopback)
* [`IpAddr::is_multicast`](https://doc.rust-lang.org/std/net/enum.IpAddr.html#method.is_multicast)
* [`Ipv4Addr::is_unspecified`](https://doc.rust-lang.org/std/net/struct.Ipv4Addr.html#method.is_unspecified)
* [`Ipv6Addr::octets`](https://doc.rust-lang.org/std/net/struct.Ipv6Addr.html#method.octets)
* [`LinkedList::contains`](https://doc.rust-lang.org/std/collections/linked_list/struct.LinkedList.html#method.contains)
* [`VecDeque::contains`](https://doc.rust-lang.org/std/collections/vec_deque/struct.VecDeque.html#method.contains)
* [`ExitStatusExt::from_raw`](https://doc.rust-lang.org/std/os/unix/process/trait.ExitStatusExt.html#tymethod.from_raw).
  Both on Unix and Windows.
* [`Receiver::recv_timeout`](https://doc.rust-lang.org/std/sync/mpsc/struct.Receiver.html#method.recv_timeout)
* [`RecvTimeoutError`](https://doc.rust-lang.org/std/sync/mpsc/enum.RecvTimeoutError.html)
* [`BinaryHeap::peek_mut`](https://doc.rust-lang.org/std/collections/binary_heap/struct.BinaryHeap.html#method.peek_mut)
* [`PeekMut`](https://doc.rust-lang.org/std/collections/binary_heap/struct.PeekMut.html)
* [`iter::Product`](https://doc.rust-lang.org/std/iter/trait.Product.html)
* [`iter::Sum`](https://doc.rust-lang.org/std/iter/trait.Sum.html)
* [`OccupiedEntry::remove_entry`](https://doc.rust-lang.org/std/collections/btree_map/struct.OccupiedEntry.html#method.remove_entry)
* [`VacantEntry::into_key`](https://doc.rust-lang.org/std/collections/btree_map/struct.VacantEntry.html#method.into_key)

Libraries
---------

* [The `format!` macro and friends now allow a single argument to be formatted
  in multiple styles](https://github.com/rust-lang/rust/pull/33642)
* [The lifetime bounds on `[T]::binary_search_by` and
  `[T]::binary_search_by_key` have been adjusted to be more flexible](https://github.com/rust-lang/rust/pull/34762)
* [`Option` implements `From` for its contained type](https://github.com/rust-lang/rust/pull/34828)
* [`Cell`, `RefCell` and `UnsafeCell` implement `From` for their contained type](https://github.com/rust-lang/rust/pull/35392)
* [`RwLock` panics if the reader count overflows](https://github.com/rust-lang/rust/pull/35378)
* [`vec_deque::Drain`, `hash_map::Drain` and `hash_set::Drain` are covariant](https://github.com/rust-lang/rust/pull/35354)
* [`vec::Drain` and `binary_heap::Drain` are covariant](https://github.com/rust-lang/rust/pull/34951)
* [`Cow<str>` implements `FromIterator` for `char`, `&str` and `String`](https://github.com/rust-lang/rust/pull/35064)
* [Sockets on Linux are correctly closed in subprocesses via `SOCK_CLOEXEC`](https://github.com/rust-lang/rust/pull/34946)
* [`hash_map::Entry`, `hash_map::VacantEntry` and `hash_map::OccupiedEntry`
  implement `Debug`](https://github.com/rust-lang/rust/pull/34937)
* [`btree_map::Entry`, `btree_map::VacantEntry` and `btree_map::OccupiedEntry`
  implement `Debug`](https://github.com/rust-lang/rust/pull/34885)
* [`String` implements `AddAssign`](https://github.com/rust-lang/rust/pull/34890)
* [Variadic `extern fn` pointers implement the `Clone`, `PartialEq`, `Eq`,
  `PartialOrd`, `Ord`, `Hash`, `fmt::Pointer`, and `fmt::Debug` traits](https://github.com/rust-lang/rust/pull/34879)
* [`FileType` implements `Debug`](https://github.com/rust-lang/rust/pull/34757)
* [References to `Mutex` and `RwLock` are unwind-safe](https://github.com/rust-lang/rust/pull/34756)
* [`mpsc::sync_channel` `Receiver`s return any available message before
  reporting a disconnect](https://github.com/rust-lang/rust/pull/34731)
* [Unicode definitions have been updated to 9.0](https://github.com/rust-lang/rust/pull/34599)
* [`env` iterators implement `DoubleEndedIterator`](https://github.com/rust-lang/rust/pull/33312)

Cargo
-----

* [Support local mirrors of registries](https://github.com/rust-lang/cargo/pull/2857)
* [Add support for command aliases](https://github.com/rust-lang/cargo/pull/2679)
* [Allow `opt-level="s"` / `opt-level="z"` in profile overrides](https://github.com/rust-lang/cargo/pull/3007)
* [Make `cargo doc --open --target` work as expected](https://github.com/rust-lang/cargo/pull/2988)
* [Speed up noop registry updates](https://github.com/rust-lang/cargo/pull/2974)
* [Update OpenSSL](https://github.com/rust-lang/cargo/pull/2971)
* [Fix `--panic=abort` with plugins](https://github.com/rust-lang/cargo/pull/2954)
* [Always pass `-C metadata` to the compiler](https://github.com/rust-lang/cargo/pull/2946)
* [Fix depending on git repos with workspaces](https://github.com/rust-lang/cargo/pull/2938)
* [Add a `--lib` flag to `cargo new`](https://github.com/rust-lang/cargo/pull/2921)
* [Add `http.cainfo` for custom certs](https://github.com/rust-lang/cargo/pull/2917)
* [Indicate the compilation profile after compiling](https://github.com/rust-lang/cargo/pull/2909)
* [Allow enabling features for dependencies with `--features`](https://github.com/rust-lang/cargo/pull/2876)
* [Add `--jobs` flag to `cargo package`](https://github.com/rust-lang/cargo/pull/2867)
* [Add `--dry-run` to `cargo publish`](https://github.com/rust-lang/cargo/pull/2849)
* [Add support for `RUSTDOCFLAGS`](https://github.com/rust-lang/cargo/pull/2794)

Performance
-----------

* [`panic::catch_unwind` is more optimized](https://github.com/rust-lang/rust/pull/35444)
* [`panic::catch_unwind` no longer accesses thread-local storage on entry](https://github.com/rust-lang/rust/pull/34866)

Tooling
-------

* [Test binaries now support a `--test-threads` argument to specify the number
  of threads used to run tests, and which acts the same as the
  `RUST_TEST_THREADS` environment variable](https://github.com/rust-lang/rust/pull/35414)
* [The test runner now emits a warning when tests run over 60 seconds](https://github.com/rust-lang/rust/pull/35405)
* [rustdoc: Fix methods in search results](https://github.com/rust-lang/rust/pull/34752)
* [`rust-lldb` warns about unsupported versions of LLDB](https://github.com/rust-lang/rust/pull/34646)
* [Rust releases now come with source packages that can be installed by rustup
  via `rustup component add rust-src`](https://github.com/rust-lang/rust/pull/34366).
  The resulting source code can be used by tools and IDES, located in the
  sysroot under `lib/rustlib/src`.

Misc
----

* [The compiler can now be built against LLVM 3.9](https://github.com/rust-lang/rust/pull/35594)
* Many minor improvements to the documentation.
* [The Rust exception handling "personality" routine is now written in Rust](https://github.com/rust-lang/rust/pull/34832)

Compatibility Notes
-------------------

* [When printing Windows `OsStr`s, unpaired surrogate codepoints are escaped
  with the lowercase format instead of the uppercase](https://github.com/rust-lang/rust/pull/35084)
* [When formatting strings, if "precision" is specified, the "fill",
  "align" and "width" specifiers are no longer ignored](https://github.com/rust-lang/rust/pull/34544)
* [The `Debug` impl for strings no longer escapes all non-ASCII characters](https://github.com/rust-lang/rust/pull/34485)


Version 1.11.0 (2016-08-18)
===========================

Language
--------

* [`cfg_attr` works on `path` attributes](https://github.com/rust-lang/rust/pull/34546)
* [Support nested `cfg_attr` attributes](https://github.com/rust-lang/rust/pull/34216)
* [Allow statement-generating braced macro invocations at the end of blocks](https://github.com/rust-lang/rust/pull/34436)
* [Macros can be expanded inside of trait definitions](https://github.com/rust-lang/rust/pull/34213)
* [`#[macro_use]` works properly when it is itself expanded from a macro](https://github.com/rust-lang/rust/pull/34032)

Stabilized APIs
---------------

* [`BinaryHeap::append`](https://doc.rust-lang.org/std/collections/binary_heap/struct.BinaryHeap.html#method.append)
* [`BTreeMap::append`](https://doc.rust-lang.org/std/collections/btree_map/struct.BTreeMap.html#method.append)
* [`BTreeMap::split_off`](https://doc.rust-lang.org/std/collections/btree_map/struct.BTreeMap.html#method.split_off)
* [`BTreeSet::append`](https://doc.rust-lang.org/std/collections/btree_set/struct.BTreeSet.html#method.append)
* [`BTreeSet::split_off`](https://doc.rust-lang.org/std/collections/btree_set/struct.BTreeSet.html#method.split_off)
* [`f32::to_degrees`](https://doc.rust-lang.org/std/primitive.f32.html#method.to_degrees)
  (in libcore - previously stabilized in libstd)
* [`f32::to_radians`](https://doc.rust-lang.org/std/primitive.f32.html#method.to_radians)
  (in libcore - previously stabilized in libstd)
* [`f64::to_degrees`](https://doc.rust-lang.org/std/primitive.f64.html#method.to_degrees)
  (in libcore - previously stabilized in libstd)
* [`f64::to_radians`](https://doc.rust-lang.org/std/primitive.f64.html#method.to_radians)
  (in libcore - previously stabilized in libstd)
* [`Iterator::sum`](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.sum)
* [`Iterator::product`](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.sum)
* [`Cell::get_mut`](https://doc.rust-lang.org/std/cell/struct.Cell.html#method.get_mut)
* [`RefCell::get_mut`](https://doc.rust-lang.org/std/cell/struct.RefCell.html#method.get_mut)

Libraries
---------

* [The `thread_local!` macro supports multiple definitions in a single
   invocation, and can apply attributes](https://github.com/rust-lang/rust/pull/34077)
* [`Cow` implements `Default`](https://github.com/rust-lang/rust/pull/34305)
* [`Wrapping` implements binary, octal, lower-hex and upper-hex
  `Display` formatting](https://github.com/rust-lang/rust/pull/34190)
* [The range types implement `Hash`](https://github.com/rust-lang/rust/pull/34180)
* [`lookup_host` ignores unknown address types](https://github.com/rust-lang/rust/pull/34067)
* [`assert_eq!` accepts a custom error message, like `assert!` does](https://github.com/rust-lang/rust/pull/33976)
* [The main thread is now called "main" instead of "&lt;main&gt;"](https://github.com/rust-lang/rust/pull/33803)

Cargo
-----

* [Disallow specifying features of transitive deps](https://github.com/rust-lang/cargo/pull/2821)
* [Add color support for Windows consoles](https://github.com/rust-lang/cargo/pull/2804)
* [Fix `harness = false` on `[lib]` sections](https://github.com/rust-lang/cargo/pull/2795)
* [Don't panic when `links` contains a '.'](https://github.com/rust-lang/cargo/pull/2787)
* [Build scripts can emit warnings](https://github.com/rust-lang/cargo/pull/2630),
  and `-vv` prints warnings for all crates.
* [Ignore file locks on OS X NFS mounts](https://github.com/rust-lang/cargo/pull/2720)
* [Don't warn about `package.metadata` keys](https://github.com/rust-lang/cargo/pull/2668).
  This provides room for expansion by arbitrary tools.
* [Add support for cdylib crate types](https://github.com/rust-lang/cargo/pull/2741)
* [Prevent publishing crates when files are dirty](https://github.com/rust-lang/cargo/pull/2781)
* [Don't fetch all crates on clean](https://github.com/rust-lang/cargo/pull/2704)
* [Propagate --color option to rustc](https://github.com/rust-lang/cargo/pull/2779)
* [Fix `cargo doc --open` on Windows](https://github.com/rust-lang/cargo/pull/2780)
* [Improve autocompletion](https://github.com/rust-lang/cargo/pull/2772)
* [Configure colors of stderr as well as stdout](https://github.com/rust-lang/cargo/pull/2739)

Performance
-----------

* [Caching projections speeds up type check dramatically for some
  workloads](https://github.com/rust-lang/rust/pull/33816)
* [The default `HashMap` hasher is SipHash 1-3 instead of SipHash 2-4](https://github.com/rust-lang/rust/pull/33940)
  This hasher is faster, but is believed to provide sufficient
  protection from collision attacks.
* [Comparison of `Ipv4Addr` is 10x faster](https://github.com/rust-lang/rust/pull/33891)

Rustdoc
-------

* [Fix empty implementation section on some module pages](https://github.com/rust-lang/rust/pull/34536)
* [Fix inlined renamed re-exports in import lists](https://github.com/rust-lang/rust/pull/34479)
* [Fix search result layout for enum variants and struct fields](https://github.com/rust-lang/rust/pull/34477)
* [Fix issues with source links to external crates](https://github.com/rust-lang/rust/pull/34387)
* [Fix redirect pages for renamed re-exports](https://github.com/rust-lang/rust/pull/34245)

Tooling
-------

* [rustc is better at finding the MSVC toolchain](https://github.com/rust-lang/rust/pull/34492)
* [When emitting debug info, rustc emits frame pointers for closures,
  shims and glue, as it does for all other functions](https://github.com/rust-lang/rust/pull/33909)
* [rust-lldb warns about unsupported versions of LLDB](https://github.com/rust-lang/rust/pull/34646)
* Many more errors have been given error codes and extended
  explanations
* API documentation continues to be improved, with many new examples

Misc
----

* [rustc no longer hangs when dependencies recursively re-export
  submodules](https://github.com/rust-lang/rust/pull/34542)
* [rustc requires LLVM 3.7+](https://github.com/rust-lang/rust/pull/34104)
* [The 'How Safe and Unsafe Interact' chapter of The Rustonomicon was
  rewritten](https://github.com/rust-lang/rust/pull/33895)
* [rustc support 16-bit pointer sizes](https://github.com/rust-lang/rust/pull/33460).
  No targets use this yet, but it works toward AVR support.

Compatibility Notes
-------------------

* [`const`s and `static`s may not have unsized types](https://github.com/rust-lang/rust/pull/34443)
* [The new follow-set rules that place restrictions on `macro_rules!`
  in order to ensure syntax forward-compatibility have been enabled](https://github.com/rust-lang/rust/pull/33982)
  This was an [amendment to RFC 550](https://github.com/rust-lang/rfcs/pull/1384),
  and has been a warning since 1.10.
* [`cfg` attribute process has been refactored to fix various bugs](https://github.com/rust-lang/rust/pull/33706).
  This causes breakage in some corner cases.


Version 1.10.0 (2016-07-07)
===========================

Language
--------

* [Allow `concat_idents!` in type positions as well as in expression
  positions](https://github.com/rust-lang/rust/pull/33735).
* [`Copy` types are required to have a trivial implementation of `Clone`](https://github.com/rust-lang/rust/pull/33420).
  [RFC 1521](https://github.com/rust-lang/rfcs/blob/master/text/1521-copy-clone-semantics.md).
* [Single-variant enums support the `#[repr(..)]` attribute](https://github.com/rust-lang/rust/pull/33355).
* [Fix `#[derive(RustcEncodable)]` in the presence of other `encode` methods](https://github.com/rust-lang/rust/pull/32908).
* [`panic!` can be converted to a runtime abort with the
  `-C panic=abort` flag](https://github.com/rust-lang/rust/pull/32900).
  [RFC 1513](https://github.com/rust-lang/rfcs/blob/master/text/1513-less-unwinding.md).
* [Add a new crate type, 'cdylib'](https://github.com/rust-lang/rust/pull/33553).
  cdylibs are dynamic libraries suitable for loading by non-Rust hosts.
  [RFC 1510](https://github.com/rust-lang/rfcs/blob/master/text/1510-cdylib.md).
  Note that Cargo does not yet directly support cdylibs.

Stabilized APIs
---------------

* `os::windows::fs::OpenOptionsExt::access_mode`
* `os::windows::fs::OpenOptionsExt::share_mode`
* `os::windows::fs::OpenOptionsExt::custom_flags`
* `os::windows::fs::OpenOptionsExt::attributes`
* `os::windows::fs::OpenOptionsExt::security_qos_flags`
* `os::unix::fs::OpenOptionsExt::custom_flags`
* [`sync::Weak::new`](http://doc.rust-lang.org/alloc/arc/struct.Weak.html#method.new)
* `Default for sync::Weak`
* [`panic::set_hook`](http://doc.rust-lang.org/std/panic/fn.set_hook.html)
* [`panic::take_hook`](http://doc.rust-lang.org/std/panic/fn.take_hook.html)
* [`panic::PanicInfo`](http://doc.rust-lang.org/std/panic/struct.PanicInfo.html)
* [`panic::PanicInfo::payload`](http://doc.rust-lang.org/std/panic/struct.PanicInfo.html#method.payload)
* [`panic::PanicInfo::location`](http://doc.rust-lang.org/std/panic/struct.PanicInfo.html#method.location)
* [`panic::Location`](http://doc.rust-lang.org/std/panic/struct.Location.html)
* [`panic::Location::file`](http://doc.rust-lang.org/std/panic/struct.Location.html#method.file)
* [`panic::Location::line`](http://doc.rust-lang.org/std/panic/struct.Location.html#method.line)
* [`ffi::CStr::from_bytes_with_nul`](http://doc.rust-lang.org/std/ffi/struct.CStr.html#method.from_bytes_with_nul)
* [`ffi::CStr::from_bytes_with_nul_unchecked`](http://doc.rust-lang.org/std/ffi/struct.CStr.html#method.from_bytes_with_nul_unchecked)
* [`ffi::FromBytesWithNulError`](http://doc.rust-lang.org/std/ffi/struct.FromBytesWithNulError.html)
* [`fs::Metadata::modified`](http://doc.rust-lang.org/std/fs/struct.Metadata.html#method.modified)
* [`fs::Metadata::accessed`](http://doc.rust-lang.org/std/fs/struct.Metadata.html#method.accessed)
* [`fs::Metadata::created`](http://doc.rust-lang.org/std/fs/struct.Metadata.html#method.created)
* `sync::atomic::Atomic{Usize,Isize,Bool,Ptr}::compare_exchange`
* `sync::atomic::Atomic{Usize,Isize,Bool,Ptr}::compare_exchange_weak`
* `collections::{btree,hash}_map::{Occupied,Vacant,}Entry::key`
* `os::unix::net::{UnixStream, UnixListener, UnixDatagram, SocketAddr}`
* [`SocketAddr::is_unnamed`](http://doc.rust-lang.org/std/os/unix/net/struct.SocketAddr.html#method.is_unnamed)
* [`SocketAddr::as_pathname`](http://doc.rust-lang.org/std/os/unix/net/struct.SocketAddr.html#method.as_pathname)
* [`UnixStream::connect`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.connect)
* [`UnixStream::pair`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.pair)
* [`UnixStream::try_clone`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.try_clone)
* [`UnixStream::local_addr`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.local_addr)
* [`UnixStream::peer_addr`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.peer_addr)
* [`UnixStream::set_read_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.read_timeout)
* [`UnixStream::set_write_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.write_timeout)
* [`UnixStream::read_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.read_timeout)
* [`UnixStream::write_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.write_timeout)
* [`UnixStream::set_nonblocking`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.set_nonblocking)
* [`UnixStream::take_error`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.take_error)
* [`UnixStream::shutdown`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixStream.html#method.shutdown)
* Read/Write/RawFd impls for `UnixStream`
* [`UnixListener::bind`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.bind)
* [`UnixListener::accept`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.accept)
* [`UnixListener::try_clone`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.try_clone)
* [`UnixListener::local_addr`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.local_addr)
* [`UnixListener::set_nonblocking`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.set_nonblocking)
* [`UnixListener::take_error`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.take_error)
* [`UnixListener::incoming`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixListener.html#method.incoming)
* RawFd impls for `UnixListener`
* [`UnixDatagram::bind`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.bind)
* [`UnixDatagram::unbound`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.unbound)
* [`UnixDatagram::pair`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.pair)
* [`UnixDatagram::connect`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.connect)
* [`UnixDatagram::try_clone`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.try_clone)
* [`UnixDatagram::local_addr`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.local_addr)
* [`UnixDatagram::peer_addr`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.peer_addr)
* [`UnixDatagram::recv_from`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.recv_from)
* [`UnixDatagram::recv`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.recv)
* [`UnixDatagram::send_to`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.send_to)
* [`UnixDatagram::send`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.send)
* [`UnixDatagram::set_read_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.set_read_timeout)
* [`UnixDatagram::set_write_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.set_write_timeout)
* [`UnixDatagram::read_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.read_timeout)
* [`UnixDatagram::write_timeout`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.write_timeout)
* [`UnixDatagram::set_nonblocking`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.set_nonblocking)
* [`UnixDatagram::take_error`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.take_error)
* [`UnixDatagram::shutdown`](http://doc.rust-lang.org/std/os/unix/net/struct.UnixDatagram.html#method.shutdown)
* RawFd impls for `UnixDatagram`
* `{BTree,Hash}Map::values_mut`
* [`<[_]>::binary_search_by_key`](http://doc.rust-lang.org/std/primitive.slice.html#method.binary_search_by_key)

Libraries
---------

* [The `abs_sub` method of floats is deprecated](https://github.com/rust-lang/rust/pull/33664).
  The semantics of this minor method are subtle and probably not what
  most people want.
* [Add implementation of Ord for Cell<T> and RefCell<T> where T: Ord](https://github.com/rust-lang/rust/pull/33306).
* [On Linux, if `HashMap`s can't be initialized with `getrandom` they
  will fall back to `/dev/urandom` temporarily to avoid blocking
  during early boot](https://github.com/rust-lang/rust/pull/33086).
* [Implemented negation for wrapping numerals](https://github.com/rust-lang/rust/pull/33067).
* [Implement `Clone` for `binary_heap::IntoIter`](https://github.com/rust-lang/rust/pull/33050).
* [Implement `Display` and `Hash` for `std::num::Wrapping`](https://github.com/rust-lang/rust/pull/33023).
* [Add `Default` implementation for `&CStr`, `CString`](https://github.com/rust-lang/rust/pull/32990).
* [Implement `From<Vec<T>>` and `Into<Vec<T>>` for `VecDeque<T>`](https://github.com/rust-lang/rust/pull/32866).
* [Implement `Default` for `UnsafeCell`, `fmt::Error`, `Condvar`,
  `Mutex`, `RwLock`](https://github.com/rust-lang/rust/pull/32785).

Cargo
-----
* [Cargo.toml supports the `profile.*.panic` option](https://github.com/rust-lang/cargo/pull/2687).
  This controls the runtime behavior of the `panic!` macro
  and can be either "unwind" (the default), or "abort".
  [RFC 1513](https://github.com/rust-lang/rfcs/blob/master/text/1513-less-unwinding.md).
* [Don't throw away errors with `-p` arguments](https://github.com/rust-lang/cargo/pull/2723).
* [Report status to stderr instead of stdout](https://github.com/rust-lang/cargo/pull/2693).
* [Build scripts are passed a `CARGO_MANIFEST_LINKS` environment
  variable that corresponds to the `links` field of the manifest](https://github.com/rust-lang/cargo/pull/2710).
* [Ban keywords from crate names](https://github.com/rust-lang/cargo/pull/2707).
* [Canonicalize `CARGO_HOME` on Windows](https://github.com/rust-lang/cargo/pull/2604).
* [Retry network requests](https://github.com/rust-lang/cargo/pull/2396).
  By default they are retried twice, which can be customized with the
  `net.retry` value in `.cargo/config`.
* [Don't print extra error info for failing subcommands](https://github.com/rust-lang/cargo/pull/2674).
* [Add `--force` flag to `cargo install`](https://github.com/rust-lang/cargo/pull/2405).
* [Don't use `flock` on NFS mounts](https://github.com/rust-lang/cargo/pull/2623).
* [Prefer building `cargo install` artifacts in temporary directories](https://github.com/rust-lang/cargo/pull/2610).
  Makes it possible to install multiple crates in parallel.
* [Add `cargo test --doc`](https://github.com/rust-lang/cargo/pull/2578).
* [Add `cargo --explain`](https://github.com/rust-lang/cargo/pull/2551).
* [Don't print warnings when `-q` is passed](https://github.com/rust-lang/cargo/pull/2576).
* [Add `cargo doc --lib` and `--bin`](https://github.com/rust-lang/cargo/pull/2577).
* [Don't require build script output to be UTF-8](https://github.com/rust-lang/cargo/pull/2560).
* [Correctly attempt multiple git usernames](https://github.com/rust-lang/cargo/pull/2584).

Performance
-----------

* [rustc memory usage was reduced by refactoring the context used for
  type checking](https://github.com/rust-lang/rust/pull/33425).
* [Speed up creation of `HashMap`s by caching the random keys used
  to initialize the hash state](https://github.com/rust-lang/rust/pull/33318).
* [The `find` implementation for `Chain` iterators is 2x faster](https://github.com/rust-lang/rust/pull/33289).
* [Trait selection optimizations speed up type checking by 15%](https://github.com/rust-lang/rust/pull/33138).
* [Efficient trie lookup for boolean Unicode properties](https://github.com/rust-lang/rust/pull/33098).
  10x faster than the previous lookup tables.
* [Special case `#[derive(Copy, Clone)]` to avoid bloat](https://github.com/rust-lang/rust/pull/31414).

Usability
---------

* Many incremental improvements to documentation and rustdoc.
* [rustdoc: List blanket trait impls](https://github.com/rust-lang/rust/pull/33514).
* [rustdoc: Clean up ABI rendering](https://github.com/rust-lang/rust/pull/33151).
* [Indexing with the wrong type produces a more informative error](https://github.com/rust-lang/rust/pull/33401).
* [Improve diagnostics for constants being used in irrefutable patterns](https://github.com/rust-lang/rust/pull/33406).
* [When many method candidates are in scope limit the suggestions to 10](https://github.com/rust-lang/rust/pull/33338).
* [Remove confusing suggestion when calling a `fn` type](https://github.com/rust-lang/rust/pull/33325).
* [Do not suggest changing `&mut self` to `&mut mut self`](https://github.com/rust-lang/rust/pull/33319).

Misc
----

* [Update i686-linux-android features to match Android ABI](https://github.com/rust-lang/rust/pull/33651).
* [Update aarch64-linux-android features to match Android ABI](https://github.com/rust-lang/rust/pull/33500).
* [`std` no longer prints backtraces on platforms where the running
  module must be loaded with `env::current_exe`, which can't be relied
  on](https://github.com/rust-lang/rust/pull/33554).
* This release includes std binaries for the i586-unknown-linux-gnu,
  i686-unknown-linux-musl, and armv7-linux-androideabi targets. The
  i586 target is for old x86 hardware without SSE2, and the armv7
  target is for Android running on modern ARM architectures.
* [The `rust-gdb` and `rust-lldb` scripts are distributed on all
  Unix platforms](https://github.com/rust-lang/rust/pull/32835).
* [On Unix the runtime aborts by calling `libc::abort` instead of
  generating an illegal instruction](https://github.com/rust-lang/rust/pull/31457).
* [Rust is now bootstrapped from the previous release of Rust,
  instead of a snapshot from an arbitrary commit](https://github.com/rust-lang/rust/pull/32942).

Compatibility Notes
-------------------

* [`AtomicBool` is now bool-sized, not word-sized](https://github.com/rust-lang/rust/pull/33579).
* [`target_env` for Linux ARM targets is just `gnu`, not
  `gnueabihf`, `gnueabi`, etc](https://github.com/rust-lang/rust/pull/33403).
* [Consistently panic on overflow in `Duration::new`](https://github.com/rust-lang/rust/pull/33072).
* [Change `String::truncate` to panic less](https://github.com/rust-lang/rust/pull/32977).
* [Add `:block` to the follow set for `:ty` and `:path`](https://github.com/rust-lang/rust/pull/32945).
  Affects how macros are parsed.
* [Fix macro hygiene bug](https://github.com/rust-lang/rust/pull/32923).
* [Feature-gated attributes on macro-generated macro invocations are
  now rejected](https://github.com/rust-lang/rust/pull/32791).
* [Suppress fallback and ambiguity errors during type inference](https://github.com/rust-lang/rust/pull/32258).
  This caused some minor changes to type inference.


Version 1.9.0 (2016-05-26)
==========================

Language
--------

* The `#[deprecated]` attribute when applied to an API will generate
  warnings when used. The warnings may be suppressed with
  `#[allow(deprecated)]`. [RFC 1270].
* [`fn` item types are zero sized, and each `fn` names a unique
  type][1.9fn]. This will break code that transmutes `fn`s, so calling
  `transmute` on a `fn` type will generate a warning for a few cycles,
  then will be converted to an error.
* [Field and method resolution understand visibility, so private
  fields and methods cannot prevent the proper use of public fields
  and methods][1.9fv].
* [The parser considers unicode codepoints in the
  `PATTERN_WHITE_SPACE` category to be whitespace][1.9ws].

Stabilized APIs
---------------

* [`std::panic`]
* [`std::panic::catch_unwind`][] (renamed from `recover`)
* [`std::panic::resume_unwind`][] (renamed from `propagate`)
* [`std::panic::AssertUnwindSafe`][] (renamed from `AssertRecoverSafe`)
* [`std::panic::UnwindSafe`][] (renamed from `RecoverSafe`)
* [`str::is_char_boundary`]
* [`<*const T>::as_ref`]
* [`<*mut T>::as_ref`]
* [`<*mut T>::as_mut`]
* [`AsciiExt::make_ascii_uppercase`]
* [`AsciiExt::make_ascii_lowercase`]
* [`char::decode_utf16`]
* [`char::DecodeUtf16`]
* [`char::DecodeUtf16Error`]
* [`char::DecodeUtf16Error::unpaired_surrogate`]
* [`BTreeSet::take`]
* [`BTreeSet::replace`]
* [`BTreeSet::get`]
* [`HashSet::take`]
* [`HashSet::replace`]
* [`HashSet::get`]
* [`OsString::with_capacity`]
* [`OsString::clear`]
* [`OsString::capacity`]
* [`OsString::reserve`]
* [`OsString::reserve_exact`]
* [`OsStr::is_empty`]
* [`OsStr::len`]
* [`std::os::unix::thread`]
* [`RawPthread`]
* [`JoinHandleExt`]
* [`JoinHandleExt::as_pthread_t`]
* [`JoinHandleExt::into_pthread_t`]
* [`HashSet::hasher`]
* [`HashMap::hasher`]
* [`CommandExt::exec`]
* [`File::try_clone`]
* [`SocketAddr::set_ip`]
* [`SocketAddr::set_port`]
* [`SocketAddrV4::set_ip`]
* [`SocketAddrV4::set_port`]
* [`SocketAddrV6::set_ip`]
* [`SocketAddrV6::set_port`]
* [`SocketAddrV6::set_flowinfo`]
* [`SocketAddrV6::set_scope_id`]
* [`slice::copy_from_slice`]
* [`ptr::read_volatile`]
* [`ptr::write_volatile`]
* [`OpenOptions::create_new`]
* [`TcpStream::set_nodelay`]
* [`TcpStream::nodelay`]
* [`TcpStream::set_ttl`]
* [`TcpStream::ttl`]
* [`TcpStream::set_only_v6`]
* [`TcpStream::only_v6`]
* [`TcpStream::take_error`]
* [`TcpStream::set_nonblocking`]
* [`TcpListener::set_ttl`]
* [`TcpListener::ttl`]
* [`TcpListener::set_only_v6`]
* [`TcpListener::only_v6`]
* [`TcpListener::take_error`]
* [`TcpListener::set_nonblocking`]
* [`UdpSocket::set_broadcast`]
* [`UdpSocket::broadcast`]
* [`UdpSocket::set_multicast_loop_v4`]
* [`UdpSocket::multicast_loop_v4`]
* [`UdpSocket::set_multicast_ttl_v4`]
* [`UdpSocket::multicast_ttl_v4`]
* [`UdpSocket::set_multicast_loop_v6`]
* [`UdpSocket::multicast_loop_v6`]
* [`UdpSocket::set_multicast_ttl_v6`]
* [`UdpSocket::multicast_ttl_v6`]
* [`UdpSocket::set_ttl`]
* [`UdpSocket::ttl`]
* [`UdpSocket::set_only_v6`]
* [`UdpSocket::only_v6`]
* [`UdpSocket::join_multicast_v4`]
* [`UdpSocket::join_multicast_v6`]
* [`UdpSocket::leave_multicast_v4`]
* [`UdpSocket::leave_multicast_v6`]
* [`UdpSocket::take_error`]
* [`UdpSocket::connect`]
* [`UdpSocket::send`]
* [`UdpSocket::recv`]
* [`UdpSocket::set_nonblocking`]

Libraries
---------

* [`std::sync::Once` is poisoned if its initialization function
  fails][1.9o].
* [`cell::Ref` and `cell::RefMut` can contain unsized types][1.9cu].
* [Most types implement `fmt::Debug`][1.9db].
* [The default buffer size used by `BufReader` and `BufWriter` was
  reduced to 8K, from 64K][1.9bf]. This is in line with the buffer size
  used by other languages.
* [`Instant`, `SystemTime` and `Duration` implement `+=` and `-=`.
  `Duration` additionally implements `*=` and `/=`][1.9ta].
* [`Skip` is a `DoubleEndedIterator`][1.9sk].
* [`From<[u8; 4]>` is implemented for `Ipv4Addr`][1.9fi].
* [`Chain` implements `BufRead`][1.9ch].
* [`HashMap`, `HashSet` and iterators are covariant][1.9hc].

Cargo
-----

* [Cargo can now run concurrently][1.9cc].
* [Top-level overrides allow specific revisions of crates to be
  overridden through the entire crate graph][1.9ct].  This is intended
  to make upgrades easier for large projects, by allowing crates to be
  forked temporarily until they've been upgraded and republished.
* [Cargo exports a `CARGO_PKG_AUTHORS` environment variable][1.9cp].
* [Cargo will pass the contents of the `RUSTFLAGS` variable to `rustc`
  on the commandline][1.9cf]. `rustc` arguments can also be specified
  in the `build.rustflags` configuration key.

Performance
-----------

* [The time complexity of comparing variables for equivalence during type
  unification is reduced from _O_(_n_!) to _O_(_n_)][1.9tu]. This leads
  to major compilation time improvement in some scenarios.
* [`ToString` is specialized for `str`, giving it the same performance
  as `to_owned`][1.9ts].
* [Spawning processes with `Command::output` no longer creates extra
  threads][1.9sp].
* [`#[derive(PartialEq)]` and `#[derive(PartialOrd)]` emit less code
  for C-like enums][1.9cl].

Misc
----

* [Passing the `--quiet` flag to a test runner will produce
  much-abbreviated output][1.9q].
* The Rust Project now publishes std binaries for the
  `mips-unknown-linux-musl`, `mipsel-unknown-linux-musl`, and
  `i586-pc-windows-msvc` targets.

Compatibility Notes
-------------------

* [`std::sync::Once` is poisoned if its initialization function
  fails][1.9o].
* [It is illegal to define methods with the same name in overlapping
  inherent `impl` blocks][1.9sn].
* [`fn` item types are zero sized, and each `fn` names a unique
  type][1.9fn]. This will break code that transmutes `fn`s, so calling
  `transmute` on a `fn` type will generate a warning for a few cycles,
  then will be converted to an error.
* [Improvements to const evaluation may trigger new errors when integer
  literals are out of range][1.9ce].


[1.9bf]: https://github.com/rust-lang/rust/pull/32695
[1.9cc]: https://github.com/rust-lang/cargo/pull/2486
[1.9ce]: https://github.com/rust-lang/rust/pull/30587
[1.9cf]: https://github.com/rust-lang/cargo/pull/2241
[1.9ch]: https://github.com/rust-lang/rust/pull/32541
[1.9cl]: https://github.com/rust-lang/rust/pull/31977
[1.9cp]: https://github.com/rust-lang/cargo/pull/2465
[1.9ct]: https://github.com/rust-lang/cargo/pull/2385
[1.9cu]: https://github.com/rust-lang/rust/pull/32652
[1.9db]: https://github.com/rust-lang/rust/pull/32054
[1.9fi]: https://github.com/rust-lang/rust/pull/32050
[1.9fn]: https://github.com/rust-lang/rust/pull/31710
[1.9fv]: https://github.com/rust-lang/rust/pull/31938
[1.9hc]: https://github.com/rust-lang/rust/pull/32635
[1.9o]: https://github.com/rust-lang/rust/pull/32325
[1.9q]: https://github.com/rust-lang/rust/pull/31887
[1.9sk]: https://github.com/rust-lang/rust/pull/31700
[1.9sn]: https://github.com/rust-lang/rust/pull/31925
[1.9sp]: https://github.com/rust-lang/rust/pull/31618
[1.9ta]: https://github.com/rust-lang/rust/pull/32448
[1.9ts]: https://github.com/rust-lang/rust/pull/32586
[1.9tu]: https://github.com/rust-lang/rust/pull/32062
[1.9ws]: https://github.com/rust-lang/rust/pull/29734
[RFC 1270]: https://github.com/rust-lang/rfcs/blob/master/text/1270-deprecation.md
[`<*const T>::as_ref`]: http://doc.rust-lang.org/nightly/std/primitive.pointer.html#method.as_ref
[`<*mut T>::as_mut`]: http://doc.rust-lang.org/nightly/std/primitive.pointer.html#method.as_mut
[`<*mut T>::as_ref`]: http://doc.rust-lang.org/nightly/std/primitive.pointer.html#method.as_ref
[`slice::copy_from_slice`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.copy_from_slice
[`AsciiExt::make_ascii_lowercase`]: http://doc.rust-lang.org/nightly/std/ascii/trait.AsciiExt.html#tymethod.make_ascii_lowercase
[`AsciiExt::make_ascii_uppercase`]: http://doc.rust-lang.org/nightly/std/ascii/trait.AsciiExt.html#tymethod.make_ascii_uppercase
[`BTreeSet::get`]: http://doc.rust-lang.org/nightly/collections/btree/set/struct.BTreeSet.html#method.get
[`BTreeSet::replace`]: http://doc.rust-lang.org/nightly/collections/btree/set/struct.BTreeSet.html#method.replace
[`BTreeSet::take`]: http://doc.rust-lang.org/nightly/collections/btree/set/struct.BTreeSet.html#method.take
[`CommandExt::exec`]: http://doc.rust-lang.org/nightly/std/os/unix/process/trait.CommandExt.html#tymethod.exec
[`File::try_clone`]: http://doc.rust-lang.org/nightly/std/fs/struct.File.html#method.try_clone
[`HashMap::hasher`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashMap.html#method.hasher
[`HashSet::get`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashSet.html#method.get
[`HashSet::hasher`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashSet.html#method.hasher
[`HashSet::replace`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashSet.html#method.replace
[`HashSet::take`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashSet.html#method.take
[`JoinHandleExt::as_pthread_t`]: http://doc.rust-lang.org/nightly/std/os/unix/thread/trait.JoinHandleExt.html#tymethod.as_pthread_t
[`JoinHandleExt::into_pthread_t`]: http://doc.rust-lang.org/nightly/std/os/unix/thread/trait.JoinHandleExt.html#tymethod.into_pthread_t
[`JoinHandleExt`]: http://doc.rust-lang.org/nightly/std/os/unix/thread/trait.JoinHandleExt.html
[`OpenOptions::create_new`]: http://doc.rust-lang.org/nightly/std/fs/struct.OpenOptions.html#method.create_new
[`OsStr::is_empty`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsStr.html#method.is_empty
[`OsStr::len`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsStr.html#method.len
[`OsString::capacity`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsString.html#method.capacity
[`OsString::clear`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsString.html#method.clear
[`OsString::reserve_exact`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsString.html#method.reserve_exact
[`OsString::reserve`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsString.html#method.reserve
[`OsString::with_capacity`]: http://doc.rust-lang.org/nightly/std/ffi/struct.OsString.html#method.with_capacity
[`RawPthread`]: http://doc.rust-lang.org/nightly/std/os/unix/thread/type.RawPthread.html
[`SocketAddr::set_ip`]: http://doc.rust-lang.org/nightly/std/net/enum.SocketAddr.html#method.set_ip
[`SocketAddr::set_port`]: http://doc.rust-lang.org/nightly/std/net/enum.SocketAddr.html#method.set_port
[`SocketAddrV4::set_ip`]: http://doc.rust-lang.org/nightly/std/net/struct.SocketAddrV4.html#method.set_ip
[`SocketAddrV4::set_port`]: http://doc.rust-lang.org/nightly/std/net/struct.SocketAddrV4.html#method.set_port
[`SocketAddrV6::set_flowinfo`]: http://doc.rust-lang.org/nightly/std/net/struct.SocketAddrV6.html#method.set_flowinfo
[`SocketAddrV6::set_ip`]: http://doc.rust-lang.org/nightly/std/net/struct.SocketAddrV6.html#method.set_ip
[`SocketAddrV6::set_port`]: http://doc.rust-lang.org/nightly/std/net/struct.SocketAddrV6.html#method.set_port
[`SocketAddrV6::set_scope_id`]: http://doc.rust-lang.org/nightly/std/net/struct.SocketAddrV6.html#method.set_scope_id
[`TcpListener::only_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.only_v6
[`TcpListener::set_nonblocking`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_nonblocking
[`TcpListener::set_only_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_only_v6
[`TcpListener::set_ttl`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_ttl
[`TcpListener::take_error`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.take_error
[`TcpListener::ttl`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.ttl
[`TcpStream::nodelay`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.nodelay
[`TcpStream::only_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.only_v6
[`TcpStream::set_nodelay`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_nodelay
[`TcpStream::set_nonblocking`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_nonblocking
[`TcpStream::set_only_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_only_v6
[`TcpStream::set_ttl`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_ttl
[`TcpStream::take_error`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.take_error
[`TcpStream::ttl`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.ttl
[`UdpSocket::broadcast`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.broadcast
[`UdpSocket::connect`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.connect
[`UdpSocket::join_multicast_v4`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.join_multicast_v4
[`UdpSocket::join_multicast_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.join_multicast_v6
[`UdpSocket::leave_multicast_v4`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.leave_multicast_v4
[`UdpSocket::leave_multicast_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.leave_multicast_v6
[`UdpSocket::multicast_loop_v4`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.multicast_loop_v4
[`UdpSocket::multicast_loop_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.multicast_loop_v6
[`UdpSocket::multicast_ttl_v4`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.multicast_ttl_v4
[`UdpSocket::multicast_ttl_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.multicast_ttl_v6
[`UdpSocket::only_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.only_v6
[`UdpSocket::recv`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.recv
[`UdpSocket::send`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.send
[`UdpSocket::set_broadcast`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_broadcast
[`UdpSocket::set_multicast_loop_v4`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_multicast_loop_v4
[`UdpSocket::set_multicast_loop_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_multicast_loop_v6
[`UdpSocket::set_multicast_ttl_v4`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_multicast_ttl_v4
[`UdpSocket::set_multicast_ttl_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_multicast_ttl_v6
[`UdpSocket::set_nonblocking`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_nonblocking
[`UdpSocket::set_only_v6`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_only_v6
[`UdpSocket::set_ttl`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.set_ttl
[`UdpSocket::take_error`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.take_error
[`UdpSocket::ttl`]: http://doc.rust-lang.org/nightly/std/net/struct.UdpSocket.html#method.ttl
[`char::DecodeUtf16Error::unpaired_surrogate`]: http://doc.rust-lang.org/nightly/std/char/struct.DecodeUtf16Error.html#method.unpaired_surrogate
[`char::DecodeUtf16Error`]: http://doc.rust-lang.org/nightly/std/char/struct.DecodeUtf16Error.html
[`char::DecodeUtf16`]: http://doc.rust-lang.org/nightly/std/char/struct.DecodeUtf16.html
[`char::decode_utf16`]: http://doc.rust-lang.org/nightly/std/char/fn.decode_utf16.html
[`ptr::read_volatile`]: http://doc.rust-lang.org/nightly/std/ptr/fn.read_volatile.html
[`ptr::write_volatile`]: http://doc.rust-lang.org/nightly/std/ptr/fn.write_volatile.html
[`std::os::unix::thread`]: http://doc.rust-lang.org/nightly/std/os/unix/thread/index.html
[`std::panic::AssertUnwindSafe`]: http://doc.rust-lang.org/nightly/std/panic/struct.AssertUnwindSafe.html
[`std::panic::UnwindSafe`]: http://doc.rust-lang.org/nightly/std/panic/trait.UnwindSafe.html
[`std::panic::catch_unwind`]: http://doc.rust-lang.org/nightly/std/panic/fn.catch_unwind.html
[`std::panic::resume_unwind`]: http://doc.rust-lang.org/nightly/std/panic/fn.resume_unwind.html
[`std::panic`]: http://doc.rust-lang.org/nightly/std/panic/index.html
[`str::is_char_boundary`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.is_char_boundary


Version 1.8.0 (2016-04-14)
==========================

Language
--------

* Rust supports overloading of compound assignment statements like
  `+=` by implementing the [`AddAssign`], [`SubAssign`],
  [`MulAssign`], [`DivAssign`], [`RemAssign`], [`BitAndAssign`],
  [`BitOrAssign`], [`BitXorAssign`], [`ShlAssign`], or [`ShrAssign`]
  traits. [RFC 953].
* Empty structs can be defined with braces, as in `struct Foo { }`, in
  addition to the non-braced form, `struct Foo;`. [RFC 218].

Libraries
---------

* Stabilized APIs:
  * [`str::encode_utf16`][] (renamed from `utf16_units`)
  * [`str::EncodeUtf16`][] (renamed from `Utf16Units`)
  * [`Ref::map`]
  * [`RefMut::map`]
  * [`ptr::drop_in_place`]
  * [`time::Instant`]
  * [`time::SystemTime`]
  * [`Instant::now`]
  * [`Instant::duration_since`][] (renamed from `duration_from_earlier`)
  * [`Instant::elapsed`]
  * [`SystemTime::now`]
  * [`SystemTime::duration_since`][] (renamed from `duration_from_earlier`)
  * [`SystemTime::elapsed`]
  * Various `Add`/`Sub` impls for `Time` and `SystemTime`
  * [`SystemTimeError`]
  * [`SystemTimeError::duration`]
  * Various impls for `SystemTimeError`
  * [`UNIX_EPOCH`]
  * [`AddAssign`], [`SubAssign`], [`MulAssign`], [`DivAssign`],
    [`RemAssign`], [`BitAndAssign`], [`BitOrAssign`],
    [`BitXorAssign`], [`ShlAssign`], [`ShrAssign`].
* [The `write!` and `writeln!` macros correctly emit errors if any of
  their arguments can't be formatted][1.8w].
* [Various I/O functions support large files on 32-bit Linux][1.8l].
* [The Unix-specific `raw` modules, which contain a number of
  redefined C types are deprecated][1.8r], including `os::raw::unix`,
  `os::raw::macos`, and `os::raw::linux`. These modules defined types
  such as `ino_t` and `dev_t`. The inconsistency of these definitions
  across platforms was making it difficult to implement `std`
  correctly. Those that need these definitions should use the `libc`
  crate. [RFC 1415].
* The Unix-specific `MetadataExt` traits, including
  `os::unix::fs::MetadataExt`, which expose values such as inode
  numbers [no longer return platform-specific types][1.8r], but
  instead return widened integers. [RFC 1415].
* [`btree_set::{IntoIter, Iter, Range}` are covariant][1.8cv].
* [Atomic loads and stores are not volatile][1.8a].
* [All types in `sync::mpsc` implement `fmt::Debug`][1.8mp].

Performance
-----------

* [Inlining hash functions lead to a 3% compile-time improvement in
  some workloads][1.8h].
* When using jemalloc, its symbols are [unprefixed so that it
  overrides the libc malloc implementation][1.8h]. This means that for
  rustc, LLVM is now using jemalloc, which results in a 6%
  compile-time improvement on a specific workload.
* [Avoid quadratic growth in function size due to cleanups][1.8cu].

Misc
----

* [32-bit MSVC builds finally implement unwinding][1.8ms].
  i686-pc-windows-msvc is now considered a tier-1 platform.
* [The `--print targets` flag prints a list of supported targets][1.8t].
* [The `--print cfg` flag prints the `cfg`s defined for the current
  target][1.8cf].
* [`rustc` can be built with an new Cargo-based build system, written
  in Rust][1.8b].  It will eventually replace Rust's Makefile-based
  build system. To enable it configure with `configure --rustbuild`.
* [Errors for non-exhaustive `match` patterns now list up to 3 missing
  variants while also indicating the total number of missing variants
  if more than 3][1.8m].
* [Executable stacks are disabled on Linux and BSD][1.8nx].
* The Rust Project now publishes binary releases of the standard
  library for a number of tier-2 targets:
  `armv7-unknown-linux-gnueabihf`, `powerpc-unknown-linux-gnu`,
  `powerpc64-unknown-linux-gnu`, `powerpc64le-unknown-linux-gnu`
  `x86_64-rumprun-netbsd`. These can be installed with
  tools such as [multirust][1.8mr].

Cargo
-----

* [`cargo init` creates a new Cargo project in the current
  directory][1.8ci].  It is otherwise like `cargo new`.
* [Cargo has configuration keys for `-v` and
  `--color`][1.8cc]. `verbose` and `color`, respectively, go in the
  `[term]` section of `.cargo/config`.
* [Configuration keys that evaluate to strings or integers can be set
  via environment variables][1.8ce]. For example the `build.jobs` key
  can be set via `CARGO_BUILD_JOBS`. Environment variables take
  precedence over config files.
* [Target-specific dependencies support Rust `cfg` syntax for
  describing targets][1.8cfg] so that dependencies for multiple
  targets can be specified together. [RFC 1361].
* [The environment variables `CARGO_TARGET_ROOT`, `RUSTC`, and
  `RUSTDOC` take precedence over the `build.target-dir`,
  `build.rustc`, and `build.rustdoc` configuration values][1.8cv].
* [The child process tree is killed on Windows when Cargo is
  killed][1.8ck].
* [The `build.target` configuration value sets the target platform,
  like `--target`][1.8ct].

Compatibility Notes
-------------------

* [Unstable compiler flags have been further restricted][1.8u]. Since
  1.0 `-Z` flags have been considered unstable, and other flags that
  were considered unstable additionally required passing `-Z
  unstable-options` to access. Unlike unstable language and library
  features though, these options have been accessible on the stable
  release channel. Going forward, *new unstable flags will not be
  available on the stable release channel*, and old unstable flags
  will warn about their usage. In the future, all unstable flags will
  be unavailable on the stable release channel.
* [It is no longer possible to `match` on empty enum variants using
  the `Variant(..)` syntax][1.8v]. This has been a warning since 1.6.
* The Unix-specific `MetadataExt` traits, including
  `os::unix::fs::MetadataExt`, which expose values such as inode
  numbers [no longer return platform-specific types][1.8r], but
  instead return widened integers. [RFC 1415].
* [Modules sourced from the filesystem cannot appear within arbitrary
  blocks, but only within other modules][1.8mf].
* [`--cfg` compiler flags are parsed strictly as identifiers][1.8c].
* On Unix, [stack overflow triggers a runtime abort instead of a
  SIGSEGV][1.8so].
* [`Command::spawn` and its equivalents return an error if any of
  its command-line arguments contain interior `NUL`s][1.8n].
* [Tuple and unit enum variants from other crates are in the type
  namespace][1.8tn].
* [On Windows `rustc` emits `.lib` files for the `staticlib` library
  type instead of `.a` files][1.8st]. Additionally, for the MSVC
  toolchain, `rustc` emits import libraries named `foo.dll.lib`
  instead of `foo.lib`.


[1.8a]: https://github.com/rust-lang/rust/pull/30962
[1.8b]: https://github.com/rust-lang/rust/pull/31123
[1.8c]: https://github.com/rust-lang/rust/pull/31530
[1.8cc]: https://github.com/rust-lang/cargo/pull/2397
[1.8ce]: https://github.com/rust-lang/cargo/pull/2398
[1.8cf]: https://github.com/rust-lang/rust/pull/31278
[1.8cfg]: https://github.com/rust-lang/cargo/pull/2328
[1.8ci]: https://github.com/rust-lang/cargo/pull/2081
[1.8ck]: https://github.com/rust-lang/cargo/pull/2370
[1.8ct]: https://github.com/rust-lang/cargo/pull/2335
[1.8cu]: https://github.com/rust-lang/rust/pull/31390
[1.8cv]: https://github.com/rust-lang/cargo/issues/2365
[1.8cv]: https://github.com/rust-lang/rust/pull/30998
[1.8h]: https://github.com/rust-lang/rust/pull/31460
[1.8l]: https://github.com/rust-lang/rust/pull/31668
[1.8m]: https://github.com/rust-lang/rust/pull/31020
[1.8mf]: https://github.com/rust-lang/rust/pull/31534
[1.8mp]: https://github.com/rust-lang/rust/pull/30894
[1.8mr]: https://users.rust-lang.org/t/multirust-0-8-with-cross-std-installation/4901
[1.8ms]: https://github.com/rust-lang/rust/pull/30448
[1.8n]: https://github.com/rust-lang/rust/pull/31056
[1.8nx]: https://github.com/rust-lang/rust/pull/30859
[1.8r]: https://github.com/rust-lang/rust/pull/31551
[1.8so]: https://github.com/rust-lang/rust/pull/31333
[1.8st]: https://github.com/rust-lang/rust/pull/29520
[1.8t]: https://github.com/rust-lang/rust/pull/31358
[1.8tn]: https://github.com/rust-lang/rust/pull/30882
[1.8u]: https://github.com/rust-lang/rust/pull/31793
[1.8v]: https://github.com/rust-lang/rust/pull/31757
[1.8w]: https://github.com/rust-lang/rust/pull/31904
[RFC 1361]: https://github.com/rust-lang/rfcs/blob/master/text/1361-cargo-cfg-dependencies.md
[RFC 1415]: https://github.com/rust-lang/rfcs/blob/master/text/1415-trim-std-os.md
[RFC 218]: https://github.com/rust-lang/rfcs/blob/master/text/0218-empty-struct-with-braces.md
[RFC 953]: https://github.com/rust-lang/rfcs/blob/master/text/0953-op-assign.md
[`AddAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.AddAssign.html
[`BitAndAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.BitAndAssign.html
[`BitOrAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.BitOrAssign.html
[`BitXorAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.BitXorAssign.html
[`DivAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.DivAssign.html
[`Instant::duration_since`]: http://doc.rust-lang.org/nightly/std/time/struct.Instant.html#method.duration_since
[`Instant::elapsed`]: http://doc.rust-lang.org/nightly/std/time/struct.Instant.html#method.elapsed
[`Instant::now`]: http://doc.rust-lang.org/nightly/std/time/struct.Instant.html#method.now
[`MulAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.MulAssign.html
[`Ref::map`]: http://doc.rust-lang.org/nightly/std/cell/struct.Ref.html#method.map
[`RefMut::map`]: http://doc.rust-lang.org/nightly/std/cell/struct.RefMut.html#method.map
[`RemAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.RemAssign.html
[`ShlAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.ShlAssign.html
[`ShrAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.ShrAssign.html
[`SubAssign`]: http://doc.rust-lang.org/nightly/std/ops/trait.SubAssign.html
[`SystemTime::duration_since`]: http://doc.rust-lang.org/nightly/std/time/struct.SystemTime.html#method.duration_since
[`SystemTime::elapsed`]: http://doc.rust-lang.org/nightly/std/time/struct.SystemTime.html#method.elapsed
[`SystemTime::now`]: http://doc.rust-lang.org/nightly/std/time/struct.SystemTime.html#method.now
[`SystemTimeError::duration`]: http://doc.rust-lang.org/nightly/std/time/struct.SystemTimeError.html#method.duration
[`SystemTimeError`]: http://doc.rust-lang.org/nightly/std/time/struct.SystemTimeError.html
[`UNIX_EPOCH`]: http://doc.rust-lang.org/nightly/std/time/constant.UNIX_EPOCH.html
[`ptr::drop_in_place`]: http://doc.rust-lang.org/nightly/std/ptr/fn.drop_in_place.html
[`str::EncodeUtf16`]: http://doc.rust-lang.org/nightly/std/str/struct.EncodeUtf16.html
[`str::encode_utf16`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.encode_utf16
[`time::Instant`]: http://doc.rust-lang.org/nightly/std/time/struct.Instant.html
[`time::SystemTime`]: http://doc.rust-lang.org/nightly/std/time/struct.SystemTime.html


Version 1.7.0 (2016-03-03)
==========================

Libraries
---------

* Stabilized APIs
  * `Path`
    * [`Path::strip_prefix`][] (renamed from relative_from)
    * [`path::StripPrefixError`][] (new error type returned from strip_prefix)
  * `Ipv4Addr`
    * [`Ipv4Addr::is_loopback`]
    * [`Ipv4Addr::is_private`]
    * [`Ipv4Addr::is_link_local`]
    * [`Ipv4Addr::is_multicast`]
    * [`Ipv4Addr::is_broadcast`]
    * [`Ipv4Addr::is_documentation`]
  * `Ipv6Addr`
    * [`Ipv6Addr::is_unspecified`]
    * [`Ipv6Addr::is_loopback`]
    * [`Ipv6Addr::is_multicast`]
  * `Vec`
    * [`Vec::as_slice`]
    * [`Vec::as_mut_slice`]
  * `String`
    * [`String::as_str`]
    * [`String::as_mut_str`]
  * Slices
    * `<[T]>::`[`clone_from_slice`], which now requires the two slices to
    be the same length
    * `<[T]>::`[`sort_by_key`]
  * checked, saturated, and overflowing operations
    * [`i32::checked_rem`], [`i32::checked_neg`], [`i32::checked_shl`], [`i32::checked_shr`]
    * [`i32::saturating_mul`]
    * [`i32::overflowing_add`], [`i32::overflowing_sub`], [`i32::overflowing_mul`], [`i32::overflowing_div`]
    * [`i32::overflowing_rem`], [`i32::overflowing_neg`], [`i32::overflowing_shl`], [`i32::overflowing_shr`]
    * [`u32::checked_rem`], [`u32::checked_neg`], [`u32::checked_shl`], [`u32::checked_shl`]
    * [`u32::saturating_mul`]
    * [`u32::overflowing_add`], [`u32::overflowing_sub`], [`u32::overflowing_mul`], [`u32::overflowing_div`]
    * [`u32::overflowing_rem`], [`u32::overflowing_neg`], [`u32::overflowing_shl`], [`u32::overflowing_shr`]
    * and checked, saturated, and overflowing operations for other primitive types
  * FFI
    * [`ffi::IntoStringError`]
    * [`CString::into_string`]
    * [`CString::into_bytes`]
    * [`CString::into_bytes_with_nul`]
    * `From<CString> for Vec<u8>`
  * `IntoStringError`
    * [`IntoStringError::into_cstring`]
    * [`IntoStringError::utf8_error`]
    * `Error for IntoStringError`
  * Hashing
    * [`std::hash::BuildHasher`]
    * [`BuildHasher::Hasher`]
    * [`BuildHasher::build_hasher`]
    * [`std::hash::BuildHasherDefault`]
    * [`HashMap::with_hasher`]
    * [`HashMap::with_capacity_and_hasher`]
    * [`HashSet::with_hasher`]
    * [`HashSet::with_capacity_and_hasher`]
    * [`std::collections::hash_map::RandomState`]
    * [`RandomState::new`]
* [Validating UTF-8 is faster by a factor of between 7 and 14x for
  ASCII input][1.7utf8]. This means that creating `String`s and `str`s
  from bytes is faster.
* [The performance of `LineWriter` (and thus `io::stdout`) was
  improved by using `memchr` to search for newlines][1.7m].
* [`f32::to_degrees` and `f32::to_radians` are stable][1.7f]. The
  `f64` variants were stabilized previously.
* [`BTreeMap` was rewritten to use less memory and improve the performance
  of insertion and iteration, the latter by as much as 5x][1.7bm].
* [`BTreeSet` and its iterators, `Iter`, `IntoIter`, and `Range` are
  covariant over their contained type][1.7bt].
* [`LinkedList` and its iterators, `Iter` and `IntoIter` are covariant
  over their contained type][1.7ll].
* [`str::replace` now accepts a `Pattern`][1.7rp], like other string
  searching methods.
* [`Any` is implemented for unsized types][1.7a].
* [`Hash` is implemented for `Duration`][1.7h].

Misc
----

* [When running tests with `--test`, rustdoc will pass `--cfg`
  arguments to the compiler][1.7dt].
* [The compiler is built with RPATH information by default][1.7rpa].
  This means that it will be possible to run `rustc` when installed in
  unusual configurations without configuring the dynamic linker search
  path explicitly.
* [`rustc` passes `--enable-new-dtags` to GNU ld][1.7dta]. This makes
  any RPATH entries (emitted with `-C rpath`) *not* take precedence
  over `LD_LIBRARY_PATH`.

Cargo
-----

* [`cargo rustc` accepts a `--profile` flag that runs `rustc` under
  any of the compilation profiles, 'dev', 'bench', or 'test'][1.7cp].
* [The `rerun-if-changed` build script directive no longer causes the
  build script to incorrectly run twice in certain scenarios][1.7rr].

Compatibility Notes
-------------------

* Soundness fixes to the interactions between associated types and
  lifetimes, specified in [RFC 1214], [now generate errors][1.7sf] for
  code that violates the new rules. This is a significant change that
  is known to break existing code, so it has emitted warnings for the
  new error cases since 1.4 to give crate authors time to adapt. The
  details of what is changing are subtle; read the RFC for more.
* [Several bugs in the compiler's visibility calculations were
  fixed][1.7v]. Since this was found to break significant amounts of
  code, the new errors will be emitted as warnings for several release
  cycles, under the `private_in_public` lint.
* Defaulted type parameters were accidentally accepted in positions
  that were not intended. In this release, [defaulted type parameters
  appearing outside of type definitions will generate a
  warning][1.7d], which will become an error in future releases.
* [Parsing "." as a float results in an error instead of 0][1.7p].
  That is, `".".parse::<f32>()` returns `Err`, not `Ok(0.0)`.
* [Borrows of closure parameters may not outlive the closure][1.7bc].

[1.7a]: https://github.com/rust-lang/rust/pull/30928
[1.7bc]: https://github.com/rust-lang/rust/pull/30341
[1.7bm]: https://github.com/rust-lang/rust/pull/30426
[1.7bt]: https://github.com/rust-lang/rust/pull/30998
[1.7cp]: https://github.com/rust-lang/cargo/pull/2224
[1.7d]: https://github.com/rust-lang/rust/pull/30724
[1.7dt]: https://github.com/rust-lang/rust/pull/30372
[1.7dta]: https://github.com/rust-lang/rust/pull/30394
[1.7f]: https://github.com/rust-lang/rust/pull/30672
[1.7h]: https://github.com/rust-lang/rust/pull/30818
[1.7ll]: https://github.com/rust-lang/rust/pull/30663
[1.7m]: https://github.com/rust-lang/rust/pull/30381
[1.7p]: https://github.com/rust-lang/rust/pull/30681
[1.7rp]: https://github.com/rust-lang/rust/pull/29498
[1.7rpa]: https://github.com/rust-lang/rust/pull/30353
[1.7rr]: https://github.com/rust-lang/cargo/pull/2279
[1.7sf]: https://github.com/rust-lang/rust/pull/30389
[1.7utf8]: https://github.com/rust-lang/rust/pull/30740
[1.7v]: https://github.com/rust-lang/rust/pull/29973
[RFC 1214]: https://github.com/rust-lang/rfcs/blob/master/text/1214-projections-lifetimes-and-wf.md
[`BuildHasher::Hasher`]: http://doc.rust-lang.org/nightly/std/hash/trait.Hasher.html
[`BuildHasher::build_hasher`]: http://doc.rust-lang.org/nightly/std/hash/trait.BuildHasher.html#tymethod.build_hasher
[`CString::into_bytes_with_nul`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CString.html#method.into_bytes_with_nul
[`CString::into_bytes`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CString.html#method.into_bytes
[`CString::into_string`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CString.html#method.into_string
[`HashMap::with_capacity_and_hasher`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashMap.html#method.with_capacity_and_hasher
[`HashMap::with_hasher`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashMap.html#method.with_hasher
[`HashSet::with_capacity_and_hasher`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashSet.html#method.with_capacity_and_hasher
[`HashSet::with_hasher`]: http://doc.rust-lang.org/nightly/std/collections/struct.HashSet.html#method.with_hasher
[`IntoStringError::into_cstring`]: http://doc.rust-lang.org/nightly/std/ffi/struct.IntoStringError.html#method.into_cstring
[`IntoStringError::utf8_error`]: http://doc.rust-lang.org/nightly/std/ffi/struct.IntoStringError.html#method.utf8_error
[`Ipv4Addr::is_broadcast`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv4Addr.html#method.is_broadcast
[`Ipv4Addr::is_documentation`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv4Addr.html#method.is_documentation
[`Ipv4Addr::is_link_local`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv4Addr.html#method.is_link_local
[`Ipv4Addr::is_loopback`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv4Addr.html#method.is_loopback
[`Ipv4Addr::is_multicast`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv4Addr.html#method.is_multicast
[`Ipv4Addr::is_private`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv4Addr.html#method.is_private
[`Ipv6Addr::is_loopback`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv6Addr.html#method.is_loopback
[`Ipv6Addr::is_multicast`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv6Addr.html#method.is_multicast
[`Ipv6Addr::is_unspecified`]: http://doc.rust-lang.org/nightly/std/net/struct.Ipv6Addr.html#method.is_unspecified
[`Path::strip_prefix`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.strip_prefix
[`RandomState::new`]: http://doc.rust-lang.org/nightly/std/collections/hash_map/struct.RandomState.html#method.new
[`String::as_mut_str`]: http://doc.rust-lang.org/nightly/std/string/struct.String.html#method.as_mut_str
[`String::as_str`]: http://doc.rust-lang.org/nightly/std/string/struct.String.html#method.as_str
[`Vec::as_mut_slice`]: http://doc.rust-lang.org/nightly/std/vec/struct.Vec.html#method.as_mut_slice
[`Vec::as_slice`]: http://doc.rust-lang.org/nightly/std/vec/struct.Vec.html#method.as_slice
[`clone_from_slice`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.clone_from_slice
[`ffi::IntoStringError`]: http://doc.rust-lang.org/nightly/std/ffi/struct.IntoStringError.html
[`i32::checked_neg`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.checked_neg
[`i32::checked_rem`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.checked_rem
[`i32::checked_shl`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.checked_shl
[`i32::checked_shr`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.checked_shr
[`i32::overflowing_add`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_add
[`i32::overflowing_div`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_div
[`i32::overflowing_mul`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_mul
[`i32::overflowing_neg`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_neg
[`i32::overflowing_rem`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_rem
[`i32::overflowing_shl`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_shl
[`i32::overflowing_shr`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_shr
[`i32::overflowing_sub`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.overflowing_sub
[`i32::saturating_mul`]: http://doc.rust-lang.org/nightly/std/primitive.i32.html#method.saturating_mul
[`path::StripPrefixError`]: http://doc.rust-lang.org/nightly/std/path/struct.StripPrefixError.html
[`sort_by_key`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.sort_by_key
[`std::collections::hash_map::RandomState`]: http://doc.rust-lang.org/nightly/std/collections/hash_map/struct.RandomState.html
[`std::hash::BuildHasherDefault`]: http://doc.rust-lang.org/nightly/std/hash/struct.BuildHasherDefault.html
[`std::hash::BuildHasher`]: http://doc.rust-lang.org/nightly/std/hash/trait.BuildHasher.html
[`u32::checked_neg`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.checked_neg
[`u32::checked_rem`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.checked_rem
[`u32::checked_neg`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.checked_neg
[`u32::checked_shl`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.checked_shl
[`u32::overflowing_add`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_add
[`u32::overflowing_div`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_div
[`u32::overflowing_mul`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_mul
[`u32::overflowing_neg`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_neg
[`u32::overflowing_rem`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_rem
[`u32::overflowing_shl`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_shl
[`u32::overflowing_shr`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_shr
[`u32::overflowing_sub`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.overflowing_sub
[`u32::saturating_mul`]: http://doc.rust-lang.org/nightly/std/primitive.u32.html#method.saturating_mul


Version 1.6.0 (2016-01-21)
==========================

Language
--------

* The `#![no_std]` attribute causes a crate to not be linked to the
  standard library, but only the [core library][1.6co], as described
  in [RFC 1184]. The core library defines common types and traits but
  has no platform dependencies whatsoever, and is the basis for Rust
  software in environments that cannot support a full port of the
  standard library, such as operating systems. Most of the core
  library is now stable.

Libraries
---------

* Stabilized APIs:
  [`Read::read_exact`],
  [`ErrorKind::UnexpectedEof`][] (renamed from `UnexpectedEOF`),
  [`fs::DirBuilder`], [`fs::DirBuilder::new`],
  [`fs::DirBuilder::recursive`], [`fs::DirBuilder::create`],
  [`os::unix::fs::DirBuilderExt`],
  [`os::unix::fs::DirBuilderExt::mode`], [`vec::Drain`],
  [`vec::Vec::drain`], [`string::Drain`], [`string::String::drain`],
  [`vec_deque::Drain`], [`vec_deque::VecDeque::drain`],
  [`collections::hash_map::Drain`],
  [`collections::hash_map::HashMap::drain`],
  [`collections::hash_set::Drain`],
  [`collections::hash_set::HashSet::drain`],
  [`collections::binary_heap::Drain`],
  [`collections::binary_heap::BinaryHeap::drain`],
  [`Vec::extend_from_slice`][] (renamed from `push_all`),
  [`Mutex::get_mut`], [`Mutex::into_inner`], [`RwLock::get_mut`],
  [`RwLock::into_inner`],
  [`Iterator::min_by_key`][] (renamed from `min_by`),
  [`Iterator::max_by_key`][] (renamed from `max_by`).
* The [core library][1.6co] is stable, as are most of its APIs.
* [The `assert_eq!` macro supports arguments that don't implement
  `Sized`][1.6ae], such as arrays. In this way it behaves more like
  `assert!`.
* Several timer functions that take duration in milliseconds [are
  deprecated in favor of those that take `Duration`][1.6ms]. These
  include `Condvar::wait_timeout_ms`, `thread::sleep_ms`, and
  `thread::park_timeout_ms`.
* The algorithm by which `Vec` reserves additional elements was
  [tweaked to not allocate excessive space][1.6a] while still growing
  exponentially.
* `From` conversions are [implemented from integers to floats][1.6f]
  in cases where the conversion is lossless. Thus they are not
  implemented for 32-bit ints to `f32`, nor for 64-bit ints to `f32`
  or `f64`. They are also not implemented for `isize` and `usize`
  because the implementations would be platform-specific. `From` is
  also implemented from `f32` to `f64`.
* `From<&Path>` and `From<PathBuf>` are implemented for `Cow<Path>`.
* `From<T>` is implemented for `Box<T>`, `Rc<T>` and `Arc<T>`.
* `IntoIterator` is implemented for `&PathBuf` and `&Path`.
* [`BinaryHeap` was refactored][1.6bh] for modest performance
  improvements.
* Sorting slices that are already sorted [is 50% faster in some
  cases][1.6s].

Cargo
-----

* Cargo will look in `$CARGO_HOME/bin` for subcommands [by default][1.6c].
* Cargo build scripts can specify their dependencies by emitting the
  [`rerun-if-changed`][1.6rr] key.
* crates.io will reject publication of crates with dependencies that
  have a wildcard version constraint. Crates with wildcard
  dependencies were seen to cause a variety of problems, as described
  in [RFC 1241]. Since 1.5 publication of such crates has emitted a
  warning.
* `cargo clean` [accepts a `--release` flag][1.6cc] to clean the
  release folder.  A variety of artifacts that Cargo failed to clean
  are now correctly deleted.

Misc
----

* The `unreachable_code` lint [warns when a function call's argument
  diverges][1.6dv].
* The parser indicates [failures that may be caused by
  confusingly-similar Unicode characters][1.6uc]
* Certain macro errors [are reported at definition time][1.6m], not
  expansion.

Compatibility Notes
-------------------

* The compiler no longer makes use of the [`RUST_PATH`][1.6rp]
  environment variable when locating crates. This was a pre-cargo
  feature for integrating with the package manager that was
  accidentally never removed.
* [A number of bugs were fixed in the privacy checker][1.6p] that
  could cause previously-accepted code to break.
* [Modules and unit/tuple structs may not share the same name][1.6ts].
* [Bugs in pattern matching unit structs were fixed][1.6us]. The tuple
  struct pattern syntax (`Foo(..)`) can no longer be used to match
  unit structs. This is a warning now, but will become an error in
  future releases. Patterns that share the same name as a const are
  now an error.
* A bug was fixed that causes [rustc not to apply default type
  parameters][1.6xc] when resolving certain method implementations of
  traits defined in other crates.

[1.6a]: https://github.com/rust-lang/rust/pull/29454
[1.6ae]: https://github.com/rust-lang/rust/pull/29770
[1.6bh]: https://github.com/rust-lang/rust/pull/29811
[1.6c]: https://github.com/rust-lang/cargo/pull/2192
[1.6cc]: https://github.com/rust-lang/cargo/pull/2131
[1.6co]: http://doc.rust-lang.org/core/index.html
[1.6dv]: https://github.com/rust-lang/rust/pull/30000
[1.6f]: https://github.com/rust-lang/rust/pull/29129
[1.6m]: https://github.com/rust-lang/rust/pull/29828
[1.6ms]: https://github.com/rust-lang/rust/pull/29604
[1.6p]: https://github.com/rust-lang/rust/pull/29726
[1.6rp]: https://github.com/rust-lang/rust/pull/30034
[1.6rr]: https://github.com/rust-lang/cargo/pull/2134
[1.6s]: https://github.com/rust-lang/rust/pull/29675
[1.6ts]: https://github.com/rust-lang/rust/issues/21546
[1.6uc]: https://github.com/rust-lang/rust/pull/29837
[1.6us]: https://github.com/rust-lang/rust/pull/29383
[1.6xc]: https://github.com/rust-lang/rust/issues/30123
[RFC 1184]: https://github.com/rust-lang/rfcs/blob/master/text/1184-stabilize-no_std.md
[RFC 1241]: https://github.com/rust-lang/rfcs/blob/master/text/1241-no-wildcard-deps.md
[`ErrorKind::UnexpectedEof`]: http://doc.rust-lang.org/nightly/std/io/enum.ErrorKind.html#variant.UnexpectedEof
[`Iterator::max_by_key`]: http://doc.rust-lang.org/nightly/std/iter/trait.Iterator.html#method.max_by_key
[`Iterator::min_by_key`]: http://doc.rust-lang.org/nightly/std/iter/trait.Iterator.html#method.min_by_key
[`Mutex::get_mut`]: http://doc.rust-lang.org/nightly/std/sync/struct.Mutex.html#method.get_mut
[`Mutex::into_inner`]: http://doc.rust-lang.org/nightly/std/sync/struct.Mutex.html#method.into_inner
[`Read::read_exact`]: http://doc.rust-lang.org/nightly/std/io/trait.Read.html#method.read_exact
[`RwLock::get_mut`]: http://doc.rust-lang.org/nightly/std/sync/struct.RwLock.html#method.get_mut
[`RwLock::into_inner`]: http://doc.rust-lang.org/nightly/std/sync/struct.RwLock.html#method.into_inner
[`Vec::extend_from_slice`]: http://doc.rust-lang.org/nightly/collections/vec/struct.Vec.html#method.extend_from_slice
[`collections::binary_heap::BinaryHeap::drain`]: http://doc.rust-lang.org/nightly/std/collections/binary_heap/struct.BinaryHeap.html#method.drain
[`collections::binary_heap::Drain`]: http://doc.rust-lang.org/nightly/std/collections/binary_heap/struct.Drain.html
[`collections::hash_map::Drain`]: http://doc.rust-lang.org/nightly/std/collections/hash_map/struct.Drain.html
[`collections::hash_map::HashMap::drain`]: http://doc.rust-lang.org/nightly/std/collections/hash_map/struct.HashMap.html#method.drain
[`collections::hash_set::Drain`]: http://doc.rust-lang.org/nightly/std/collections/hash_set/struct.Drain.html
[`collections::hash_set::HashSet::drain`]: http://doc.rust-lang.org/nightly/std/collections/hash_set/struct.HashSet.html#method.drain
[`fs::DirBuilder::create`]: http://doc.rust-lang.org/nightly/std/fs/struct.DirBuilder.html#method.create
[`fs::DirBuilder::new`]: http://doc.rust-lang.org/nightly/std/fs/struct.DirBuilder.html#method.new
[`fs::DirBuilder::recursive`]: http://doc.rust-lang.org/nightly/std/fs/struct.DirBuilder.html#method.recursive
[`fs::DirBuilder`]: http://doc.rust-lang.org/nightly/std/fs/struct.DirBuilder.html
[`os::unix::fs::DirBuilderExt::mode`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.DirBuilderExt.html#tymethod.mode
[`os::unix::fs::DirBuilderExt`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.DirBuilderExt.html
[`string::Drain`]: http://doc.rust-lang.org/nightly/std/string/struct.Drain.html
[`string::String::drain`]: http://doc.rust-lang.org/nightly/std/string/struct.String.html#method.drain
[`vec::Drain`]: http://doc.rust-lang.org/nightly/std/vec/struct.Drain.html
[`vec::Vec::drain`]: http://doc.rust-lang.org/nightly/std/vec/struct.Vec.html#method.drain
[`vec_deque::Drain`]: http://doc.rust-lang.org/nightly/std/collections/vec_deque/struct.Drain.html
[`vec_deque::VecDeque::drain`]: http://doc.rust-lang.org/nightly/std/collections/vec_deque/struct.VecDeque.html#method.drain


Version 1.5.0 (2015-12-10)
==========================

* ~700 changes, numerous bugfixes

Highlights
----------

* Stabilized APIs:
  [`BinaryHeap::from`], [`BinaryHeap::into_sorted_vec`],
  [`BinaryHeap::into_vec`], [`Condvar::wait_timeout`],
  [`FileTypeExt::is_block_device`], [`FileTypeExt::is_char_device`],
  [`FileTypeExt::is_fifo`], [`FileTypeExt::is_socket`],
  [`FileTypeExt`], [`Formatter::alternate`], [`Formatter::fill`],
  [`Formatter::precision`], [`Formatter::sign_aware_zero_pad`],
  [`Formatter::sign_minus`], [`Formatter::sign_plus`],
  [`Formatter::width`], [`Iterator::cmp`], [`Iterator::eq`],
  [`Iterator::ge`], [`Iterator::gt`], [`Iterator::le`],
  [`Iterator::lt`], [`Iterator::ne`], [`Iterator::partial_cmp`],
  [`Path::canonicalize`], [`Path::exists`], [`Path::is_dir`],
  [`Path::is_file`], [`Path::metadata`], [`Path::read_dir`],
  [`Path::read_link`], [`Path::symlink_metadata`],
  [`Utf8Error::valid_up_to`], [`Vec::resize`],
  [`VecDeque::as_mut_slices`], [`VecDeque::as_slices`],
  [`VecDeque::insert`], [`VecDeque::shrink_to_fit`],
  [`VecDeque::swap_remove_back`], [`VecDeque::swap_remove_front`],
  [`slice::split_first_mut`], [`slice::split_first`],
  [`slice::split_last_mut`], [`slice::split_last`],
  [`char::from_u32_unchecked`], [`fs::canonicalize`],
  [`str::MatchIndices`], [`str::RMatchIndices`],
  [`str::match_indices`], [`str::rmatch_indices`],
  [`str::slice_mut_unchecked`], [`string::ParseError`].
* Rust applications hosted on crates.io can be installed locally to
  `~/.cargo/bin` with the [`cargo install`] command. Among other
  things this makes it easier to augment Cargo with new subcommands:
  when a binary named e.g. `cargo-foo` is found in `$PATH` it can be
  invoked as `cargo foo`.
* Crates with wildcard (`*`) dependencies will [emit warnings when
  published][1.5w]. In 1.6 it will no longer be possible to publish
  crates with wildcard dependencies.

Breaking Changes
----------------

* The rules determining when a particular lifetime must outlive
  a particular value (known as '[dropck]') have been [modified
  to not rely on parametricity][1.5p].
* [Implementations of `AsRef` and `AsMut` were added to `Box`, `Rc`,
  and `Arc`][1.5a]. Because these smart pointer types implement
  `Deref`, this causes breakage in cases where the interior type
  contains methods of the same name.
* [Correct a bug in Rc/Arc][1.5c] that caused [dropck] to be unaware
  that they could drop their content. Soundness fix.
* All method invocations are [properly checked][1.5wf1] for
  [well-formedness][1.5wf2]. Soundness fix.
* Traits whose supertraits contain `Self` are [not object
  safe][1.5o]. Soundness fix.
* Target specifications support a [`no_default_libraries`][1.5nd]
  setting that controls whether `-nodefaultlibs` is passed to the
  linker, and in turn the `is_like_windows` setting no longer affects
  the `-nodefaultlibs` flag.
* `#[derive(Show)]`, long-deprecated, [has been removed][1.5ds].
* The `#[inline]` and `#[repr]` attributes [can only appear
  in valid locations][1.5at].
* Native libraries linked from the local crate are [passed to
  the linker before native libraries from upstream crates][1.5nl].
* Two rarely-used attributes, `#[no_debug]` and
  `#[omit_gdb_pretty_printer_section]` [are feature gated][1.5fg].
* Negation of unsigned integers, which has been a warning for
  several releases, [is now behind a feature gate and will
  generate errors][1.5nu].
* The parser accidentally accepted visibility modifiers on
  enum variants, a bug [which has been fixed][1.5ev].
* [A bug was fixed that allowed `use` statements to import unstable
  features][1.5use].

Language
--------

* When evaluating expressions at compile-time that are not
  compile-time constants (const-evaluating expressions in non-const
  contexts), incorrect code such as overlong bitshifts and arithmetic
  overflow will [generate a warning instead of an error][1.5ce],
  delaying the error until runtime. This will allow the
  const-evaluator to be expanded in the future backwards-compatibly.
* The `improper_ctypes` lint [no longer warns about using `isize` and
  `usize` in FFI][1.5ict].

Libraries
---------

* `Arc<T>` and `Rc<T>` are [covariant with respect to `T` instead of
  invariant][1.5c].
* `Default` is [implemented for mutable slices][1.5d].
* `FromStr` is [implemented for `SockAddrV4` and `SockAddrV6`][1.5s].
* There are now `From` conversions [between floating point
  types][1.5f] where the conversions are lossless.
* There are now `From` conversions [between integer types][1.5i] where
  the conversions are lossless.
* [`fs::Metadata` implements `Clone`][1.5fs].
* The `parse` method [accepts a leading "+" when parsing
  integers][1.5pi].
* [`AsMut` is implemented for `Vec`][1.5am].
* The `clone_from` implementations for `String` and `BinaryHeap` [have
  been optimized][1.5cf] and no longer rely on the default impl.
* The `extern "Rust"`, `extern "C"`, `unsafe extern "Rust"` and
  `unsafe extern "C"` function types now [implement `Clone`,
  `PartialEq`, `Eq`, `PartialOrd`, `Ord`, `Hash`, `fmt::Pointer`, and
  `fmt::Debug` for up to 12 arguments][1.5fp].
* [Dropping `Vec`s is much faster in unoptimized builds when the
  element types don't implement `Drop`][1.5dv].
* A bug that caused in incorrect behavior when [combining `VecDeque`
  with zero-sized types][1.5vdz] was resolved.
* [`PartialOrd` for slices is faster][1.5po].

Miscellaneous
-------------

* [Crate metadata size was reduced by 20%][1.5md].
* [Improvements to code generation reduced the size of libcore by 3.3
  MB and rustc's memory usage by 18MB][1.5m].
* [Improvements to deref translation increased performance in
  unoptimized builds][1.5dr].
* Various errors in trait resolution [are deduplicated to only be
  reported once][1.5te].
* Rust has preliminary [support for rumprun kernels][1.5rr].
* Rust has preliminary [support for NetBSD on amd64][1.5na].

[1.5use]: https://github.com/rust-lang/rust/pull/28364
[1.5po]: https://github.com/rust-lang/rust/pull/28436
[1.5ev]: https://github.com/rust-lang/rust/pull/28442
[1.5nu]: https://github.com/rust-lang/rust/pull/28468
[1.5dr]: https://github.com/rust-lang/rust/pull/28491
[1.5vdz]: https://github.com/rust-lang/rust/pull/28494
[1.5md]: https://github.com/rust-lang/rust/pull/28521
[1.5fg]: https://github.com/rust-lang/rust/pull/28522
[1.5dv]: https://github.com/rust-lang/rust/pull/28531
[1.5na]: https://github.com/rust-lang/rust/pull/28543
[1.5fp]: https://github.com/rust-lang/rust/pull/28560
[1.5rr]: https://github.com/rust-lang/rust/pull/28593
[1.5cf]: https://github.com/rust-lang/rust/pull/28602
[1.5nl]: https://github.com/rust-lang/rust/pull/28605
[1.5te]: https://github.com/rust-lang/rust/pull/28645
[1.5at]: https://github.com/rust-lang/rust/pull/28650
[1.5am]: https://github.com/rust-lang/rust/pull/28663
[1.5m]: https://github.com/rust-lang/rust/pull/28778
[1.5ict]: https://github.com/rust-lang/rust/pull/28779
[1.5a]: https://github.com/rust-lang/rust/pull/28811
[1.5pi]: https://github.com/rust-lang/rust/pull/28826
[1.5ce]: https://github.com/rust-lang/rfcs/blob/master/text/1229-compile-time-asserts.md
[1.5p]: https://github.com/rust-lang/rfcs/blob/master/text/1238-nonparametric-dropck.md
[1.5i]: https://github.com/rust-lang/rust/pull/28921
[1.5fs]: https://github.com/rust-lang/rust/pull/29021
[1.5f]: https://github.com/rust-lang/rust/pull/29129
[1.5ds]: https://github.com/rust-lang/rust/pull/29148
[1.5s]: https://github.com/rust-lang/rust/pull/29190
[1.5d]: https://github.com/rust-lang/rust/pull/29245
[1.5o]: https://github.com/rust-lang/rust/pull/29259
[1.5nd]: https://github.com/rust-lang/rust/pull/28578
[1.5wf2]: https://github.com/rust-lang/rfcs/blob/master/text/1214-projections-lifetimes-and-wf.md
[1.5wf1]: https://github.com/rust-lang/rust/pull/28669
[dropck]: https://doc.rust-lang.org/nightly/nomicon/dropck.html
[1.5c]: https://github.com/rust-lang/rust/pull/29110
[1.5w]: https://github.com/rust-lang/rfcs/blob/master/text/1241-no-wildcard-deps.md
[`cargo install`]: https://github.com/rust-lang/rfcs/blob/master/text/1200-cargo-install.md
[`BinaryHeap::from`]: http://doc.rust-lang.org/nightly/std/convert/trait.From.html#method.from
[`BinaryHeap::into_sorted_vec`]: http://doc.rust-lang.org/nightly/std/collections/struct.BinaryHeap.html#method.into_sorted_vec
[`BinaryHeap::into_vec`]: http://doc.rust-lang.org/nightly/std/collections/struct.BinaryHeap.html#method.into_vec
[`Condvar::wait_timeout`]: http://doc.rust-lang.org/nightly/std/sync/struct.Condvar.html#method.wait_timeout
[`FileTypeExt::is_block_device`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.FileTypeExt.html#tymethod.is_block_device
[`FileTypeExt::is_char_device`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.FileTypeExt.html#tymethod.is_char_device
[`FileTypeExt::is_fifo`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.FileTypeExt.html#tymethod.is_fifo
[`FileTypeExt::is_socket`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.FileTypeExt.html#tymethod.is_socket
[`FileTypeExt`]: http://doc.rust-lang.org/nightly/std/os/unix/fs/trait.FileTypeExt.html
[`Formatter::alternate`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.alternate
[`Formatter::fill`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.fill
[`Formatter::precision`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.precision
[`Formatter::sign_aware_zero_pad`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.sign_aware_zero_pad
[`Formatter::sign_minus`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.sign_minus
[`Formatter::sign_plus`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.sign_plus
[`Formatter::width`]: http://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.width
[`Iterator::cmp`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.cmp
[`Iterator::eq`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.eq
[`Iterator::ge`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.ge
[`Iterator::gt`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.gt
[`Iterator::le`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.le
[`Iterator::lt`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.lt
[`Iterator::ne`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.ne
[`Iterator::partial_cmp`]: http://doc.rust-lang.org/nightly/core/iter/trait.Iterator.html#method.partial_cmp
[`Path::canonicalize`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.canonicalize
[`Path::exists`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.exists
[`Path::is_dir`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.is_dir
[`Path::is_file`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.is_file
[`Path::metadata`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.metadata
[`Path::read_dir`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.read_dir
[`Path::read_link`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.read_link
[`Path::symlink_metadata`]: http://doc.rust-lang.org/nightly/std/path/struct.Path.html#method.symlink_metadata
[`Utf8Error::valid_up_to`]: http://doc.rust-lang.org/nightly/core/str/struct.Utf8Error.html#method.valid_up_to
[`Vec::resize`]: http://doc.rust-lang.org/nightly/std/vec/struct.Vec.html#method.resize
[`VecDeque::as_mut_slices`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.as_mut_slices
[`VecDeque::as_slices`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.as_slices
[`VecDeque::insert`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.insert
[`VecDeque::shrink_to_fit`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.shrink_to_fit
[`VecDeque::swap_remove_back`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.swap_remove_back
[`VecDeque::swap_remove_front`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.swap_remove_front
[`slice::split_first_mut`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.split_first_mut
[`slice::split_first`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.split_first
[`slice::split_last_mut`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.split_last_mut
[`slice::split_last`]: http://doc.rust-lang.org/nightly/std/primitive.slice.html#method.split_last
[`char::from_u32_unchecked`]: http://doc.rust-lang.org/nightly/std/char/fn.from_u32_unchecked.html
[`fs::canonicalize`]: http://doc.rust-lang.org/nightly/std/fs/fn.canonicalize.html
[`str::MatchIndices`]: http://doc.rust-lang.org/nightly/std/str/struct.MatchIndices.html
[`str::RMatchIndices`]: http://doc.rust-lang.org/nightly/std/str/struct.RMatchIndices.html
[`str::match_indices`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.match_indices
[`str::rmatch_indices`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.rmatch_indices
[`str::slice_mut_unchecked`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.slice_mut_unchecked
[`string::ParseError`]: http://doc.rust-lang.org/nightly/std/string/enum.ParseError.html

Version 1.4.0 (2015-10-29)
==========================

* ~1200 changes, numerous bugfixes

Highlights
----------

* Windows builds targeting the 64-bit MSVC ABI and linker (instead of
  GNU) are now supported and recommended for use.

Breaking Changes
----------------

* [Several changes have been made to fix type soundness and improve
  the behavior of associated types][sound]. See [RFC 1214]. Although
  we have mostly introduced these changes as warnings this release, to
  become errors next release, there are still some scenarios that will
  see immediate breakage.
* [The `str::lines` and `BufRead::lines` iterators treat `\r\n` as
  line breaks in addition to `\n`][crlf].
* [Loans of `'static` lifetime extend to the end of a function][stat].
* [`str::parse` no longer introduces avoidable rounding error when
  parsing floating point numbers. Together with earlier changes to
  float formatting/output, "round trips" like f.to_string().parse()
  now preserve the value of f exactly. Additionally, leading plus
  signs are now accepted][fp3].


Language
--------

* `use` statements that import multiple items [can now rename
  them][i], as in `use foo::{bar as kitten, baz as puppy}`.
* [Binops work correctly on fat pointers][binfat].
* `pub extern crate`, which does not behave as expected, [issues a
  warning][pec] until a better solution is found.

Libraries
---------

* [Many APIs were stabilized][stab]: `<Box<str>>::into_string`,
  [`Arc::downgrade`], [`Arc::get_mut`], [`Arc::make_mut`],
  [`Arc::try_unwrap`], [`Box::from_raw`], [`Box::into_raw`], [`CStr::to_str`],
  [`CStr::to_string_lossy`], [`CString::from_raw`], [`CString::into_raw`],
  [`IntoRawFd::into_raw_fd`], [`IntoRawFd`],
  `IntoRawHandle::into_raw_handle`, `IntoRawHandle`,
  `IntoRawSocket::into_raw_socket`, `IntoRawSocket`, [`Rc::downgrade`],
  [`Rc::get_mut`], [`Rc::make_mut`], [`Rc::try_unwrap`], [`Result::expect`],
  [`String::into_boxed_str`], [`TcpStream::read_timeout`],
  [`TcpStream::set_read_timeout`], [`TcpStream::set_write_timeout`],
  [`TcpStream::write_timeout`], [`UdpSocket::read_timeout`],
  [`UdpSocket::set_read_timeout`], [`UdpSocket::set_write_timeout`],
  [`UdpSocket::write_timeout`], `Vec::append`, `Vec::split_off`,
  [`VecDeque::append`], [`VecDeque::retain`], [`VecDeque::split_off`],
  [`rc::Weak::upgrade`], [`rc::Weak`], [`slice::Iter::as_slice`],
  [`slice::IterMut::into_slice`], [`str::CharIndices::as_str`],
  [`str::Chars::as_str`], [`str::split_at_mut`], [`str::split_at`],
  [`sync::Weak::upgrade`], [`sync::Weak`], [`thread::park_timeout`],
  [`thread::sleep`].
* [Some APIs were deprecated][dep]: `BTreeMap::with_b`,
  `BTreeSet::with_b`, `Option::as_mut_slice`, `Option::as_slice`,
  `Result::as_mut_slice`, `Result::as_slice`, `f32::from_str_radix`,
  `f64::from_str_radix`.
* [Reverse-searching strings is faster with the 'two-way'
  algorithm][s].
* [`std::io::copy` allows `?Sized` arguments][cc].
* The `Windows`, `Chunks`, and `ChunksMut` iterators over slices all
  [override `count`, `nth` and `last` with an O(1)
  implementation][it].
* [`Default` is implemented for arrays up to `[T; 32]`][d].
* [`IntoRawFd` has been added to the Unix-specific prelude,
  `IntoRawSocket` and `IntoRawHandle` to the Windows-specific
  prelude][pr].
* [`Extend<String>` and `FromIterator<String` are both implemented for
  `String`][es].
* [`IntoIterator` is implemented for references to `Option` and
  `Result`][into2].
* [`HashMap` and `HashSet` implement `Extend<&T>` where `T:
  Copy`][ext] as part of [RFC 839]. This will cause type inference
  breakage in rare situations.
* [`BinaryHeap` implements `Debug`][bh2].
* [`Borrow` and `BorrowMut` are implemented for fixed-size
  arrays][bm].
* [`extern fn`s with the "Rust" and "C" ABIs implement common
  traits including `Eq`, `Ord`, `Debug`, `Hash`][fp].
* [String comparison is faster][faststr].
* `&mut T` where `T: std::fmt::Write` [also implements
  `std::fmt::Write`][mutw].
* [A stable regression in `VecDeque::push_back` and other
  capacity-altering methods that caused panics for zero-sized types
  was fixed][vd].
* [Function pointers implement traits for up to 12 parameters][fp2].

Miscellaneous
-------------

* The compiler [no longer uses the 'morestack' feature to prevent
  stack overflow][mm]. Instead it uses guard pages and stack
  probes (though stack probes are not yet implemented on any platform
  but Windows).
* [The compiler matches traits faster when projections are involved][p].
* The 'improper_ctypes' lint [no longer warns about use of `isize` and
  `usize`][ffi].
* [Cargo now displays useful information about what its doing during
  `cargo update`][cu].

[`Arc::downgrade`]: http://doc.rust-lang.org/nightly/alloc/arc/struct.Arc.html#method.downgrade
[`Arc::make_mut`]: http://doc.rust-lang.org/nightly/alloc/arc/struct.Arc.html#method.make_mut
[`Arc::get_mut`]: http://doc.rust-lang.org/nightly/alloc/arc/struct.Arc.html#method.get_mut
[`Arc::try_unwrap`]: http://doc.rust-lang.org/nightly/alloc/arc/struct.Arc.html#method.try_unwrap
[`Box::from_raw`]: http://doc.rust-lang.org/nightly/alloc/boxed/struct.Box.html#method.from_raw
[`Box::into_raw`]: http://doc.rust-lang.org/nightly/alloc/boxed/struct.Box.html#method.into_raw
[`CStr::to_str`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CStr.html#method.to_str
[`CStr::to_string_lossy`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CStr.html#method.to_string_lossy
[`CString::from_raw`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CString.html#method.from_raw
[`CString::into_raw`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CString.html#method.into_raw
[`IntoRawFd::into_raw_fd`]: http://doc.rust-lang.org/nightly/std/os/unix/io/trait.IntoRawFd.html#tymethod.into_raw_fd
[`IntoRawFd`]: http://doc.rust-lang.org/nightly/std/os/unix/io/trait.IntoRawFd.html
[`Rc::downgrade`]: http://doc.rust-lang.org/nightly/alloc/rc/struct.Rc.html#method.downgrade
[`Rc::get_mut`]: http://doc.rust-lang.org/nightly/alloc/rc/struct.Rc.html#method.get_mut
[`Rc::make_mut`]: http://doc.rust-lang.org/nightly/alloc/rc/struct.Rc.html#method.make_mut
[`Rc::try_unwrap`]: http://doc.rust-lang.org/nightly/alloc/rc/struct.Rc.html#method.try_unwrap
[`Result::expect`]: http://doc.rust-lang.org/nightly/core/result/enum.Result.html#method.expect
[`String::into_boxed_str`]: http://doc.rust-lang.org/nightly/collections/string/struct.String.html#method.into_boxed_str
[`TcpStream::read_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.read_timeout
[`TcpStream::set_read_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_read_timeout
[`TcpStream::write_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.write_timeout
[`TcpStream::set_write_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_write_timeout
[`UdpSocket::read_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.read_timeout
[`UdpSocket::set_read_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_read_timeout
[`UdpSocket::write_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.write_timeout
[`UdpSocket::set_write_timeout`]: http://doc.rust-lang.org/nightly/std/net/struct.TcpStream.html#method.set_write_timeout
[`VecDeque::append`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.append
[`VecDeque::retain`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.retain
[`VecDeque::split_off`]: http://doc.rust-lang.org/nightly/std/collections/struct.VecDeque.html#method.split_off
[`rc::Weak::upgrade`]: http://doc.rust-lang.org/nightly/std/rc/struct.Weak.html#method.upgrade
[`rc::Weak`]: http://doc.rust-lang.org/nightly/std/rc/struct.Weak.html
[`slice::Iter::as_slice`]: http://doc.rust-lang.org/nightly/std/slice/struct.Iter.html#method.as_slice
[`slice::IterMut::into_slice`]: http://doc.rust-lang.org/nightly/std/slice/struct.IterMut.html#method.into_slice
[`str::CharIndices::as_str`]: http://doc.rust-lang.org/nightly/std/str/struct.CharIndices.html#method.as_str
[`str::Chars::as_str`]: http://doc.rust-lang.org/nightly/std/str/struct.Chars.html#method.as_str
[`str::split_at_mut`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.split_at_mut
[`str::split_at`]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.split_at
[`sync::Weak::upgrade`]: http://doc.rust-lang.org/nightly/std/sync/struct.Weak.html#method.upgrade
[`sync::Weak`]: http://doc.rust-lang.org/nightly/std/sync/struct.Weak.html
[`thread::park_timeout`]: http://doc.rust-lang.org/nightly/std/thread/fn.park_timeout.html
[`thread::sleep`]: http://doc.rust-lang.org/nightly/std/thread/fn.sleep.html
[bh2]: https://github.com/rust-lang/rust/pull/28156
[binfat]: https://github.com/rust-lang/rust/pull/28270
[bm]: https://github.com/rust-lang/rust/pull/28197
[cc]: https://github.com/rust-lang/rust/pull/27531
[crlf]: https://github.com/rust-lang/rust/pull/28034
[cu]: https://github.com/rust-lang/cargo/pull/1931
[d]: https://github.com/rust-lang/rust/pull/27825
[dep]: https://github.com/rust-lang/rust/pull/28339
[es]: https://github.com/rust-lang/rust/pull/27956
[ext]: https://github.com/rust-lang/rust/pull/28094
[faststr]: https://github.com/rust-lang/rust/pull/28338
[ffi]: https://github.com/rust-lang/rust/pull/28779
[fp]: https://github.com/rust-lang/rust/pull/28268
[fp2]: https://github.com/rust-lang/rust/pull/28560
[fp3]: https://github.com/rust-lang/rust/pull/27307
[i]: https://github.com/rust-lang/rust/pull/27451
[into2]: https://github.com/rust-lang/rust/pull/28039
[it]: https://github.com/rust-lang/rust/pull/27652
[mm]: https://github.com/rust-lang/rust/pull/27338
[mutw]: https://github.com/rust-lang/rust/pull/28368
[sound]: https://github.com/rust-lang/rust/pull/27641
[p]: https://github.com/rust-lang/rust/pull/27866
[pec]: https://github.com/rust-lang/rust/pull/28486
[pr]: https://github.com/rust-lang/rust/pull/27896
[RFC 839]: https://github.com/rust-lang/rfcs/blob/master/text/0839-embrace-extend-extinguish.md
[RFC 1214]: https://github.com/rust-lang/rfcs/blob/master/text/1214-projections-lifetimes-and-wf.md
[s]: https://github.com/rust-lang/rust/pull/27474
[stab]: https://github.com/rust-lang/rust/pull/28339
[stat]: https://github.com/rust-lang/rust/pull/28321
[vd]: https://github.com/rust-lang/rust/pull/28494

Version 1.3.0 (2015-09-17)
==============================

* ~900 changes, numerous bugfixes

Highlights
----------

* The [new object lifetime defaults][nold] have been [turned
  on][nold2] after a cycle of warnings about the change. Now types
  like `&'a Box<Trait>` (or `&'a Rc<Trait>`, etc) will change from
  being interpreted as `&'a Box<Trait+'a>` to `&'a
  Box<Trait+'static>`.
* [The Rustonomicon][nom] is a new book in the official documentation
  that dives into writing unsafe Rust.
* The [`Duration`] API, [has been stabilized][ds]. This basic unit of
  timekeeping is employed by other std APIs, as well as out-of-tree
  time crates.

Breaking Changes
----------------

* The [new object lifetime defaults][nold] have been [turned
  on][nold2] after a cycle of warnings about the change.
* There is a known [regression][lr] in how object lifetime elision is
  interpreted, the proper solution for which is undetermined.
* The `#[prelude_import]` attribute, an internal implementation
  detail, was accidentally stabilized previously. [It has been put
  behind the `prelude_import` feature gate][pi]. This change is
  believed to break no existing code.
* The behavior of [`size_of_val`][dst1] and [`align_of_val`][dst2] is
  [more sane for dynamically sized types][dst3]. Code that relied on
  the previous behavior is thought to be broken.
* The `dropck` rules, which checks that destructors can't access
  destroyed values, [have been updated][dropck] to match the
  [RFC][dropckrfc]. This fixes some soundness holes, and as such will
  cause some previously-compiling code to no longer build.

Language
--------

* The [new object lifetime defaults][nold] have been [turned
  on][nold2] after a cycle of warnings about the change.
* Semicolons may [now follow types and paths in
  macros](https://github.com/rust-lang/rust/pull/27000).
* The behavior of [`size_of_val`][dst1] and [`align_of_val`][dst2] is
  [more sane for dynamically sized types][dst3]. Code that relied on
  the previous behavior is not known to exist, and suspected to be
  broken.
* `'static` variables [may now be recursive][st].
* `ref` bindings choose between [`Deref`] and [`DerefMut`]
  implementations correctly.
* The `dropck` rules, which checks that destructors can't access
  destroyed values, [have been updated][dropck] to match the
  [RFC][dropckrfc].

Libraries
---------

* The [`Duration`] API, [has been stabilized][ds], as well as the
  `std::time` module, which presently contains only `Duration`.
* `Box<str>` and `Box<[T]>` both implement `Clone`.
* The owned C string, [`CString`], implements [`Borrow`] and the
  borrowed C string, [`CStr`], implements [`ToOwned`]. The two of
  these allow C strings to be borrowed and cloned in generic code.
* [`CStr`] implements [`Debug`].
* [`AtomicPtr`] implements [`Debug`].
* [`Error`] trait objects [can be downcast to their concrete types][e]
  in many common configurations, using the [`is`], [`downcast`],
  [`downcast_ref`] and [`downcast_mut`] methods, similarly to the
  [`Any`] trait.
* Searching for substrings now [employs the two-way algorithm][search]
  instead of doing a naive search. This gives major speedups to a
  number of methods, including [`contains`][sc], [`find`][sf],
  [`rfind`][srf], [`split`][ss]. [`starts_with`][ssw] and
  [`ends_with`][sew] are also faster.
* The performance of `PartialEq` for slices is [much faster][ps].
* The [`Hash`] trait offers the default method, [`hash_slice`], which
  is overridden and optimized by the implementations for scalars.
* The [`Hasher`] trait now has a number of specialized `write_*`
  methods for primitive types, for efficiency.
* The I/O-specific error type, [`std::io::Error`][ie], gained a set of
  methods for accessing the 'inner error', if any: [`get_ref`][iegr],
  [`get_mut`][iegm], [`into_inner`][ieii]. As well, the implementation
  of [`std::error::Error::cause`][iec] also delegates to the inner
  error.
* [`process::Child`][pc] gained the [`id`] method, which returns a
  `u32` representing the platform-specific process identifier.
* The [`connect`] method on slices is deprecated, replaced by the new
  [`join`] method (note that both of these are on the *unstable*
  [`SliceConcatExt`] trait, but through the magic of the prelude are
  available to stable code anyway).
* The [`Div`] operator is implemented for [`Wrapping`] types.
* [`DerefMut` is implemented for `String`][dms].
* Performance of SipHash (the default hasher for `HashMap`) is
  [better for long data][sh].
* [`AtomicPtr`] implements [`Send`].
* The [`read_to_end`] implementations for [`Stdin`] and [`File`]
  are now [specialized to use uninitialized buffers for increased
  performance][rte].
* Lifetime parameters of foreign functions [are now resolved
  properly][f].

Misc
----

* Rust can now, with some coercion, [produce programs that run on
  Windows XP][xp], though XP is not considered a supported platform.
* Porting Rust on Windows from the GNU toolchain to MSVC continues
  ([1][win1], [2][win2], [3][win3], [4][win4]). It is still not
  recommended for use in 1.3, though should be fully-functional
  in the [64-bit 1.4 beta][b14].
* On Fedora-based systems installation will [properly configure the
  dynamic linker][fl].
* The compiler gained many new extended error descriptions, which can
  be accessed with the `--explain` flag.
* The `dropck` pass, which checks that destructors can't access
  destroyed values, [has been rewritten][dropck]. This fixes some
  soundness holes, and as such will cause some previously-compiling
  code to no longer build.
* `rustc` now uses [LLVM to write archive files where possible][ar].
  Eventually this will eliminate the compiler's dependency on the ar
  utility.
* Rust has [preliminary support for i686 FreeBSD][fb] (it has long
  supported FreeBSD on x86_64).
* The [`unused_mut`][lum], [`unconditional_recursion`][lur],
  [`improper_ctypes`][lic], and [`negate_unsigned`][lnu] lints are
  more strict.
* If landing pads are disabled (with `-Z no-landing-pads`), [`panic!`
  will kill the process instead of leaking][nlp].

[`Any`]: http://doc.rust-lang.org/nightly/std/any/trait.Any.html
[`AtomicPtr`]: http://doc.rust-lang.org/nightly/std/sync/atomic/struct.AtomicPtr.html
[`Borrow`]: http://doc.rust-lang.org/nightly/std/borrow/trait.Borrow.html
[`CStr`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CStr.html
[`CString`]: http://doc.rust-lang.org/nightly/std/ffi/struct.CString.html
[`Debug`]: http://doc.rust-lang.org/nightly/std/fmt/trait.Debug.html
[`DerefMut`]: http://doc.rust-lang.org/nightly/std/ops/trait.DerefMut.html
[`Deref`]: http://doc.rust-lang.org/nightly/std/ops/trait.Deref.html
[`Div`]: http://doc.rust-lang.org/nightly/std/ops/trait.Div.html
[`Duration`]: http://doc.rust-lang.org/nightly/std/time/struct.Duration.html
[`Error`]: http://doc.rust-lang.org/nightly/std/error/trait.Error.html
[`File`]: http://doc.rust-lang.org/nightly/std/fs/struct.File.html
[`Hash`]: http://doc.rust-lang.org/nightly/std/hash/trait.Hash.html
[`Hasher`]: http://doc.rust-lang.org/nightly/std/hash/trait.Hasher.html
[`Send`]: http://doc.rust-lang.org/nightly/std/marker/trait.Send.html
[`SliceConcatExt`]: http://doc.rust-lang.org/nightly/std/slice/trait.SliceConcatExt.html
[`Stdin`]: http://doc.rust-lang.org/nightly/std/io/struct.Stdin.html
[`ToOwned`]: http://doc.rust-lang.org/nightly/std/borrow/trait.ToOwned.html
[`Wrapping`]: http://doc.rust-lang.org/nightly/std/num/struct.Wrapping.html
[`connect`]: http://doc.rust-lang.org/nightly/std/slice/trait.SliceConcatExt.html#method.connect
[`downcast_mut`]: http://doc.rust-lang.org/nightly/std/error/trait.Error.html#method.downcast_mut
[`downcast_ref`]: http://doc.rust-lang.org/nightly/std/error/trait.Error.html#method.downcast_ref
[`downcast`]: http://doc.rust-lang.org/nightly/std/error/trait.Error.html#method.downcast
[`hash_slice`]: http://doc.rust-lang.org/nightly/std/hash/trait.Hash.html#method.hash_slice
[`id`]: http://doc.rust-lang.org/nightly/std/process/struct.Child.html#method.id
[`is`]: http://doc.rust-lang.org/nightly/std/error/trait.Error.html#method.is
[`join`]: http://doc.rust-lang.org/nightly/std/slice/trait.SliceConcatExt.html#method.join
[`read_to_end`]: http://doc.rust-lang.org/nightly/std/io/trait.Read.html#method.read_to_end
[ar]: https://github.com/rust-lang/rust/pull/26926
[b14]: https://static.rust-lang.org/dist/rust-beta-x86_64-pc-windows-msvc.msi
[dms]: https://github.com/rust-lang/rust/pull/26241
[dropck]: https://github.com/rust-lang/rust/pull/27261
[dropckrfc]: https://github.com/rust-lang/rfcs/blob/master/text/0769-sound-generic-drop.md
[ds]: https://github.com/rust-lang/rust/pull/26818
[dst1]: http://doc.rust-lang.org/nightly/std/mem/fn.size_of_val.html
[dst2]: http://doc.rust-lang.org/nightly/std/mem/fn.align_of_val.html
[dst3]: https://github.com/rust-lang/rust/pull/27351
[e]: https://github.com/rust-lang/rust/pull/24793
[f]: https://github.com/rust-lang/rust/pull/26588
[fb]: https://github.com/rust-lang/rust/pull/26959
[fl]: https://github.com/rust-lang/rust-installer/pull/41
[hs]: http://doc.rust-lang.org/nightly/std/hash/trait.Hash.html#method.hash_slice
[ie]: http://doc.rust-lang.org/nightly/std/io/struct.Error.html
[iec]: http://doc.rust-lang.org/nightly/std/io/struct.Error.html#method.cause
[iegm]: http://doc.rust-lang.org/nightly/std/io/struct.Error.html#method.get_mut
[iegr]: http://doc.rust-lang.org/nightly/std/io/struct.Error.html#method.get_ref
[ieii]: http://doc.rust-lang.org/nightly/std/io/struct.Error.html#method.into_inner
[lic]: https://github.com/rust-lang/rust/pull/26583
[lnu]: https://github.com/rust-lang/rust/pull/27026
[lr]: https://github.com/rust-lang/rust/issues/27248
[lum]: https://github.com/rust-lang/rust/pull/26378
[lur]: https://github.com/rust-lang/rust/pull/26783
[nlp]: https://github.com/rust-lang/rust/pull/27176
[nold2]: https://github.com/rust-lang/rust/pull/27045
[nold]: https://github.com/rust-lang/rfcs/blob/master/text/1156-adjust-default-object-bounds.md
[nom]: http://doc.rust-lang.org/nightly/nomicon/
[pc]: http://doc.rust-lang.org/nightly/std/process/struct.Child.html
[pi]: https://github.com/rust-lang/rust/pull/26699
[ps]: https://github.com/rust-lang/rust/pull/26884
[rte]: https://github.com/rust-lang/rust/pull/26950
[sc]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.contains
[search]: https://github.com/rust-lang/rust/pull/26327
[sew]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.ends_with
[sf]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.find
[sh]: https://github.com/rust-lang/rust/pull/27280
[srf]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.rfind
[ss]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.split
[ssw]: http://doc.rust-lang.org/nightly/std/primitive.str.html#method.starts_with
[st]: https://github.com/rust-lang/rust/pull/26630
[win1]: https://github.com/rust-lang/rust/pull/26569
[win2]: https://github.com/rust-lang/rust/pull/26741
[win3]: https://github.com/rust-lang/rust/pull/26741
[win4]: https://github.com/rust-lang/rust/pull/27210
[xp]: https://github.com/rust-lang/rust/pull/26569

Version 1.2.0 (2015-08-07)
==========================

* ~1200 changes, numerous bugfixes

Highlights
----------

* [Dynamically-sized-type coercions][dst] allow smart pointer types
  like `Rc` to contain types without a fixed size, arrays and trait
  objects, finally enabling use of `Rc<[T]>` and completing the
  implementation of DST.
* [Parallel codegen][parcodegen] is now working again, which can
  substantially speed up large builds in debug mode; It also gets
  another ~33% speedup when bootstrapping on a 4 core machine (using 8
  jobs). It's not enabled by default, but will be "in the near
  future". It can be activated with the `-C codegen-units=N` flag to
  `rustc`.
* This is the first release with [experimental support for linking
  with the MSVC linker and lib C on Windows (instead of using the GNU
  variants via MinGW)][win]. It is yet recommended only for the most
  intrepid Rustaceans.
* Benchmark compilations are showing a 30% improvement in
  bootstrapping over 1.1.

Breaking Changes
----------------

* The [`to_uppercase`] and [`to_lowercase`] methods on `char` now do
  unicode case mapping, which is a previously-planned change in
  behavior and considered a bugfix.
* [`mem::align_of`] now specifies [the *minimum alignment* for
  T][align], which is usually the alignment programs are interested
  in, and the same value reported by clang's
  `alignof`. [`mem::min_align_of`] is deprecated. This is not known to
  break real code.
* [The `#[packed]` attribute is no longer silently accepted by the
  compiler][packed]. This attribute did nothing and code that
  mentioned it likely did not work as intended.
* Associated type defaults are [now behind the
  `associated_type_defaults` feature gate][ad]. In 1.1 associated type
  defaults *did not work*, but could be mentioned syntactically. As
  such this breakage has minimal impact.

Language
--------

* Patterns with `ref mut` now correctly invoke [`DerefMut`] when
  matching against dereferenceable values.

Libraries
---------

* The [`Extend`] trait, which grows a collection from an iterator, is
  implemented over iterators of references, for `String`, `Vec`,
  `LinkedList`, `VecDeque`, `EnumSet`, `BinaryHeap`, `VecMap`,
  `BTreeSet` and `BTreeMap`. [RFC][extend-rfc].
* The [`iter::once`] function returns an iterator that yields a single
  element, and [`iter::empty`] returns an iterator that yields no
  elements.
* The [`matches`] and [`rmatches`] methods on `str` return iterators
  over substring matches.
* [`Cell`] and [`RefCell`] both implement `Eq`.
* A number of methods for wrapping arithmetic are added to the
  integral types, [`wrapping_div`], [`wrapping_rem`],
  [`wrapping_neg`], [`wrapping_shl`], [`wrapping_shr`]. These are in
  addition to the existing [`wrapping_add`], [`wrapping_sub`], and
  [`wrapping_mul`] methods, and alternatives to the [`Wrapping`]
  type.. It is illegal for the default arithmetic operations in Rust
  to overflow; the desire to wrap must be explicit.
* The `{:#?}` formatting specifier [displays the alternate,
  pretty-printed][debugfmt] form of the `Debug` formatter. This
  feature was actually introduced prior to 1.0 with little
  fanfare.
* [`fmt::Formatter`] implements [`fmt::Write`], a `fmt`-specific trait
  for writing data to formatted strings, similar to [`io::Write`].
* [`fmt::Formatter`] adds 'debug builder' methods, [`debug_struct`],
  [`debug_tuple`], [`debug_list`], [`debug_set`], [`debug_map`]. These
  are used by code generators to emit implementations of [`Debug`].
* `str` has new [`to_uppercase`][strup] and [`to_lowercase`][strlow]
  methods that convert case, following Unicode case mapping.
* It is now easier to handle poisoned locks. The [`PoisonError`]
  type, returned by failing lock operations, exposes `into_inner`,
  `get_ref`, and `get_mut`, which all give access to the inner lock
  guard, and allow the poisoned lock to continue to operate. The
  `is_poisoned` method of [`RwLock`] and [`Mutex`] can poll for a
  poisoned lock without attempting to take the lock.
* On Unix the [`FromRawFd`] trait is implemented for [`Stdio`], and
  [`AsRawFd`] for [`ChildStdin`], [`ChildStdout`], [`ChildStderr`].
  On Windows the `FromRawHandle` trait is implemented for `Stdio`,
  and `AsRawHandle` for `ChildStdin`, `ChildStdout`,
  `ChildStderr`.
* [`io::ErrorKind`] has a new variant, `InvalidData`, which indicates
  malformed input.

Misc
----

* `rustc` employs smarter heuristics for guessing at [typos].
* `rustc` emits more efficient code for [no-op conversions between
  unsafe pointers][nop].
* Fat pointers are now [passed in pairs of immediate arguments][fat],
  resulting in faster compile times and smaller code.

[`Extend`]: https://doc.rust-lang.org/nightly/std/iter/trait.Extend.html
[extend-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0839-embrace-extend-extinguish.md
[`iter::once`]: https://doc.rust-lang.org/nightly/std/iter/fn.once.html
[`iter::empty`]: https://doc.rust-lang.org/nightly/std/iter/fn.empty.html
[`matches`]: https://doc.rust-lang.org/nightly/std/primitive.str.html#method.matches
[`rmatches`]: https://doc.rust-lang.org/nightly/std/primitive.str.html#method.rmatches
[`Cell`]: https://doc.rust-lang.org/nightly/std/cell/struct.Cell.html
[`RefCell`]: https://doc.rust-lang.org/nightly/std/cell/struct.RefCell.html
[`wrapping_add`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_add
[`wrapping_sub`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_sub
[`wrapping_mul`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_mul
[`wrapping_div`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_div
[`wrapping_rem`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_rem
[`wrapping_neg`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_neg
[`wrapping_shl`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_shl
[`wrapping_shr`]: https://doc.rust-lang.org/nightly/std/primitive.i8.html#method.wrapping_shr
[`Wrapping`]: https://doc.rust-lang.org/nightly/std/num/struct.Wrapping.html
[`fmt::Formatter`]: https://doc.rust-lang.org/nightly/std/fmt/struct.Formatter.html
[`fmt::Write`]: https://doc.rust-lang.org/nightly/std/fmt/trait.Write.html
[`io::Write`]: https://doc.rust-lang.org/nightly/std/io/trait.Write.html
[`debug_struct`]: https://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.debug_struct
[`debug_tuple`]: https://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.debug_tuple
[`debug_list`]: https://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.debug_list
[`debug_set`]: https://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.debug_set
[`debug_map`]: https://doc.rust-lang.org/nightly/core/fmt/struct.Formatter.html#method.debug_map
[`Debug`]: https://doc.rust-lang.org/nightly/std/fmt/trait.Debug.html
[strup]: https://doc.rust-lang.org/nightly/std/primitive.str.html#method.to_uppercase
[strlow]: https://doc.rust-lang.org/nightly/std/primitive.str.html#method.to_lowercase
[`to_uppercase`]: https://doc.rust-lang.org/nightly/std/primitive.char.html#method.to_uppercase
[`to_lowercase`]: https://doc.rust-lang.org/nightly/std/primitive.char.html#method.to_lowercase
[`PoisonError`]: https://doc.rust-lang.org/nightly/std/sync/struct.PoisonError.html
[`RwLock`]: https://doc.rust-lang.org/nightly/std/sync/struct.RwLock.html
[`Mutex`]: https://doc.rust-lang.org/nightly/std/sync/struct.Mutex.html
[`FromRawFd`]: https://doc.rust-lang.org/nightly/std/os/unix/io/trait.FromRawFd.html
[`AsRawFd`]: https://doc.rust-lang.org/nightly/std/os/unix/io/trait.AsRawFd.html
[`Stdio`]: https://doc.rust-lang.org/nightly/std/process/struct.Stdio.html
[`ChildStdin`]: https://doc.rust-lang.org/nightly/std/process/struct.ChildStdin.html
[`ChildStdout`]: https://doc.rust-lang.org/nightly/std/process/struct.ChildStdout.html
[`ChildStderr`]: https://doc.rust-lang.org/nightly/std/process/struct.ChildStderr.html
[`io::ErrorKind`]: https://doc.rust-lang.org/nightly/std/io/enum.ErrorKind.html
[debugfmt]: https://www.reddit.com/r/rust/comments/3ceaui/psa_produces_prettyprinted_debug_output/
[`DerefMut`]: https://doc.rust-lang.org/nightly/std/ops/trait.DerefMut.html
[`mem::align_of`]: https://doc.rust-lang.org/nightly/std/mem/fn.align_of.html
[align]: https://github.com/rust-lang/rust/pull/25646
[`mem::min_align_of`]: https://doc.rust-lang.org/nightly/std/mem/fn.min_align_of.html
[typos]: https://github.com/rust-lang/rust/pull/26087
[nop]: https://github.com/rust-lang/rust/pull/26336
[fat]: https://github.com/rust-lang/rust/pull/26411
[dst]: https://github.com/rust-lang/rfcs/blob/master/text/0982-dst-coercion.md
[parcodegen]: https://github.com/rust-lang/rust/pull/26018
[packed]: https://github.com/rust-lang/rust/pull/25541
[ad]: https://github.com/rust-lang/rust/pull/27382
[win]: https://github.com/rust-lang/rust/pull/25350

Version 1.1.0 (2015-06-25)
=========================

* ~850 changes, numerous bugfixes

Highlights
----------

* The [`std::fs` module has been expanded][fs] to expand the set of
  functionality exposed:
  * `DirEntry` now supports optimizations like `file_type` and `metadata` which
    don't incur a syscall on some platforms.
  * A `symlink_metadata` function has been added.
  * The `fs::Metadata` structure now lowers to its OS counterpart, providing
    access to all underlying information.
* The compiler now contains extended explanations of many errors. When an error
  with an explanation occurs the compiler suggests using the `--explain` flag
  to read the explanation. Error explanations are also [available online][err-index].
* Thanks to multiple [improvements][sk] to [type checking][pre], as
  well as other work, the time to bootstrap the compiler decreased by
  32%.

Libraries
---------

* The [`str::split_whitespace`] method splits a string on unicode
  whitespace boundaries.
* On both Windows and Unix, new extension traits provide conversion of
  I/O types to and from the underlying system handles. On Unix, these
  traits are [`FromRawFd`] and [`AsRawFd`], on Windows `FromRawHandle`
  and `AsRawHandle`. These are implemented for `File`, `TcpStream`,
  `TcpListener`, and `UpdSocket`. Further implementations for
  `std::process` will be stabilized later.
* On Unix, [`std::os::unix::symlink`] creates symlinks. On
  Windows, symlinks can be created with
  `std::os::windows::symlink_dir` and
  `std::os::windows::symlink_file`.
* The `mpsc::Receiver` type can now be converted into an iterator with
  `into_iter` on the [`IntoIterator`] trait.
* `Ipv4Addr` can be created from `u32` with the `From<u32>`
  implementation of the [`From`] trait.
* The `Debug` implementation for `RangeFull` [creates output that is
  more consistent with other implementations][rf].
* [`Debug` is implemented for `File`][file].
* The `Default` implementation for `Arc` [no longer requires `Sync +
  Send`][arc].
* [The `Iterator` methods `count`, `nth`, and `last` have been
  overridden for slices to have O(1) performance instead of O(n)][si].
* Incorrect handling of paths on Windows has been improved in both the
  compiler and the standard library.
* [`AtomicPtr` gained a `Default` implementation][ap].
* In accordance with Rust's policy on arithmetic overflow `abs` now
  [panics on overflow when debug assertions are enabled][abs].
* The [`Cloned`] iterator, which was accidentally left unstable for
  1.0 [has been stabilized][c].
* The [`Incoming`] iterator, which iterates over incoming TCP
  connections, and which was accidentally unnamable in 1.0, [is now
  properly exported][inc].
* [`BinaryHeap`] no longer corrupts itself [when functions called by
  `sift_up` or `sift_down` panic][bh].
* The [`split_off`] method of `LinkedList` [no longer corrupts
  the list in certain scenarios][ll].

Misc
----

* Type checking performance [has improved notably][sk] with
  [multiple improvements][pre].
* The compiler [suggests code changes][ch] for more errors.
* rustc and it's build system have experimental support for [building
  toolchains against MUSL][m] instead of glibc on Linux.
* The compiler defines the `target_env` cfg value, which is used for
  distinguishing toolchains that are otherwise for the same
  platform. Presently this is set to `gnu` for common GNU Linux
  targets and for MinGW targets, and `musl` for MUSL Linux targets.
* The [`cargo rustc`][crc] command invokes a build with custom flags
  to rustc.
* [Android executables are always position independent][pie].
* [The `drop_with_repr_extern` lint warns about mixing `repr(C)`
  with `Drop`][drop].

[`str::split_whitespace`]: https://doc.rust-lang.org/nightly/std/primitive.str.html#method.split_whitespace
[`FromRawFd`]: https://doc.rust-lang.org/nightly/std/os/unix/io/trait.FromRawFd.html
[`AsRawFd`]: https://doc.rust-lang.org/nightly/std/os/unix/io/trait.AsRawFd.html
[`std::os::unix::symlink`]: https://doc.rust-lang.org/nightly/std/os/unix/fs/fn.symlink.html
[`IntoIterator`]: https://doc.rust-lang.org/nightly/std/iter/trait.IntoIterator.html
[`From`]: https://doc.rust-lang.org/nightly/std/convert/trait.From.html
[rf]: https://github.com/rust-lang/rust/pull/24491
[err-index]: https://doc.rust-lang.org/error-index.html
[sk]: https://github.com/rust-lang/rust/pull/24615
[pre]: https://github.com/rust-lang/rust/pull/25323
[file]: https://github.com/rust-lang/rust/pull/24598
[ch]: https://github.com/rust-lang/rust/pull/24683
[arc]: https://github.com/rust-lang/rust/pull/24695
[si]: https://github.com/rust-lang/rust/pull/24701
[ap]: https://github.com/rust-lang/rust/pull/24834
[m]: https://github.com/rust-lang/rust/pull/24777
[fs]: https://github.com/rust-lang/rfcs/blob/master/text/1044-io-fs-2.1.md
[crc]: https://github.com/rust-lang/cargo/pull/1568
[pie]: https://github.com/rust-lang/rust/pull/24953
[abs]: https://github.com/rust-lang/rust/pull/25441
[c]: https://github.com/rust-lang/rust/pull/25496
[`Cloned`]: https://doc.rust-lang.org/nightly/std/iter/struct.Cloned.html
[`Incoming`]: https://doc.rust-lang.org/nightly/std/net/struct.Incoming.html
[inc]: https://github.com/rust-lang/rust/pull/25522
[bh]: https://github.com/rust-lang/rust/pull/25856
[`BinaryHeap`]: https://doc.rust-lang.org/nightly/std/collections/struct.BinaryHeap.html
[ll]: https://github.com/rust-lang/rust/pull/26022
[`split_off`]: https://doc.rust-lang.org/nightly/collections/linked_list/struct.LinkedList.html#method.split_off
[drop]: https://github.com/rust-lang/rust/pull/24935

Version 1.0.0 (2015-05-15)
========================

* ~1500 changes, numerous bugfixes

Highlights
----------

* The vast majority of the standard library is now `#[stable]`. It is
  no longer possible to use unstable features with a stable build of
  the compiler.
* Many popular crates on [crates.io] now work on the stable release
  channel.
* Arithmetic on basic integer types now [checks for overflow in debug
  builds][overflow].

Language
--------

* Several [restrictions have been added to trait coherence][coh] in
  order to make it easier for upstream authors to change traits
  without breaking downstream code.
* Digits of binary and octal literals are [lexed more eagerly][lex] to
  improve error messages and macro behavior. For example, `0b1234` is
  now lexed as `0b1234` instead of two tokens, `0b1` and `234`.
* Trait bounds [are always invariant][inv], eliminating the need for
  the `PhantomFn` and `MarkerTrait` lang items, which have been
  removed.
* ["-" is no longer a valid character in crate names][cr], the `extern crate
  "foo" as bar` syntax has been replaced with `extern crate foo as
  bar`, and Cargo now automatically translates "-" in *package* names
  to underscore for the crate name.
* [Lifetime shadowing is an error][lt].
* [`Send` no longer implies `'static`][send-rfc].
* [UFCS now supports trait-less associated paths][moar-ufcs] like
  `MyType::default()`.
* Primitive types [now have inherent methods][prim-inherent],
  obviating the need for extension traits like `SliceExt`.
* Methods with `Self: Sized` in their `where` clause are [considered
  object-safe][self-sized], allowing many extension traits like
  `IteratorExt` to be merged into the traits they extended.
* You can now [refer to associated types][assoc-where] whose
  corresponding trait bounds appear only in a `where` clause.
* The final bits of [OIBIT landed][oibit-final], meaning that traits
  like `Send` and `Sync` are now library-defined.
* A [Reflect trait][reflect] was introduced, which means that
  downcasting via the `Any` trait is effectively limited to concrete
  types. This helps retain the potentially-important "parametricity"
  property: generic code cannot behave differently for different type
  arguments except in minor ways.
* The `unsafe_destructor` feature is now deprecated in favor of the
  [new `dropck`][dropck]. This change is a major reduction in unsafe
  code.

Libraries
---------

* The `thread_local` module [has been renamed to `std::thread`][th].
* The methods of `IteratorExt` [have been moved to the `Iterator`
  trait itself][ie].
* Several traits that implement Rust's conventions for type
  conversions, `AsMut`, `AsRef`, `From`, and `Into` have been
  [centralized in the `std::convert` module][con].
* The `FromError` trait [was removed in favor of `From`][fe].
* The basic sleep function [has moved to
  `std::thread::sleep_ms`][slp].
* The `splitn` function now takes an `n` parameter that represents the
  number of items yielded by the returned iterator [instead of the
  number of 'splits'][spl].
* [On Unix, all file descriptors are `CLOEXEC` by default][clo].
* [Derived implementations of `PartialOrd` now order enums according
  to their explicitly-assigned discriminants][po].
* [Methods for searching strings are generic over `Pattern`s][pat],
  implemented presently by `&char`, `&str`, `FnMut(char) -> bool` and
  some others.
* [In method resolution, object methods are resolved before inherent
  methods][meth].
* [`String::from_str` has been deprecated in favor of the `From` impl,
  `String::from`][sf].
* [`io::Error` implements `Sync`][ios].
* [The `words` method on `&str` has been replaced with
  `split_whitespace`][sw], to avoid answering the tricky question, 'what is
  a word?'
* The new path and IO modules are complete and `#[stable]`. This
  was the major library focus for this cycle.
* The path API was [revised][path-normalize] to normalize `.`,
  adjusting the tradeoffs in favor of the most common usage.
* A large number of remaining APIs in `std` were also stabilized
  during this cycle; about 75% of the non-deprecated API surface
  is now stable.
* The new [string pattern API][string-pattern] landed, which makes
  the string slice API much more internally consistent and flexible.
* A new set of [generic conversion traits][conversion] replaced
  many existing ad hoc traits.
* Generic numeric traits were [completely removed][num-traits]. This
  was made possible thanks to inherent methods for primitive types,
  and the removal gives maximal flexibility for designing a numeric
  hierarchy in the future.
* The `Fn` traits are now related via [inheritance][fn-inherit]
  and provide ergonomic [blanket implementations][fn-blanket].
* The `Index` and `IndexMut` traits were changed to
  [take the index by value][index-value], enabling code like
  `hash_map["string"]` to work.
* `Copy` now [inherits][copy-clone] from `Clone`, meaning that all
  `Copy` data is known to be `Clone` as well.

Misc
----

* Many errors now have extended explanations that can be accessed with
  the `--explain` flag to `rustc`.
* Many new examples have been added to the standard library
  documentation.
* rustdoc has received a number of improvements focused on completion
  and polish.
* Metadata was tuned, shrinking binaries [by 27%][metadata-shrink].
* Much headway was made on ecosystem-wide CI, making it possible
  to [compare builds for breakage][ci-compare].


[crates.io]: http://crates.io
[clo]: https://github.com/rust-lang/rust/pull/24034
[coh]: https://github.com/rust-lang/rfcs/blob/master/text/1023-rebalancing-coherence.md
[con]: https://github.com/rust-lang/rust/pull/23875
[cr]: https://github.com/rust-lang/rust/pull/23419
[fe]: https://github.com/rust-lang/rust/pull/23879
[ie]: https://github.com/rust-lang/rust/pull/23300
[inv]: https://github.com/rust-lang/rust/pull/23938
[ios]: https://github.com/rust-lang/rust/pull/24133
[lex]: https://github.com/rust-lang/rfcs/blob/master/text/0879-small-base-lexing.md
[lt]: https://github.com/rust-lang/rust/pull/24057
[meth]: https://github.com/rust-lang/rust/pull/24056
[pat]: https://github.com/rust-lang/rfcs/blob/master/text/0528-string-patterns.md
[po]: https://github.com/rust-lang/rust/pull/24270
[sf]: https://github.com/rust-lang/rust/pull/24517
[slp]: https://github.com/rust-lang/rust/pull/23949
[spl]: https://github.com/rust-lang/rfcs/blob/master/text/0979-align-splitn-with-other-languages.md
[sw]: https://github.com/rust-lang/rfcs/blob/master/text/1054-str-words.md
[th]: https://github.com/rust-lang/rfcs/blob/master/text/0909-move-thread-local-to-std-thread.md
[send-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0458-send-improvements.md
[moar-ufcs]: https://github.com/rust-lang/rust/pull/22172
[prim-inherent]: https://github.com/rust-lang/rust/pull/23104
[overflow]: https://github.com/rust-lang/rfcs/blob/master/text/0560-integer-overflow.md
[metadata-shrink]: https://github.com/rust-lang/rust/pull/22971
[self-sized]: https://github.com/rust-lang/rust/pull/22301
[assoc-where]: https://github.com/rust-lang/rust/pull/22512
[string-pattern]: https://github.com/rust-lang/rust/pull/22466
[oibit-final]: https://github.com/rust-lang/rust/pull/21689
[reflect]: https://github.com/rust-lang/rust/pull/23712
[conversion]: https://github.com/rust-lang/rfcs/pull/529
[num-traits]: https://github.com/rust-lang/rust/pull/23549
[index-value]: https://github.com/rust-lang/rust/pull/23601
[dropck]: https://github.com/rust-lang/rfcs/pull/769
[ci-compare]: https://gist.github.com/brson/a30a77836fbec057cbee
[fn-inherit]: https://github.com/rust-lang/rust/pull/23282
[fn-blanket]: https://github.com/rust-lang/rust/pull/23895
[copy-clone]: https://github.com/rust-lang/rust/pull/23860
[path-normalize]: https://github.com/rust-lang/rust/pull/23229


Version 1.0.0-alpha.2 (2015-02-20)
=====================================

* ~1300 changes, numerous bugfixes

* Highlights

    * The various I/O modules were [overhauled][io-rfc] to reduce
      unnecessary abstractions and provide better interoperation with
      the underlying platform. The old `io` module remains temporarily
      at `std::old_io`.
    * The standard library now [participates in feature gating][feat],
      so use of unstable libraries now requires a `#![feature(...)]`
      attribute. The impact of this change is [described on the
      forum][feat-forum]. [RFC][feat-rfc].

* Language

    * `for` loops [now operate on the `IntoIterator` trait][into],
      which eliminates the need to call `.iter()`, etc. to iterate
      over collections. There are some new subtleties to remember
      though regarding what sort of iterators various types yield, in
      particular that `for foo in bar { }` yields values from a move
      iterator, destroying the original collection. [RFC][into-rfc].
    * Objects now have [default lifetime bounds][obj], so you don't
      have to write `Box<Trait+'static>` when you don't care about
      storing references. [RFC][obj-rfc].
    * In types that implement `Drop`, [lifetimes must outlive the
      value][drop]. This will soon make it possible to safely
      implement `Drop` for types where `#[unsafe_destructor]` is now
      required. Read the [gorgeous RFC][drop-rfc] for details.
    * The fully qualified <T as Trait>::X syntax lets you set the Self
      type for a trait method or associated type. [RFC][ufcs-rfc].
    * References to types that implement `Deref<U>` now [automatically
      coerce to references][deref] to the dereferenced type `U`,
      e.g. `&T where T: Deref<U>` automatically coerces to `&U`. This
      should eliminate many unsightly uses of `&*`, as when converting
      from references to vectors into references to
      slices. [RFC][deref-rfc].
    * The explicit [closure kind syntax][close] (`|&:|`, `|&mut:|`,
      `|:|`) is obsolete and closure kind is inferred from context.
    * [`Self` is a keyword][Self].

* Libraries

    * The `Show` and `String` formatting traits [have been
      renamed][fmt] to `Debug` and `Display` to more clearly reflect
      their related purposes. Automatically getting a string
      conversion to use with `format!("{:?}", something_to_debug)` is
      now written `#[derive(Debug)]`.
    * Abstract [OS-specific string types][osstr], `std::ff::{OsString,
      OsStr}`, provide strings in platform-specific encodings for easier
      interop with system APIs. [RFC][osstr-rfc].
    * The `boxed::into_raw` and `Box::from_raw` functions [convert
      between `Box<T>` and `*mut T`][boxraw], a common pattern for
      creating raw pointers.

* Tooling

    * Certain long error messages of the form 'expected foo found bar'
      are now [split neatly across multiple
      lines][multiline]. Examples in the PR.
    * On Unix Rust can be [uninstalled][un] by running
      `/usr/local/lib/rustlib/uninstall.sh`.
    * The `#[rustc_on_unimplemented]` attribute, requiring the
      'on_unimplemented' feature, lets rustc [display custom error
      messages when a trait is expected to be implemented for a type
      but is not][onun].

* Misc

    * Rust is tested against a [LALR grammar][lalr], which parses
      almost all the Rust files that rustc does.

[boxraw]: https://github.com/rust-lang/rust/pull/21318
[close]: https://github.com/rust-lang/rust/pull/21843
[deref]: https://github.com/rust-lang/rust/pull/21351
[deref-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0241-deref-conversions.md
[drop]: https://github.com/rust-lang/rust/pull/21972
[drop-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0769-sound-generic-drop.md
[feat]: https://github.com/rust-lang/rust/pull/21248
[feat-forum]: https://users.rust-lang.org/t/psa-important-info-about-rustcs-new-feature-staging/82/5
[feat-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0507-release-channels.md
[fmt]: https://github.com/rust-lang/rust/pull/21457
[into]: https://github.com/rust-lang/rust/pull/20790
[into-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0235-collections-conventions.md#intoiterator-and-iterable
[io-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0517-io-os-reform.md
[lalr]: https://github.com/rust-lang/rust/pull/21452
[multiline]: https://github.com/rust-lang/rust/pull/19870
[obj]: https://github.com/rust-lang/rust/pull/22230
[obj-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0599-default-object-bound.md
[onun]: https://github.com/rust-lang/rust/pull/20889
[osstr]: https://github.com/rust-lang/rust/pull/21488
[osstr-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0517-io-os-reform.md
[Self]: https://github.com/rust-lang/rust/pull/22158
[ufcs-rfc]: https://github.com/rust-lang/rfcs/blob/master/text/0132-ufcs.md
[un]: https://github.com/rust-lang/rust/pull/22256


Version 1.0.0-alpha (2015-01-09)
==================================

  * ~2400 changes, numerous bugfixes

  * Highlights

    * The language itself is considered feature complete for 1.0,
      though there will be many usability improvements and bugfixes
      before the final release.
    * Nearly 50% of the public API surface of the standard library has
      been declared 'stable'. Those interfaces are unlikely to change
      before 1.0.
    * The long-running debate over integer types has been
      [settled][ints]: Rust will ship with types named `isize` and
      `usize`, rather than `int` and `uint`, for pointer-sized
      integers. Guidelines will be rolled out during the alpha cycle.
    * Most crates that are not `std` have been moved out of the Rust
      distribution into the Cargo ecosystem so they can evolve
      separately and don't need to be stabilized as quickly, including
      'time', 'getopts', 'num', 'regex', and 'term'.
    * Documentation continues to be expanded with more API coverage, more
      examples, and more in-depth explanations. The guides have been
      consolidated into [The Rust Programming Language][trpl].
    * "[Rust By Example][rbe]" is now maintained by the Rust team.
    * All official Rust binary installers now come with [Cargo], the
      Rust package manager.

* Language

    * Closures have been [completely redesigned][unboxed] to be
      implemented in terms of traits, can now be used as generic type
      bounds and thus monomorphized and inlined, or via an opaque
      pointer (boxed) as in the old system. The new system is often
      referred to as 'unboxed' closures.
    * Traits now support [associated types][assoc], allowing families
      of related types to be defined together and used generically in
      powerful ways.
    * Enum variants are [namespaced by their type names][enum].
    * [`where` clauses][where] provide a more versatile and attractive
      syntax for specifying generic bounds, though the previous syntax
      remains valid.
    * Rust again picks a [fallback][fb] (either i32 or f64) for uninferred
      numeric types.
    * Rust [no longer has a runtime][rt] of any description, and only
      supports OS threads, not green threads.
    * At long last, Rust has been overhauled for 'dynamically-sized
      types' ([DST]), which integrates 'fat pointers' (object types,
      arrays, and `str`) more deeply into the type system, making it
      more consistent.
    * Rust now has a general [range syntax][range], `i..j`, `i..`, and
      `..j` that produce range types and which, when combined with the
      `Index` operator and multidispatch, leads to a convenient slice
      notation, `[i..j]`.
    * The new range syntax revealed an ambiguity in the fixed-length
      array syntax, so now fixed length arrays [are written `[T;
      N]`][arrays].
    * The `Copy` trait is no longer implemented automatically. Unsafe
      pointers no longer implement `Sync` and `Send` so types
      containing them don't automatically either. `Sync` and `Send`
      are now 'unsafe traits' so one can "forcibly" implement them via
      `unsafe impl` if a type confirms to the requirements for them
      even though the internals do not (e.g. structs containing unsafe
      pointers like `Arc`). These changes are intended to prevent some
      footguns and are collectively known as [opt-in built-in
      traits][oibit] (though `Sync` and `Send` will soon become pure
      library types unknown to the compiler).
    * Operator traits now take their operands [by value][ops], and
      comparison traits can use multidispatch to compare one type
      against multiple other types, allowing e.g. `String` to be
      compared with `&str`.
    * `if let` and `while let` are no longer feature-gated.
    * Rust has adopted a more [uniform syntax for escaping unicode
      characters][unicode].
    * `macro_rules!` [has been declared stable][mac]. Though it is a
      flawed system it is sufficiently popular that it must be usable
      for 1.0. Effort has gone into [future-proofing][mac-future] it
      in ways that will allow other macro systems to be developed in
      parallel, and won't otherwise impact the evolution of the
      language.
    * The prelude has been [pared back significantly][prelude] such
      that it is the minimum necessary to support the most pervasive
      code patterns, and through [generalized where clauses][where]
      many of the prelude extension traits have been consolidated.
    * Rust's rudimentary reflection [has been removed][refl], as it
      incurred too much code generation for little benefit.
    * [Struct variants][structvars] are no longer feature-gated.
    * Trait bounds can be [polymorphic over lifetimes][hrtb]. Also
      known as 'higher-ranked trait bounds', this crucially allows
      unboxed closures to work.
    * Macros invocations surrounded by parens or square brackets and
      not terminated by a semicolon are [parsed as
      expressions][macros], which makes expressions like `vec![1i32,
      2, 3].len()` work as expected.
    * Trait objects now implement their traits automatically, and
      traits that can be coerced to objects now must be [object
      safe][objsafe].
    * Automatically deriving traits is now done with `#[derive(...)]`
      not `#[deriving(...)]` for [consistency with other naming
      conventions][derive].
    * Importing the containing module or enum at the same time as
      items or variants they contain is [now done with `self` instead
      of `mod`][self], as in use `foo::{self, bar}`
    * Glob imports are no longer feature-gated.
    * The `box` operator and `box` patterns have been feature-gated
      pending a redesign. For now unique boxes should be allocated
      like other containers, with `Box::new`.

* Libraries

    * A [series][coll1] of [efforts][coll2] to establish
      [conventions][coll3] for collections types has resulted in API
      improvements throughout the standard library.
    * New [APIs for error handling][err] provide ergonomic interop
      between error types, and [new conventions][err-conv] describe
      more clearly the recommended error handling strategies in Rust.
    * The `fail!` macro has been renamed to [`panic!`][panic] so that
      it is easier to discuss failure in the context of error handling
      without making clarifications as to whether you are referring to
      the 'fail' macro or failure more generally.
    * On Linux, `OsRng` prefers the new, more reliable `getrandom`
      syscall when available.
    * The 'serialize' crate has been renamed 'rustc-serialize' and
      moved out of the distribution to Cargo. Although it is widely
      used now, it is expected to be superseded in the near future.
    * The `Show` formatter, typically implemented with
      `#[derive(Show)]` is [now requested with the `{:?}`
      specifier][show] and is intended for use by all types, for uses
      such as `println!` debugging. The new `String` formatter must be
      implemented by hand, uses the `{}` specifier, and is intended
      for full-fidelity conversions of things that can logically be
      represented as strings.

* Tooling

    * [Flexible target specification][flex] allows rustc's code
      generation to be configured to support otherwise-unsupported
      platforms.
    * Rust comes with rust-gdb and rust-lldb scripts that launch their
      respective debuggers with Rust-appropriate pretty-printing.
    * The Windows installation of Rust is distributed with the
      MinGW components currently required to link binaries on that
      platform.

* Misc

    * Nullable enum optimizations have been extended to more types so
      that e.g. `Option<Vec<T>>` and `Option<String>` take up no more
      space than the inner types themselves.
    * Work has begun on supporting AArch64.

[Cargo]: https://crates.io
[unboxed]: http://smallcultfollowing.com/babysteps/blog/2014/11/26/purging-proc/
[enum]: https://github.com/rust-lang/rfcs/blob/master/text/0390-enum-namespacing.md
[flex]: https://github.com/rust-lang/rfcs/blob/master/text/0131-target-specification.md
[err]: https://github.com/rust-lang/rfcs/blob/master/text/0201-error-chaining.md
[err-conv]: https://github.com/rust-lang/rfcs/blob/master/text/0236-error-conventions.md
[rt]: https://github.com/rust-lang/rfcs/blob/master/text/0230-remove-runtime.md
[mac]: https://github.com/rust-lang/rfcs/blob/master/text/0453-macro-reform.md
[mac-future]: https://github.com/rust-lang/rfcs/pull/550
[DST]: http://smallcultfollowing.com/babysteps/blog/2014/01/05/dst-take-5/
[coll1]: https://github.com/rust-lang/rfcs/blob/master/text/0235-collections-conventions.md
[coll2]: https://github.com/rust-lang/rfcs/blob/master/text/0509-collections-reform-part-2.md
[coll3]: https://github.com/rust-lang/rfcs/blob/master/text/0216-collection-views.md
[ops]: https://github.com/rust-lang/rfcs/blob/master/text/0439-cmp-ops-reform.md
[prelude]: https://github.com/rust-lang/rfcs/blob/master/text/0503-prelude-stabilization.md
[where]: https://github.com/rust-lang/rfcs/blob/master/text/0135-where.md
[refl]: https://github.com/rust-lang/rfcs/blob/master/text/0379-remove-reflection.md
[panic]: https://github.com/rust-lang/rfcs/blob/master/text/0221-panic.md
[structvars]: https://github.com/rust-lang/rfcs/blob/master/text/0418-struct-variants.md
[hrtb]: https://github.com/rust-lang/rfcs/blob/master/text/0387-higher-ranked-trait-bounds.md
[unicode]: https://github.com/rust-lang/rfcs/blob/master/text/0446-es6-unicode-escapes.md
[oibit]: https://github.com/rust-lang/rfcs/blob/master/text/0019-opt-in-builtin-traits.md
[macros]: https://github.com/rust-lang/rfcs/blob/master/text/0378-expr-macros.md
[range]: https://github.com/rust-lang/rfcs/blob/master/text/0439-cmp-ops-reform.md#indexing-and-slicing
[arrays]: https://github.com/rust-lang/rfcs/blob/master/text/0520-new-array-repeat-syntax.md
[show]: https://github.com/rust-lang/rfcs/blob/master/text/0504-show-stabilization.md
[derive]: https://github.com/rust-lang/rfcs/blob/master/text/0534-deriving2derive.md
[self]: https://github.com/rust-lang/rfcs/blob/master/text/0532-self-in-use.md
[fb]: https://github.com/rust-lang/rfcs/blob/master/text/0212-restore-int-fallback.md
[objsafe]: https://github.com/rust-lang/rfcs/blob/master/text/0255-object-safety.md
[assoc]: https://github.com/rust-lang/rfcs/blob/master/text/0195-associated-items.md
[ints]: https://github.com/rust-lang/rfcs/pull/544#issuecomment-68760871
[trpl]: https://doc.rust-lang.org/book/index.html
[rbe]: http://rustbyexample.com/


Version 0.12.0 (2014-10-09)
=============================

  * ~1900 changes, numerous bugfixes

  * Highlights

    * The introductory documentation (now called The Rust Guide) has
      been completely rewritten, as have a number of supplementary
      guides.
    * Rust's package manager, Cargo, continues to improve and is
      sometimes considered to be quite awesome.
    * Many API's in `std` have been reviewed and updated for
      consistency with the in-development Rust coding
      guidelines. The standard library documentation tracks
      stabilization progress.
    * Minor libraries have been moved out-of-tree to the rust-lang org
      on GitHub: uuid, semver, glob, num, hexfloat, fourcc. They can
      be installed with Cargo.
    * Lifetime elision allows lifetime annotations to be left off of
      function declarations in many common scenarios.
    * Rust now works on 64-bit Windows.

  * Language
    * Indexing can be overloaded with the `Index` and `IndexMut`
      traits.
    * The `if let` construct takes a branch only if the `let` pattern
      matches, currently behind the 'if_let' feature gate.
    * 'where clauses', a more flexible syntax for specifying trait
      bounds that is more aesthetic, have been added for traits and
      free functions. Where clauses will in the future make it
      possible to constrain associated types, which would be
      impossible with the existing syntax.
    * A new slicing syntax (e.g. `[0..4]`) has been introduced behind
      the 'slicing_syntax' feature gate, and can be overloaded with
      the `Slice` or `SliceMut` traits.
    * The syntax for matching of sub-slices has been changed to use a
      postfix `..` instead of prefix (.e.g. `[a, b, c..]`), for
      consistency with other uses of `..` and to future-proof
      potential additional uses of the syntax.
    * The syntax for matching inclusive ranges in patterns has changed
      from `0..3` to `0...4` to be consistent with the exclusive range
      syntax for slicing.
    * Matching of sub-slices in non-tail positions (e.g.  `[a.., b,
      c]`) has been put behind the 'advanced_slice_patterns' feature
      gate and may be removed in the future.
    * Components of tuples and tuple structs can be extracted using
      the `value.0` syntax, currently behind the `tuple_indexing`
      feature gate.
    * The `#[crate_id]` attribute is no longer supported; versioning
      is handled by the package manager.
    * Renaming crate imports are now written `extern crate foo as bar`
      instead of `extern crate bar = foo`.
    * Renaming use statements are now written `use foo as bar` instead
      of `use bar = foo`.
    * `let` and `match` bindings and argument names in macros are now
      hygienic.
    * The new, more efficient, closure types ('unboxed closures') have
      been added under a feature gate, 'unboxed_closures'. These will
      soon replace the existing closure types, once higher-ranked
      trait lifetimes are added to the language.
    * `move` has been added as a keyword, for indicating closures
      that capture by value.
    * Mutation and assignment is no longer allowed in pattern guards.
    * Generic structs and enums can now have trait bounds.
    * The `Share` trait is now called `Sync` to free up the term
      'shared' to refer to 'shared reference' (the default reference
      type.
    * Dynamically-sized types have been mostly implemented,
      unifying the behavior of fat-pointer types with the rest of the
      type system.
    * As part of dynamically-sized types, the `Sized` trait has been
      introduced, which qualifying types implement by default, and
      which type parameters expect by default. To specify that a type
      parameter does not need to be sized, write `<Sized? T>`. Most
      types are `Sized`, notable exceptions being unsized arrays
      (`[T]`) and trait types.
    * Closures can return `!`, as in `|| -> !` or `proc() -> !`.
    * Lifetime bounds can now be applied to type parameters and object
      types.
    * The old, reference counted GC type, `Gc<T>` which was once
      denoted by the `@` sigil, has finally been removed. GC will be
      revisited in the future.

  * Libraries
    * Library documentation has been improved for a number of modules.
    * Bit-vectors, collections::bitv has been modernized.
    * The url crate is deprecated in favor of
      http://github.com/servo/rust-url, which can be installed with
      Cargo.
    * Most I/O stream types can be cloned and subsequently closed from
      a different thread.
    * A `std::time::Duration` type has been added for use in I/O
      methods that rely on timers, as well as in the 'time' crate's
      `Timespec` arithmetic.
    * The runtime I/O abstraction layer that enabled the green thread
      scheduler to do non-thread-blocking I/O has been removed, along
      with the libuv-based implementation employed by the green thread
      scheduler. This will greatly simplify the future I/O work.
    * `collections::btree` has been rewritten to have a more
      idiomatic and efficient design.

  * Tooling
    * rustdoc output now indicates the stability levels of API's.
    * The `--crate-name` flag can specify the name of the crate
      being compiled, like `#[crate_name]`.
    * The `-C metadata` specifies additional metadata to hash into
      symbol names, and `-C extra-filename` specifies additional
      information to put into the output filename, for use by the
      package manager for versioning.
    * debug info generation has continued to improve and should be
      more reliable under both gdb and lldb.
    * rustc has experimental support for compiling in parallel
      using the `-C codegen-units` flag.
    * rustc no longer encodes rpath information into binaries by
      default.

  * Misc
    * Stack usage has been optimized with LLVM lifetime annotations.
    * Official Rust binaries on Linux are more compatible with older
      kernels and distributions, built on CentOS 5.10.


Version 0.11.0 (2014-07-02)
==========================

  * ~1700 changes, numerous bugfixes

  * Language
    * ~[T] has been removed from the language. This type is superseded by
      the Vec<T> type.
    * ~str has been removed from the language. This type is superseded by
      the String type.
    * ~T has been removed from the language. This type is superseded by the
      Box<T> type.
    * @T has been removed from the language. This type is superseded by the
      standard library's std::gc::Gc<T> type.
    * Struct fields are now all private by default.
    * Vector indices and shift amounts are both required to be a `uint`
      instead of any integral type.
    * Byte character, byte string, and raw byte string literals are now all
      supported by prefixing the normal literal with a `b`.
    * Multiple ABIs are no longer allowed in an ABI string
    * The syntax for lifetimes on closures/procedures has been tweaked
      slightly: `<'a>|A, B|: 'b + K -> T`
    * Floating point modulus has been removed from the language; however it
      is still provided by a library implementation.
    * Private enum variants are now disallowed.
    * The `priv` keyword has been removed from the language.
    * A closure can no longer be invoked through a &-pointer.
    * The `use foo, bar, baz;` syntax has been removed from the language.
    * The transmute intrinsic no longer works on type parameters.
    * Statics now allow blocks/items in their definition.
    * Trait bounds are separated from objects with + instead of : now.
    * Objects can no longer be read while they are mutably borrowed.
    * The address of a static is now marked as insignificant unless the
      #[inline(never)] attribute is placed it.
    * The #[unsafe_destructor] attribute is now behind a feature gate.
    * Struct literals are no longer allowed in ambiguous positions such as
      if, while, match, and for..in.
    * Declaration of lang items and intrinsics are now feature-gated by
      default.
    * Integral literals no longer default to `int`, and floating point
      literals no longer default to `f64`. Literals must be suffixed with an
      appropriate type if inference cannot determine the type of the
      literal.
    * The Box<T> type is no longer implicitly borrowed to &mut T.
    * Procedures are now required to not capture borrowed references.

  * Libraries
    * The standard library is now a "facade" over a number of underlying
      libraries. This means that development on the standard library should
      be speedier due to smaller crates, as well as a clearer line between
      all dependencies.
    * A new library, libcore, lives under the standard library's facade
      which is Rust's "0-assumption" library, suitable for embedded and
      kernel development for example.
    * A regex crate has been added to the standard distribution. This crate
      includes statically compiled regular expressions.
    * The unwrap/unwrap_err methods on Result require a Show bound for
      better error messages.
    * The return types of the std::comm primitives have been centralized
      around the Result type.
    * A number of I/O primitives have gained the ability to time out their
      operations.
    * A number of I/O primitives have gained the ability to close their
      reading/writing halves to cancel pending operations.
    * Reverse iterator methods have been removed in favor of `rev()` on
      their forward-iteration counterparts.
    * A bitflags! macro has been added to enable easy interop with C and
      management of bit flags.
    * A debug_assert! macro is now provided which is disabled when
      `--cfg ndebug` is passed to the compiler.
    * A graphviz crate has been added for creating .dot files.
    * The std::cast module has been migrated into std::mem.
    * The std::local_data api has been migrated from freestanding functions
      to being based on methods.
    * The Pod trait has been renamed to Copy.
    * jemalloc has been added as the default allocator for types.
    * The API for allocating memory has been changed to use proper alignment
      and sized deallocation
    * Connecting a TcpStream or binding a TcpListener is now based on a
      string address and a u16 port. This allows connecting to a hostname as
      opposed to an IP.
    * The Reader trait now contains a core method, read_at_least(), which
      correctly handles many repeated 0-length reads.
    * The process-spawning API is now centered around a builder-style
      Command struct.
    * The :? printing qualifier has been moved from the standard library to
      an external libdebug crate.
    * Eq/Ord have been renamed to PartialEq/PartialOrd. TotalEq/TotalOrd
      have been renamed to Eq/Ord.
    * The select/plural methods have been removed from format!. The escapes
      for { and } have also changed from \{ and \} to {{ and }},
      respectively.
    * The TaskBuilder API has been re-worked to be a true builder, and
      extension traits for spawning native/green tasks have been added.

  * Tooling
    * All breaking changes to the language or libraries now have their
      commit message annotated with `[breaking-change]` to allow for easy
      discovery of breaking changes.
    * The compiler will now try to suggest how to annotate lifetimes if a
      lifetime-related error occurs.
    * Debug info continues to be improved greatly with general bug fixes and
      better support for situations like link time optimization (LTO).
    * Usage of syntax extensions when cross-compiling has been fixed.
    * Functionality equivalent to GCC & Clang's -ffunction-sections,
      -fdata-sections and --gc-sections has been enabled by default
    * The compiler is now stricter about where it will load module files
      from when a module is declared via `mod foo;`.
    * The #[phase(syntax)] attribute has been renamed to #[phase(plugin)].
      Syntax extensions are now discovered via a "plugin registrar" type
      which will be extended in the future to other various plugins.
    * Lints have been restructured to allow for dynamically loadable lints.
    * A number of rustdoc improvements:
      * The HTML output has been visually redesigned.
      * Markdown is now powered by hoedown instead of sundown.
      * Searching heuristics have been greatly improved.
      * The search index has been reduced in size by a great amount.
      * Cross-crate documentation via `pub use` has been greatly improved.
      * Primitive types are now hyperlinked and documented.
    * Documentation has been moved from static.rust-lang.org/doc to
      doc.rust-lang.org
    * A new sandbox, play.rust-lang.org, is available for running and
      sharing rust code examples on-line.
    * Unused attributes are now more robustly warned about.
    * The dead_code lint now warns about unused struct fields.
    * Cross-compiling to iOS is now supported.
    * Cross-compiling to mipsel is now supported.
    * Stability attributes are now inherited by default and no longer apply
      to intra-crate usage, only inter-crate usage.
    * Error message related to non-exhaustive match expressions have been
      greatly improved.


Version 0.10 (2014-04-03)
=========================

  * ~1500 changes, numerous bugfixes

  * Language
    * A new RFC process is now in place for modifying the language.
    * Patterns with `@`-pointers have been removed from the language.
    * Patterns with unique vectors (`~[T]`) have been removed from the
      language.
    * Patterns with unique strings (`~str`) have been removed from the
      language.
    * `@str` has been removed from the language.
    * `@[T]` has been removed from the language.
    * `@self` has been removed from the language.
    * `@Trait` has been removed from the language.
    * Headers on `~` allocations which contain `@` boxes inside the type for
      reference counting have been removed.
    * The semantics around the lifetimes of temporary expressions have changed,
      see #3511 and #11585 for more information.
    * Cross-crate syntax extensions are now possible, but feature gated. See
      #11151 for more information. This includes both `macro_rules!` macros as
      well as syntax extensions such as `format!`.
    * New lint modes have been added, and older ones have been turned on to be
      warn-by-default.
      * Unnecessary parentheses
      * Uppercase statics
      * Camel Case types
      * Uppercase variables
      * Publicly visible private types
      * `#[deriving]` with raw pointers
    * Unsafe functions can no longer be coerced to closures.
    * Various obscure macros such as `log_syntax!` are now behind feature gates.
    * The `#[simd]` attribute is now behind a feature gate.
    * Visibility is no longer allowed on `extern crate` statements, and
      unnecessary visibility (`priv`) is no longer allowed on `use` statements.
    * Trailing commas are now allowed in argument lists and tuple patterns.
    * The `do` keyword has been removed, it is now a reserved keyword.
    * Default type parameters have been implemented, but are feature gated.
    * Borrowed variables through captures in closures are now considered soundly.
    * `extern mod` is now `extern crate`
    * The `Freeze` trait has been removed.
    * The `Share` trait has been added for types that can be shared among
      threads.
    * Labels in macros are now hygienic.
    * Expression/statement macro invocations can be delimited with `{}` now.
    * Treatment of types allowed in `static mut` locations has been tweaked.
    * The `*` and `.` operators are now overloadable through the `Deref` and
      `DerefMut` traits.
    * `~Trait` and `proc` no longer have `Send` bounds by default.
    * Partial type hints are now supported with the `_` type marker.
    * An `Unsafe` type was introduced for interior mutability. It is now
      considered undefined to transmute from `&T` to `&mut T` without using the
      `Unsafe` type.
    * The #[linkage] attribute was implemented for extern statics/functions.
    * The inner attribute syntax has changed from `#[foo];` to `#![foo]`.
    * `Pod` was renamed to `Copy`.

  * Libraries
    * The `libextra` library has been removed. It has now been decomposed into
      component libraries with smaller and more focused nuggets of
      functionality. The full list of libraries can be found on the
      documentation index page.
    * std: `std::condition` has been removed. All I/O errors are now propagated
      through the `Result` type. In order to assist with error handling, a
      `try!` macro for unwrapping errors with an early return and a lint for
      unused results has been added. See #12039 for more information.
    * std: The `vec` module has been renamed to `slice`.
    * std: A new vector type, `Vec<T>`, has been added in preparation for DST.
      This will become the only growable vector in the future.
    * std: `std::io` now has more public re-exports. Types such as `BufferedReader`
      are now found at `std::io::BufferedReader` instead of
      `std::io::buffered::BufferedReader`.
    * std: `print` and `println` are no longer in the prelude, the `print!` and
      `println!` macros are intended to be used instead.
    * std: `Rc` now has a `Weak` pointer for breaking cycles, and it no longer
      attempts to statically prevent cycles.
    * std: The standard distribution is adopting the policy of pushing failure
      to the user rather than failing in libraries. Many functions (such as
      `slice::last()`) now return `Option<T>` instead of `T` + failing.
    * std: `fmt::Default` has been renamed to `fmt::Show`, and it now has a new
      deriving mode: `#[deriving(Show)]`.
    * std: `ToStr` is now implemented for all types implementing `Show`.
    * std: The formatting trait methods now take `&self` instead of `&T`
    * std: The `invert()` method on iterators has been renamed to `rev()`
    * std: `std::num` has seen a reduction in the genericity of its traits,
      consolidating functionality into a few core traits.
    * std: Backtraces are now printed on task failure if the environment
      variable `RUST_BACKTRACE` is present.
    * std: Naming conventions for iterators have been standardized. More details
      can be found on the wiki's style guide.
    * std: `eof()` has been removed from the `Reader` trait. Specific types may
      still implement the function.
    * std: Networking types are now cloneable to allow simultaneous reads/writes.
    * std: `assert_approx_eq!` has been removed
    * std: The `e` and `E` formatting specifiers for floats have been added to
      print them in exponential notation.
    * std: The `Times` trait has been removed
    * std: Indications of variance and opting out of builtin bounds is done
      through marker types in `std::kinds::marker` now
    * std: `hash` has been rewritten, `IterBytes` has been removed, and
      `#[deriving(Hash)]` is now possible.
    * std: `SharedChan` has been removed, `Sender` is now cloneable.
    * std: `Chan` and `Port` were renamed to `Sender` and `Receiver`.
    * std: `Chan::new` is now `channel()`.
    * std: A new synchronous channel type has been implemented.
    * std: A `select!` macro is now provided for selecting over `Receiver`s.
    * std: `hashmap` and `trie` have been moved to `libcollections`
    * std: `run` has been rolled into `io::process`
    * std: `assert_eq!` now uses `{}` instead of `{:?}`
    * std: The equality and comparison traits have seen some reorganization.
    * std: `rand` has moved to `librand`.
    * std: `to_{lower,upper}case` has been implemented for `char`.
    * std: Logging has been moved to `liblog`.
    * collections: `HashMap` has been rewritten for higher performance and less
      memory usage.
    * native: The default runtime is now `libnative`. If `libgreen` is desired,
      it can be booted manually. The runtime guide has more information and
      examples.
    * native: All I/O functionality except signals has been implemented.
    * green: Task spawning with `libgreen` has been optimized with stack caching
      and various trimming of code.
    * green: Tasks spawned by `libgreen` now have an unmapped guard page.
    * sync: The `extra::sync` module has been updated to modern rust (and moved
      to the `sync` library), tweaking and improving various interfaces while
      dropping redundant functionality.
    * sync: A new `Barrier` type has been added to the `sync` library.
    * sync: An efficient mutex for native and green tasks has been implemented.
    * serialize: The `base64` module has seen some improvement. It treats
      newlines better, has non-string error values, and has seen general
      cleanup.
    * fourcc: A `fourcc!` macro was introduced
    * hexfloat: A `hexfloat!` macro was implemented for specifying floats via a
      hexadecimal literal.

  * Tooling
    * `rustpkg` has been deprecated and removed from the main repository. Its
      replacement, `cargo`, is under development.
    * Nightly builds of rust are now available
    * The memory usage of rustc has been improved many times throughout this
      release cycle.
    * The build process supports disabling rpath support for the rustc binary
      itself.
    * Code generation has improved in some cases, giving more information to the
      LLVM optimization passes to enable more extensive optimizations.
    * Debuginfo compatibility with lldb on OSX has been restored.
    * The master branch is now gated on an android bot, making building for
      android much more reliable.
    * Output flags have been centralized into one `--emit` flag.
    * Crate type flags have been centralized into one `--crate-type` flag.
    * Codegen flags have been consolidated behind a `-C` flag.
    * Linking against outdated crates now has improved error messages.
    * Error messages with lifetimes will often suggest how to annotate the
      function to fix the error.
    * Many more types are documented in the standard library, and new guides
      were written.
    * Many `rustdoc` improvements:
      * code blocks are syntax highlighted.
      * render standalone markdown files.
      * the --test flag tests all code blocks by default.
      * exported macros are displayed.
      * re-exported types have their documentation inlined at the location of the
        first re-export.
      * search works across crates that have been rendered to the same output
        directory.


Version 0.9 (2014-01-09)
==========================

   * ~1800 changes, numerous bugfixes

   * Language
      * The `float` type has been removed. Use `f32` or `f64` instead.
      * A new facility for enabling experimental features (feature gating) has
        been added, using the crate-level `#[feature(foo)]` attribute.
      * Managed boxes (@) are now behind a feature gate
        (`#[feature(managed_boxes)]`) in preparation for future removal. Use the
        standard library's `Gc` or `Rc` types instead.
      * `@mut` has been removed. Use `std::cell::{Cell, RefCell}` instead.
      * Jumping back to the top of a loop is now done with `continue` instead of
        `loop`.
      * Strings can no longer be mutated through index assignment.
      * Raw strings can be created via the basic `r"foo"` syntax or with matched
        hash delimiters, as in `r###"foo"###`.
      * `~fn` is now written `proc (args) -> retval { ... }` and may only be
        called once.
      * The `&fn` type is now written `|args| -> ret` to match the literal form.
      * `@fn`s have been removed.
      * `do` only works with procs in order to make it obvious what the cost
        of `do` is.
      * Single-element tuple-like structs can no longer be dereferenced to
        obtain the inner value. A more comprehensive solution for overloading
        the dereference operator will be provided in the future.
      * The `#[link(...)]` attribute has been replaced with
        `#[crate_id = "name#vers"]`.
      * Empty `impl`s must be terminated with empty braces and may not be
        terminated with a semicolon.
      * Keywords are no longer allowed as lifetime names; the `self` lifetime
        no longer has any special meaning.
      * The old `fmt!` string formatting macro has been removed.
      * `printf!` and `printfln!` (old-style formatting) removed in favor of
        `print!` and `println!`.
      * `mut` works in patterns now, as in `let (mut x, y) = (1, 2);`.
      * The `extern mod foo (name = "bar")` syntax has been removed. Use
        `extern mod foo = "bar"` instead.
      * New reserved keywords: `alignof`, `offsetof`, `sizeof`.
      * Macros can have attributes.
      * Macros can expand to items with attributes.
      * Macros can expand to multiple items.
      * The `asm!` macro is feature-gated (`#[feature(asm)]`).
      * Comments may be nested.
      * Values automatically coerce to trait objects they implement, without
        an explicit `as`.
      * Enum discriminants are no longer an entire word but as small as needed to
        contain all the variants. The `repr` attribute can be used to override
        the discriminant size, as in `#[repr(int)]` for integer-sized, and
        `#[repr(C)]` to match C enums.
      * Non-string literals are not allowed in attributes (they never worked).
      * The FFI now supports variadic functions.
      * Octal numeric literals, as in `0o7777`.
      * The `concat!` syntax extension performs compile-time string concatenation.
      * The `#[fixed_stack_segment]` and `#[rust_stack]` attributes have been
        removed as Rust no longer uses segmented stacks.
      * Non-ascii identifiers are feature-gated (`#[feature(non_ascii_idents)]`).
      * Ignoring all fields of an enum variant or tuple-struct is done with `..`,
        not `*`; ignoring remaining fields of a struct is also done with `..`,
        not `_`; ignoring a slice of a vector is done with `..`, not `.._`.
      * `rustc` supports the "win64" calling convention via `extern "win64"`.
      * `rustc` supports the "system" calling convention, which defaults to the
        preferred convention for the target platform, "stdcall" on 32-bit Windows,
        "C" elsewhere.
      * The `type_overflow` lint (default: warn) checks literals for overflow.
      * The `unsafe_block` lint (default: allow) checks for usage of `unsafe`.
      * The `attribute_usage` lint (default: warn) warns about unknown
        attributes.
      * The `unknown_features` lint (default: warn) warns about unknown
        feature gates.
      * The `dead_code` lint (default: warn) checks for dead code.
      * Rust libraries can be linked statically to one another
      * `#[link_args]` is behind the `link_args` feature gate.
      * Native libraries are now linked with `#[link(name = "foo")]`
      * Native libraries can be statically linked to a rust crate
        (`#[link(name = "foo", kind = "static")]`).
      * Native OS X frameworks are now officially supported
        (`#[link(name = "foo", kind = "framework")]`).
      * The `#[thread_local]` attribute creates thread-local (not task-local)
        variables. Currently behind the `thread_local` feature gate.
      * The `return` keyword may be used in closures.
      * Types that can be copied via a memcpy implement the `Pod` kind.
      * The `cfg` attribute can now be used on struct fields and enum variants.

   * Libraries
      * std: The `option` and `result` API's have been overhauled to make them
        simpler, more consistent, and more composable.
      * std: The entire `std::io` module has been replaced with one that is
        more comprehensive and that properly interfaces with the underlying
        scheduler. File, TCP, UDP, Unix sockets, pipes, and timers are all
        implemented.
      * std: `io::util` contains a number of useful implementations of
        `Reader` and `Writer`, including `NullReader`, `NullWriter`,
        `ZeroReader`, `TeeReader`.
      * std: The reference counted pointer type `extra::rc` moved into std.
      * std: The `Gc` type in the `gc` module will replace `@` (it is currently
        just a wrapper around it).
      * std: The `Either` type has been removed.
      * std: `fmt::Default` can be implemented for any type to provide default
        formatting to the `format!` macro, as in `format!("{}", myfoo)`.
      * std: The `rand` API continues to be tweaked.
      * std: The `rust_begin_unwind` function, useful for inserting breakpoints
        on failure in gdb, is now named `rust_fail`.
      * std: The `each_key` and `each_value` methods on `HashMap` have been
        replaced by the `keys` and `values` iterators.
      * std: Functions dealing with type size and alignment have moved from the
        `sys` module to the `mem` module.
      * std: The `path` module was written and API changed.
      * std: `str::from_utf8` has been changed to cast instead of allocate.
      * std: `starts_with` and `ends_with` methods added to vectors via the
        `ImmutableEqVector` trait, which is in the prelude.
      * std: Vectors can be indexed with the `get_opt` method, which returns `None`
        if the index is out of bounds.
      * std: Task failure no longer propagates between tasks, as the model was
        complex, expensive, and incompatible with thread-based tasks.
      * std: The `Any` type can be used for dynamic typing.
      * std: `~Any` can be passed to the `fail!` macro and retrieved via
        `task::try`.
      * std: Methods that produce iterators generally do not have an `_iter`
        suffix now.
      * std: `cell::Cell` and `cell::RefCell` can be used to introduce mutability
        roots (mutable fields, etc.). Use instead of e.g. `@mut`.
      * std: `util::ignore` renamed to `prelude::drop`.
      * std: Slices have `sort` and `sort_by` methods via the `MutableVector`
        trait.
      * std: `vec::raw` has seen a lot of cleanup and API changes.
      * std: The standard library no longer includes any C++ code, and very
        minimal C, eliminating the dependency on libstdc++.
      * std: Runtime scheduling and I/O functionality has been factored out into
        extensible interfaces and is now implemented by two different crates:
        libnative, for native threading and I/O; and libgreen, for green threading
        and I/O. This paves the way for using the standard library in more limited
        embedded environments.
      * std: The `comm` module has been rewritten to be much faster, have a
        simpler, more consistent API, and to work for both native and green
        threading.
      * std: All libuv dependencies have been moved into the rustuv crate.
      * native: New implementations of runtime scheduling on top of OS threads.
      * native: New native implementations of TCP, UDP, file I/O, process spawning,
        and other I/O.
      * green: The green thread scheduler and message passing types are almost
        entirely lock-free.
      * extra: The `flatpipes` module had bitrotted and was removed.
      * extra: All crypto functions have been removed and Rust now has a policy of
        not reimplementing crypto in the standard library. In the future crypto
        will be provided by external crates with bindings to established libraries.
      * extra: `c_vec` has been modernized.
      * extra: The `sort` module has been removed. Use the `sort` method on
        mutable slices.

   * Tooling
      * The `rust` and `rusti` commands have been removed, due to lack of
        maintenance.
      * `rustdoc` was completely rewritten.
      * `rustdoc` can test code examples in documentation.
      * `rustpkg` can test packages with the argument, 'test'.
      * `rustpkg` supports arbitrary dependencies, including C libraries.
      * `rustc`'s support for generating debug info is improved again.
      * `rustc` has better error reporting for unbalanced delimiters.
      * `rustc`'s JIT support was removed due to bitrot.
      * Executables and static libraries can be built with LTO (-Z lto)
      * `rustc` adds a `--dep-info` flag for communicating dependencies to
        build tools.


Version 0.8 (2013-09-26)
============================

   * ~2200 changes, numerous bugfixes

   * Language
      * The `for` loop syntax has changed to work with the `Iterator` trait.
      * At long last, unwinding works on Windows.
      * Default methods are ready for use.
      * Many trait inheritance bugs fixed.
      * Owned and borrowed trait objects work more reliably.
      * `copy` is no longer a keyword. It has been replaced by the `Clone` trait.
      * rustc can omit emission of code for the `debug!` macro if it is passed
        `--cfg ndebug`
      * mod.rs is now "blessed". When loading `mod foo;`, rustc will now look
        for foo.rs, then foo/mod.rs, and will generate an error when both are
        present.
      * Strings no longer contain trailing nulls. The new `std::c_str` module
        provides new mechanisms for converting to C strings.
      * The type of foreign functions is now `extern "C" fn` instead of `*u8'.
      * The FFI has been overhauled such that foreign functions are called directly,
        instead of through a stack-switching wrapper.
      * Calling a foreign function must be done through a Rust function with the
        `#[fixed_stack_segment]` attribute.
      * The `externfn!` macro can be used to declare both a foreign function and
        a `#[fixed_stack_segment]` wrapper at once.
      * `pub` and `priv` modifiers on `extern` blocks are no longer parsed.
      * `unsafe` is no longer allowed on extern fns - they are all unsafe.
      * `priv` is disallowed everywhere except for struct fields and enum variants.
      * `&T` (besides `&'static T`) is no longer allowed in `@T`.
      * `ref` bindings in irrefutable patterns work correctly now.
      * `char` is now prevented from containing invalid code points.
      * Casting to `bool` is no longer allowed.
      * `\0` is now accepted as an escape in chars and strings.
      * `yield` is a reserved keyword.
      * `typeof` is a reserved keyword.
      * Crates may be imported by URL with `extern mod foo = "url";`.
      * Explicit enum discriminants may be given as uints as in `enum E { V = 0u }`
      * Static vectors can be initialized with repeating elements,
        e.g. `static foo: [u8, .. 100]: [0, .. 100];`.
      * Static structs can be initialized with functional record update,
        e.g. `static foo: Foo = Foo { a: 5, .. bar };`.
      * `cfg!` can be used to conditionally execute code based on the crate
        configuration, similarly to `#[cfg(...)]`.
      * The `unnecessary_qualification` lint detects unneeded module
        prefixes (default: allow).
      * Arithmetic operations have been implemented on the SIMD types in
        `std::unstable::simd`.
      * Exchange allocation headers were removed, reducing memory usage.
      * `format!` implements a completely new, extensible, and higher-performance
        string formatting system. It will replace `fmt!`.
      * `print!` and `println!` write formatted strings (using the `format!`
        extension) to stdout.
      * `write!` and `writeln!` write formatted strings (using the `format!`
        extension) to the new Writers in `std::rt::io`.
      * The library section in which a function or static is placed may
        be specified with `#[link_section = "..."]`.
      * The `proto!` syntax extension for defining bounded message protocols
        was removed.
      * `macro_rules!` is hygienic for `let` declarations.
      * The `#[export_name]` attribute specifies the name of a symbol.
      * `unreachable!` can be used to indicate unreachable code, and fails
        if executed.

   * Libraries
      * std: Transitioned to the new runtime, written in Rust.
      * std: Added an experimental I/O library, `rt::io`, based on the new
        runtime.
      * std: A new generic `range` function was added to the prelude, replacing
        `uint::range` and friends.
      * std: `range_rev` no longer exists. Since range is an iterator it can be
        reversed with `range(lo, hi).invert()`.
      * std: The `chain` method on option renamed to `and_then`; `unwrap_or_default`
        renamed to `unwrap_or`.
      * std: The `iterator` module was renamed to `iter`.
      * std: Integral types now support the `checked_add`, `checked_sub`, and
        `checked_mul` operations for detecting overflow.
      * std: Many methods in `str`, `vec`, `option, `result` were renamed for
        consistency.
      * std: Methods are standardizing on conventions for casting methods:
        `to_foo` for copying, `into_foo` for moving, `as_foo` for temporary
        and cheap casts.
      * std: The `CString` type in `c_str` provides new ways to convert to and
        from C strings.
      * std: `DoubleEndedIterator` can yield elements in two directions.
      * std: The `mut_split` method on vectors partitions an `&mut [T]` into
        two splices.
      * std: `str::from_bytes` renamed to `str::from_utf8`.
      * std: `pop_opt` and `shift_opt` methods added to vectors.
      * std: The task-local data interface no longer uses @, and keys are
        no longer function pointers.
      * std: The `swap_unwrap` method of `Option` renamed to `take_unwrap`.
      * std: Added `SharedPort` to `comm`.
      * std: `Eq` has a default method for `ne`; only `eq` is required
        in implementations.
      * std: `Ord` has default methods for `le`, `gt` and `ge`; only `lt`
        is required in implementations.
      * std: `is_utf8` performance is improved, impacting many string functions.
      * std: `os::MemoryMap` provides cross-platform mmap.
      * std: `ptr::offset` is now unsafe, but also more optimized. Offsets that
        are not 'in-bounds' are considered undefined.
      * std: Many freestanding functions in `vec` removed in favor of methods.
      * std: Many freestanding functions on scalar types removed in favor of
        methods.
      * std: Many options to task builders were removed since they don't make
        sense in the new scheduler design.
      * std: More containers implement `FromIterator` so can be created by the
        `collect` method.
      * std: More complete atomic types in `unstable::atomics`.
      * std: `comm::PortSet` removed.
      * std: Mutating methods in the `Set` and `Map` traits have been moved into
        the `MutableSet` and `MutableMap` traits. `Container::is_empty`,
        `Map::contains_key`, `MutableMap::insert`, and `MutableMap::remove` have
        default implementations.
      * std: Various `from_str` functions were removed in favor of a generic
        `from_str` which is available in the prelude.
      * std: `util::unreachable` removed in favor of the `unreachable!` macro.
      * extra: `dlist`, the doubly-linked list was modernized.
      * extra: Added a `hex` module with `ToHex` and `FromHex` traits.
      * extra: Added `glob` module, replacing `std::os::glob`.
      * extra: `rope` was removed.
      * extra: `deque` was renamed to `ringbuf`. `RingBuf` implements `Deque`.
      * extra: `net`, and `timer` were removed. The experimental replacements
        are `std::rt::io::net` and `std::rt::io::timer`.
      * extra: Iterators implemented for `SmallIntMap`.
      * extra: Iterators implemented for `Bitv` and `BitvSet`.
      * extra: `SmallIntSet` removed. Use `BitvSet`.
      * extra: Performance of JSON parsing greatly improved.
      * extra: `semver` updated to SemVer 2.0.0.
      * extra: `term` handles more terminals correctly.
      * extra: `dbg` module removed.
      * extra: `par` module removed.
      * extra: `future` was cleaned up, with some method renames.
      * extra: Most free functions in `getopts` were converted to methods.

   * Other
      * rustc's debug info generation (`-Z debug-info`) is greatly improved.
      * rustc accepts `--target-cpu` to compile to a specific CPU architecture,
        similarly to gcc's `--march` flag.
      * rustc's performance compiling small crates is much better.
      * rustpkg has received many improvements.
      * rustpkg supports git tags as package IDs.
      * rustpkg builds into target-specific directories so it can be used for
        cross-compiling.
      * The number of concurrent test tasks is controlled by the environment
        variable RUST_TEST_TASKS.
      * The test harness can now report metrics for benchmarks.
      * All tools have man pages.
      * Programs compiled with `--test` now support the `-h` and `--help` flags.
      * The runtime uses jemalloc for allocations.
      * Segmented stacks are temporarily disabled as part of the transition to
        the new runtime. Stack overflows are possible!
      * A new documentation backend, rustdoc_ng, is available for use. It is
        still invoked through the normal `rustdoc` command.


Version 0.7 (2013-07-03)
=======================

   * ~2000 changes, numerous bugfixes

   * Language
      * `impl`s no longer accept a visibility qualifier. Put them on methods
        instead.
      * The borrow checker has been rewritten with flow-sensitivity, fixing
        many bugs and inconveniences.
      * The `self` parameter no longer implicitly means `&'self self`,
        and can be explicitly marked with a lifetime.
      * Overloadable compound operators (`+=`, etc.) have been temporarily
        removed due to bugs.
      * The `for` loop protocol now requires `for`-iterators to return `bool`
        so they compose better.
      * The `Durable` trait is replaced with the `'static` bounds.
      * Trait default methods work more often.
      * Structs with the `#[packed]` attribute have byte alignment and
        no padding between fields.
      * Type parameters bound by `Copy` must now be copied explicitly with
        the `copy` keyword.
      * It is now illegal to move out of a dereferenced unsafe pointer.
      * `Option<~T>` is now represented as a nullable pointer.
      * `@mut` does dynamic borrow checks correctly.
      * The `main` function is only detected at the topmost level of the crate.
        The `#[main]` attribute is still valid anywhere.
      * Struct fields may no longer be mutable. Use inherited mutability.
      * The `#[no_send]` attribute makes a type that would otherwise be
        `Send`, not.
      * The `#[no_freeze]` attribute makes a type that would otherwise be
        `Freeze`, not.
      * Unbounded recursion will abort the process after reaching the limit
        specified by the `RUST_MAX_STACK` environment variable (default: 1GB).
      * The `vecs_implicitly_copyable` lint mode has been removed. Vectors
        are never implicitly copyable.
      * `#[static_assert]` makes compile-time assertions about static bools.
      * At long last, 'argument modes' no longer exist.
      * The rarely used `use mod` statement no longer exists.

   * Syntax extensions
      * `fail!` and `assert!` accept `~str`, `&'static str` or `fmt!`-style
        argument list.
      * `Encodable`, `Decodable`, `Ord`, `TotalOrd`, `TotalEq`, `DeepClone`,
        `Rand`, `Zero` and `ToStr` can all be automatically derived with
        `#[deriving(...)]`.
      * The `bytes!` macro returns a vector of bytes for string, u8, char,
        and unsuffixed integer literals.

   * Libraries
      * The `core` crate was renamed to `std`.
      * The `std` crate was renamed to `extra`.
      * More and improved documentation.
      * std: `iterator` module for external iterator objects.
      * Many old-style (internal, higher-order function) iterators replaced by
        implementations of `Iterator`.
      * std: Many old internal vector and string iterators,
        incl. `any`, `all`. removed.
      * std: The `finalize` method of `Drop` renamed to `drop`.
      * std: The `drop` method now takes `&mut self` instead of `&self`.
      * std: The prelude no longer re-exports any modules, only types and traits.
      * std: Prelude additions: `print`, `println`, `FromStr`, `ApproxEq`, `Equiv`,
        `Iterator`, `IteratorUtil`, many numeric traits, many tuple traits.
      * std: New numeric traits: `Fractional`, `Real`, `RealExt`, `Integer`, `Ratio`,
        `Algebraic`, `Trigonometric`, `Exponential`, `Primitive`.
      * std: Tuple traits and accessors defined for up to 12-tuples, e.g.
        `(0, 1, 2).n2()` or `(0, 1, 2).n2_ref()`.
      * std: Many types implement `Clone`.
      * std: `path` type renamed to `Path`.
      * std: `mut` module and `Mut` type removed.
      * std: Many standalone functions removed in favor of methods and iterators
        in `vec`, `str`. In the future methods will also work as functions.
      * std: `reinterpret_cast` removed. Use `transmute`.
      * std: ascii string handling in `std::ascii`.
      * std: `Rand` is implemented for ~/@.
      * std: `run` module for spawning processes overhauled.
      * std: Various atomic types added to `unstable::atomic`.
      * std: Various types implement `Zero`.
      * std: `LinearMap` and `LinearSet` renamed to `HashMap` and `HashSet`.
      * std: Borrowed pointer functions moved from `ptr` to `borrow`.
      * std: Added `os::mkdir_recursive`.
      * std: Added `os::glob` function performs filesystems globs.
      * std: `FuzzyEq` renamed to `ApproxEq`.
      * std: `Map` now defines `pop` and `swap` methods.
      * std: `Cell` constructors converted to static methods.
      * extra: `rc` module adds the reference counted pointers, `Rc` and `RcMut`.
      * extra: `flate` module moved from `std` to `extra`.
      * extra: `fileinput` module for iterating over a series of files.
      * extra: `Complex` number type and `complex` module.
      * extra: `Rational` number type and `rational` module.
      * extra: `BigInt`, `BigUint` implement numeric and comparison traits.
      * extra: `term` uses terminfo now, is more correct.
      * extra: `arc` functions converted to methods.
      * extra: Implementation of fixed output size variations of SHA-2.

   * Tooling
      * `unused_variable`  lint mode for unused variables (default: warn).
      * `unused_unsafe` lint mode for detecting unnecessary `unsafe` blocks
        (default: warn).
      * `unused_mut` lint mode for identifying unused `mut` qualifiers
        (default: warn).
      * `dead_assignment` lint mode for unread variables (default: warn).
      * `unnecessary_allocation` lint mode detects some heap allocations that are
        immediately borrowed so could be written without allocating (default: warn).
      * `missing_doc` lint mode (default: allow).
      * `unreachable_code` lint mode (default: warn).
      * The `rusti` command has been rewritten and a number of bugs addressed.
      * rustc outputs in color on more terminals.
      * rustc accepts a `--link-args` flag to pass arguments to the linker.
      * rustc accepts a `-Z print-link-args` flag for debugging linkage.
      * Compiling with `-g` will make the binary record information about
        dynamic borrowcheck failures for debugging.
      * rustdoc has a nicer stylesheet.
      * Various improvements to rustdoc.
      * Improvements to rustpkg (see the detailed release notes).


Version 0.6 (2013-04-03)
========================

   * ~2100 changes, numerous bugfixes

   * Syntax changes
      * The self type parameter in traits is now spelled `Self`
      * The `self` parameter in trait and impl methods must now be explicitly
        named (for example: `fn f(&self) { }`). Implicit self is deprecated.
      * Static methods no longer require the `static` keyword and instead
        are distinguished by the lack of a `self` parameter
      * Replaced the `Durable` trait with the `'static` lifetime
      * The old closure type syntax with the trailing sigil has been
        removed in favor of the more consistent leading sigil
      * `super` is a keyword, and may be prefixed to paths
      * Trait bounds are separated with `+` instead of whitespace
      * Traits are implemented with `impl Trait for Type`
        instead of `impl Type: Trait`
      * Lifetime syntax is now `&'l foo` instead of `&l/foo`
      * The `export` keyword has finally been removed
      * The `move` keyword has been removed (see "Semantic changes")
      * The interior mutability qualifier on vectors, `[mut T]`, has been
        removed. Use `&mut [T]`, etc.
      * `mut` is no longer valid in `~mut T`. Use inherited mutability
      * `fail` is no longer a keyword. Use `fail!()`
      * `assert` is no longer a keyword. Use `assert!()`
      * `log` is no longer a keyword. use `debug!`, etc.
      * 1-tuples may be represented as `(T,)`
      * Struct fields may no longer be `mut`. Use inherited mutability,
        `@mut T`, `core::mut` or `core::cell`
      * `extern mod { ... }` is no longer valid syntax for foreign
        function modules. Use extern blocks: `extern { ... }`
      * Newtype enums removed. Use tuple-structs.
      * Trait implementations no longer support visibility modifiers
      * Pattern matching over vectors improved and expanded
      * `const` renamed to `static` to correspond to lifetime name,
        and make room for future `static mut` unsafe mutable globals.
      * Replaced `#[deriving_eq]` with `#[deriving(Eq)]`, etc.
      * `Clone` implementations can be automatically generated with
        `#[deriving(Clone)]`
      * Casts to traits must use a pointer sigil, e.g. `@foo as @Bar`
        instead of `foo as Bar`.
      * Fixed length vector types are now written as `[int, .. 3]`
        instead of `[int * 3]`.
      * Fixed length vector types can express the length as a constant
        expression. (ex: `[int, .. GL_BUFFER_SIZE - 2]`)

   * Semantic changes
      * Types with owned pointers or custom destructors move by default,
        eliminating the `move` keyword
      * All foreign functions are considered unsafe
      * &mut is now unaliasable
      * Writes to borrowed @mut pointers are prevented dynamically
      * () has size 0
      * The name of the main function can be customized using #[main]
      * The default type of an inferred closure is &fn instead of @fn
      * `use` statements may no longer be "chained" - they cannot import
        identifiers imported by previous `use` statements
      * `use` statements are crate relative, importing from the "top"
        of the crate by default. Paths may be prefixed with `super::`
        or `self::` to change the search behavior.
      * Method visibility is inherited from the implementation declaration
      * Structural records have been removed
      * Many more types can be used in static items, including enums
        'static-lifetime pointers and vectors
      * Pattern matching over vectors improved and expanded
      * Typechecking of closure types has been overhauled to
        improve inference and eliminate unsoundness
      * Macros leave scope at the end of modules, unless that module is
        tagged with #[macro_escape]

   * Libraries
      * Added big integers to `std::bigint`
      * Removed `core::oldcomm` module
      * Added pipe-based `core::comm` module
      * Numeric traits have been reorganized under `core::num`
      * `vec::slice` finally returns a slice
      * `debug!` and friends don't require a format string, e.g. `debug!(Foo)`
      * Containers reorganized around traits in `core::container`
      * `core::dvec` removed, `~[T]` is a drop-in replacement
      * `core::send_map` renamed to `core::hashmap`
      * `std::map` removed; replaced with `core::hashmap`
      * `std::treemap` reimplemented as an owned balanced tree
      * `std::deque` and `std::smallintmap` reimplemented as owned containers
      * `core::trie` added as a fast ordered map for integer keys
      * Set types added to `core::hashmap`, `core::trie` and `std::treemap`
      * `Ord` split into `Ord` and `TotalOrd`. `Ord` is still used to
        overload the comparison operators, whereas `TotalOrd` is used
        by certain container types

   * Other
      * Replaced the 'cargo' package manager with 'rustpkg'
      * Added all-purpose 'rust' tool
      * `rustc --test` now supports benchmarks with the `#[bench]` attribute
      * rustc now *attempts* to offer spelling suggestions
      * Improved support for ARM and Android
      * Preliminary MIPS backend
      * Improved foreign function ABI implementation for x86, x86_64
      * Various memory usage improvements
      * Rust code may be embedded in foreign code under limited circumstances
      * Inline assembler supported by new asm!() syntax extension.


Version 0.5 (2012-12-21)
===========================

   * ~900 changes, numerous bugfixes

   * Syntax changes
      * Removed `<-` move operator
      * Completed the transition from the `#fmt` extension syntax to `fmt!`
      * Removed old fixed length vector syntax - `[T]/N`
      * New token-based quasi-quoters, `quote_tokens!`, `quote_expr!`, etc.
      * Macros may now expand to items and statements
      * `a.b()` is always parsed as a method call, never as a field projection
      * `Eq` and `IterBytes` implementations can be automatically generated
        with `#[deriving_eq]` and `#[deriving_iter_bytes]` respectively
      * Removed the special crate language for `.rc` files
      * Function arguments may consist of any irrefutable pattern

   * Semantic changes
      * `&` and `~` pointers may point to objects
      * Tuple structs - `struct Foo(Bar, Baz)`. Will replace newtype enums.
      * Enum variants may be structs
      * Destructors can be added to all nominal types with the Drop trait
      * Structs and nullary enum variants may be constants
      * Values that cannot be implicitly copied are now automatically moved
        without writing `move` explicitly
      * `&T` may now be coerced to `*T`
      * Coercions happen in `let` statements as well as function calls
      * `use` statements now take crate-relative paths
      * The module and type namespaces have been merged so that static
        method names can be resolved under the trait in which they are
        declared

   * Improved support for language features
      * Trait inheritance works in many scenarios
      * More support for explicit self arguments in methods - `self`, `&self`
        `@self`, and `~self` all generally work as expected
      * Static methods work in more situations
      * Experimental: Traits may declare default methods for the implementations
        to use

   * Libraries
      * New condition handling system in `core::condition`
      * Timsort added to `std::sort`
      * New priority queue, `std::priority_queue`
      * Pipes for serializable types, `std::flatpipes'
      * Serialization overhauled to be trait-based
      * Expanded `getopts` definitions
      * Moved futures to `std`
      * More functions are pure now
      * `core::comm` renamed to `oldcomm`. Still deprecated
      * `rustdoc` and `cargo` are libraries now

   * Misc
      * Added a preliminary REPL, `rusti`
      * License changed from MIT to dual MIT/APL2


Version 0.4 (2012-10-15)
==========================

   * ~2000 changes, numerous bugfixes

   * Syntax
      * All keywords are now strict and may not be used as identifiers anywhere
      * Keyword removal: 'again', 'import', 'check', 'new', 'owned', 'send',
        'of', 'with', 'to', 'class'.
      * Classes are replaced with simpler structs
      * Explicit method self types
      * `ret` became `return` and `alt` became `match`
      * `import` is now `use`; `use is now `extern mod`
      * `extern mod { ... }` is now `extern { ... }`
      * `use mod` is the recommended way to import modules
      * `pub` and `priv` replace deprecated export lists
      * The syntax of `match` pattern arms now uses fat arrow (=>)
      * `main` no longer accepts an args vector; use `os::args` instead

   * Semantics
      * Trait implementations are now coherent, ala Haskell typeclasses
      * Trait methods may be static
      * Argument modes are deprecated
      * Borrowed pointers are much more mature and recommended for use
      * Strings and vectors in the static region are stored in constant memory
      * Typestate was removed
      * Resolution rewritten to be more reliable
      * Support for 'dual-mode' data structures (freezing and thawing)

   * Libraries
      * Most binary operators can now be overloaded via the traits in
        `core::ops'
      * `std::net::url` for representing URLs
      * Sendable hash maps in `core::send_map`
      * `core::task' gained a (currently unsafe) task-local storage API

   * Concurrency
      * An efficient new intertask communication primitive called the pipe,
        along with a number of higher-level channel types, in `core::pipes`
      * `std::arc`, an atomically reference counted, immutable, shared memory
        type
      * `std::sync`, various exotic synchronization tools based on arcs and pipes
      * Futures are now based on pipes and sendable
      * More robust linked task failure
      * Improved task builder API

   * Other
      * Improved error reporting
      * Preliminary JIT support
      * Preliminary work on precise GC
      * Extensive architectural improvements to rustc
      * Begun a transition away from buggy C++-based reflection (shape) code to
        Rust-based (visitor) code
      * All hash functions and tables converted to secure, randomized SipHash


Version 0.3  (2012-07-12)
========================

   * ~1900 changes, numerous bugfixes

   * New coding conveniences
      * Integer-literal suffix inference
      * Per-item control over warnings, errors
      * #[cfg(windows)] and #[cfg(unix)] attributes
      * Documentation comments
      * More compact closure syntax
      * 'do' expressions for treating higher-order functions as
        control structures
      * *-patterns (wildcard extended to all constructor fields)

   * Semantic cleanup
      * Name resolution pass and exhaustiveness checker rewritten
      * Region pointers and borrow checking supersede alias
        analysis
      * Init-ness checking is now provided by a region-based liveness
        pass instead of the typestate pass; same for last-use analysis
      * Extensive work on region pointers

   * Experimental new language features
      * Slices and fixed-size, interior-allocated vectors
      * #!-comments for lang versioning, shell execution
      * Destructors and iface implementation for classes;
        type-parameterized classes and class methods
      * 'const' type kind for types that can be used to implement
        shared-memory concurrency patterns

   * Type reflection

   * Removal of various obsolete features
      * Keywords: 'be', 'prove', 'syntax', 'note', 'mutable', 'bind',
                 'crust', 'native' (now 'extern'), 'cont' (now 'again')

      * Constructs: do-while loops ('do' repurposed), fn binding,
                    resources (replaced by destructors)

   * Compiler reorganization
      * Syntax-layer of compiler split into separate crate
      * Clang (from LLVM project) integrated into build
      * Typechecker split into sub-modules

   * New library code
      * New time functions
      * Extension methods for many built-in types
      * Arc: atomic-refcount read-only / exclusive-use shared cells
      * Par: parallel map and search routines
      * Extensive work on libuv interface
      * Much vector code moved to libraries
      * Syntax extensions: #line, #col, #file, #mod, #stringify,
        #include, #include_str, #include_bin

   * Tool improvements
      * Cargo automatically resolves dependencies


Version 0.2  (2012-03-29)
=========================

   * >1500 changes, numerous bugfixes

   * New docs and doc tooling

   * New port: FreeBSD x86_64

   * Compilation model enhancements
      * Generics now specialized, multiply instantiated
      * Functions now inlined across separate crates

   * Scheduling, stack and threading fixes
      * Noticeably improved message-passing performance
      * Explicit schedulers
      * Callbacks from C
      * Helgrind clean

   * Experimental new language features
      * Operator overloading
      * Region pointers
      * Classes

   * Various language extensions
      * C-callback function types: 'crust fn ...'
      * Infinite-loop construct: 'loop { ... }'
      * Shorten 'mutable' to 'mut'
      * Required mutable-local qualifier: 'let mut ...'
      * Basic glob-exporting: 'export foo::*;'
      * Alt now exhaustive, 'alt check' for runtime-checked
      * Block-function form of 'for' loop, with 'break' and 'ret'.

   * New library code
      * AST quasi-quote syntax extension
      * Revived libuv interface
      * New modules: core::{future, iter}, std::arena
      * Merged per-platform std::{os*, fs*} to core::{libc, os}
      * Extensive cleanup, regularization in libstd, libcore


Version 0.1  (2012-01-20)
===============================

   * Most language features work, including:
      * Unique pointers, unique closures, move semantics
      * Interface-constrained generics
      * Static interface dispatch
      * Stack growth
      * Multithread task scheduling
      * Typestate predicates
      * Failure unwinding, destructors
      * Pattern matching and destructuring assignment
      * Lightweight block-lambda syntax
      * Preliminary macro-by-example

   * Compiler works with the following configurations:
      * Linux: x86 and x86_64 hosts and targets
      * macOS: x86 and x86_64 hosts and targets
      * Windows: x86 hosts and targets

   * Cross compilation / multi-target configuration supported.

   * Preliminary API-documentation and package-management tools included.

Known issues:

   * Documentation is incomplete.

   * Performance is below intended target.

   * Standard library APIs are subject to extensive change, reorganization.

   * Language-level versioning is not yet operational - future code will
     break unexpectedly.
