#!/bin/bash
set -e
export RUSTC_VERSION=1.29.0 MRUSTC_TARGET_VER=1.29 OUTDIR_SUF=-1.29.0
make
ln -s `pwd`/bin/famc `pwd`/bin/mrustc
make -f minicargo.mk RUSTCSRC $@
make -f minicargo.mk LIBS $@
