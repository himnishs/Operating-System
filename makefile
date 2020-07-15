GCCPARAMS = -m64 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386


objects = loader.o kernel.o

%.o: %.cpp
	g++ $(GCCPARAMS) -o $@ -c $<

%.o: %.s
	as -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld -m elf_i386 $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin
.PHONY: clean
clean:
	rm -rf obj kernel.o loader.o