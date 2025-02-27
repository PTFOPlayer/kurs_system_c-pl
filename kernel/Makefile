kernel := build/kernel.bin
iso := build/os.iso

linker_script := src/linker.ld
grub_cfg := src/grub.cfg

assembly_source_files := $(wildcard src/*.asm)
assembly_object_files := $(patsubst src/%.asm, \
	build/%.o, $(assembly_source_files))

c_source_files := $(wildcard src/*.c)
c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))

CFLAGS = -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
             -nostartfiles -nodefaultlibs -c
			 
.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -m 128M -cdrom $(iso) -serial stdio

run_debug: $(iso)
	@qemu-system-x86_64 -m 128M -no-shutdown -no-reboot -d cpu_reset -cdrom $(iso) -serial stdio

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub2-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	@ld -n -T $(linker_script) -o $(kernel) $(wildcard build/*.o)

build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

build/%.o: src/%.c
	@gcc $(CFLAGS) -mno-red-zone $(c_source_files) -o $(c_object_files)