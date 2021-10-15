#!/bin/bash
bin/x86_64-pc-linux-gnu-gcc.exe -g -O -o hello.com.dbg hello.c                       \
  -static -fno-pie -no-pie -mno-red-zone -fno-omit-frame-pointer -nostdlib -nostdinc \
  -Wl,--gc-sections -Wl,-z,max-page-size=0x1000 -fuse-ld=bfd                         \
  -Wl,-T,ape.lds -include cosmopolitan.h crt.o ape.o cosmopolitan.a
bin/x86_64-pc-linux-gnu-objcopy.exe -S -O binary hello.com.dbg hello.com
