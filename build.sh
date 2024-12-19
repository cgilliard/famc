#!/bin/sh


if [ ! -d rustc-1.29.0-src ]; then
  curl -s https://static.rust-lang.org/dist/rustc-1.29.0-src.tar.gz | tar -xzf -
fi

make bin/mrustc
make -f minicargo.mk bin/minicargo
make -f minicargo.mk output/libcore.rlib
