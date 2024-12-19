#!/bin/sh

curl  https://static.rust-lang.org/dist/rustc-1.29.0-src.tar.gz > rustc-1.29.0-src.tar.gz
gzip -dc rustc-1.29.0-src.tar.gz | tar xvf -
make bin/mrustc
make -f minicargo.mk bin/minicargo
make -f minicargo.mk output/libcore.rlib
