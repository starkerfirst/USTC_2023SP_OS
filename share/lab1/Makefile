ASM_FLAGS= -m32 --pipe -Wall -fasm -g -O1 -fno-stack-protector
multibootHeader.bin: multibootHeader.s
	gcc -c $(ASM_FLAGS) multibootHeader.s -o multibootHeader.o
	ld -n -T multibootHeader.ld multibootHeader.o -o multibootHeader.bin
clean:
	rm -rf ./multibootHeader.bin ./multibootHeader.o