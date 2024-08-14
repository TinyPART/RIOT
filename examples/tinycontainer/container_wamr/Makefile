# TinyContainer
#
#  Copyright (C) 2023-2024 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

CC=CLANG

CCFLAGS += -Wall
CCFLAGS += --target=wasm32-unknown-unknown-wasm
CCFLAGS += -emit-llvm
CCFLAGS += -Os
CCFLAGS += -flto
CCFLAGS += -fvisibility=hidden
CCFLAGS += -ffunction-sections
CCFLAGS += -fdata-sections

LDFLAGS += -z stack-size=128
LDFLAGS += --global-base=16
LDFLAGS += --export main
LDFLAGS += --export=__heap_base
LDFLAGS += --export=__data_end
LDFLAGS += --allow-undefined
LDFLAGS += --strip-all
LDFLAGS += --export-dynamic
LDFLAGS += -error-limit=0
LDFLAGS += --lto-O3
LDFLAGS += -O3
LDFLAGS += --gc-sections
LDFLAGS += --initial-memory=65536
LDFLAGS += --no-entry

all: simple_container_wasm.h

simple_container_wasm.h: simple_container.wasm
	xxd -i simple_container.wasm simple_container_wasm.h

simple_container.wasm: simple_container.o
	wasm-ld ${LDFLAGS} -o simple_container.wasm simple_container.o

simple_container.o: simple_container.c
	clang -c ${CCFLAGS} -o simple_container.o simple_container.c

.SILENT: clean

clean:
	rm -f simple_container_wasm.h
	rm -f simple_container.wasm
	rm -f simple_container.o
