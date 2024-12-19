#!/bin/sh

make bin/mrustc
make -f minicargo.mk bin/minicargo
make -f minicargo.mk output/libcore.rlib
