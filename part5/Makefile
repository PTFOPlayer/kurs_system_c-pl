arch ?= x86_64
kernel := build/kernel.bin
iso := build/os.iso

linker_script := src/linker.ld
grub_cfg := src/grub.cfg
assembly_source_files := $(wildcard src/*.asm)
assembly_object_files := $(patsubst src/%.asm, \
	build/%.o, $(assembly_source_files))

c_source_files := $(wildcard src/c/*.c)
c_object_files := $(patsubst src/c/%.c, \
	build/%.o, $(c_source_files))

obj_files := $(wildcard build/*.o)

CFLAGS = -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
             -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c 
.PHONY: all clean run run_kvm iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

run_kvm: $(iso)
	@qemu-system-x86_64 --enable-kvm -cpu host -cdrom $(iso)


iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub2-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	@ld --no-warn-rwx-segment -n -T $(linker_script) -o $(kernel) $(obj_files)

# compile assembly files
build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

build/%.o: src/c/%.c
	@gcc $(CFLAGS)  $(c_source_files) -o $(c_object_files)