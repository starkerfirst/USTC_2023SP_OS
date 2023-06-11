#!/bin/sh

make clean

make

if [ $? -ne 0 ]; then
	echo "make failed"
else
	echo "make succeed"
	qemu-system-i386 -s -S -d int -kernel output/myOS.elf -serial pty  &
fi
