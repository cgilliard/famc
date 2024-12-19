#!/bin/sh

RUSTC_SRC="rustc-`cat rust-version`-src";
if [ ! -d ${RUSTC_SRC} ]; then
  curl -s https://static.rust-lang.org/dist/${RUSTC_SRC}.tar.gz | tar -xzf -
fi

make bin/mrustc
make -f minicargo.mk bin/minicargo
make -f minicargo.mk output/libcore.rlib
