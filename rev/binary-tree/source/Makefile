all: build

build: main.elf

%.elf: %.o
	ld --omagic --strip-all $< -o $@

%.o: %.asm last.bin
	nasm -Wall -felf64 $<
	rm last.bin code.bin

last.bin:
	./gen.py

clean:
	rm -rf *.bin *.o *.elf