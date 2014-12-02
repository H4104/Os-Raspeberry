#!/bin/sh

arm-none-eabi-gdb ../kernel.elf -x gdbinit

reset
