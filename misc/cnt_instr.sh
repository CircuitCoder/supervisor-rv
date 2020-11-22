#!/bin/sh

# How to use: Within /kernel
# $ riscv64-unknown-elf-objdump -S -Mno-aliases kernel.elf | ../misc/cnt_instr.sh


cat - \
  | grep -oE "^[0-9A-Fa-f]+:\s*[0-9A-Fa-f]+\s*(\S+)(\s|$)" \
  | awk '{ print $3 }' \
  | sort | uniq -c | sort -n
