Change Log
==========

All notable changes to this project will be documented in this file. This
project adheres to [Semantic Versioning](http://semver.org/).

## 2.1.2
- Fix bug around getting `use` context #906
- Update rustc-ap-syntax to fix build in current nightly #911

## 2.1.1
- Fix coordinate bug
- Get doc string for macro #905

## 2.1.0
- Support completions for stdlib macros #902
- Support extern "~"  block #895
- Support `crate_in_paths` #891
- Fix bug of getting completion context from `use` statement #886
- Handle const unsafe fn #879
- Limit recursion depth through glob imports #875
- Enable completion based on trait bound for function args #871
- Fix bug in search_closure_args #862
- Replace cargo.rs with cargo crate #855
- Migrate over to rustc_ap_syntax #854
- Make RUST_SRC_PATH optional #808
- Refactor based on clippy #860

## 2.0.14
- Cache generic impls #839
- Cache parsed TOML file and cargo crate roots #838
- Skip `pub` keyword as a temporary fix for #624 #850
- Remove complex generic type by impl trait #848
- Fix bug for array expression #841
- Support completion for enum variants without type annotation #825
- Fix bug for raw string #822

## 2.0.13
- Fix bug for finding the start of match statement #819

## 2.0.12
- Fix bug that broke completions in previous release #807

## 2.0.11

- Use `rustup` to find libstd path even when used as library #799

## 2.0.10

- Support resolving `use as` aliases declared in multi-element `use` statements #753
- Provide suggestions for global paths in more cases #765
- Suggestions imported via `use as` statements now return their in-scope alias as the match string #767
- Add new commands for converting between points and coordinates in files #776
- Return fewer duplicate suggestions #778
- Handle cases where mod names and trait methods collide, such as `fmt` #781

## 2.0.9

- Support completion after using try operator `?` #726
- Find methods on cooked string literals #728
- Fix bug caused by closure completions feature #734
- Find static methods on enums #737
- Find doc comments on named and indexed struct fields #739
- Find `pub(restricted)` items #748

## 2.0.8

- Fix bug finding definitions where impl contains bang #717
- Find definition for closures #697
- Resolve types for tuple struct fields #722
- Resolve types for let patterns #724
- Fix completions for reference fields #723

## 2.0.7

- Fix panic with macros called `impl*` #701
- Relax semver specs

## 2.0.6

- resolve Self (e.g. in-impl function calls like Self::myfunction())
- Fix stack overflow issue on unresolvable imports :tada: #698

## 2.0.5

- Chained completions on separate lines now work #686

## 2.0.4

- Fix for find-doc not always returning full doc string #675

## 2.0.3

- Fix for recursion in certain `use foo::{self, ..}` cases #669

## 2.0.2

- Internal fixes so we can publish on crates.io

## 2.0.1

- Syntex 0.52 #643

- Fix `racer --help` bug from 2.0 refactor #662

- Support short revision identifiers for git checkout paths #664

- Handle self resolution when using `use mymod::{self, Thing}` #665

- Fix type alias resolution #666

## 2.0

- Rework public API to hide many implementation details and allow the project to
  move forward without breaking changes.

- Many fixes that didn't make it into the changelog, but we're going to work on
  that in the future!

## 1.2

- Added basic 'daemon' mode, racer process can be kept running between
  invocations

- now uses clap to parse command line options

- Adds caching of file source and code indices

- Adds an alternative 'tabbed' mode where inputs and outputs can be tab
  separated for easier parsing

- emacs and vim support split out into their own git projects [emacs-racer] and
  [vim-racer], respectively.

- Fix issue resolving some `std::*` modules in latest rust source: (rust std lib
  implicitly imports core with `#![no_std]`)

- Searches multirust overrides when locating cargo src directories

## 1.0.0 2015-07-29

- First release

[vim-racer]: https://github.com/racer-rust/vim-racer
[emacs-racer]: https://github.com/racer-rust/emacs-racer
