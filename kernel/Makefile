build_path := ./build
kernel := $(build_path)/kernel.bin
hdd := $(build_path)/image.hdd

linker_script := src/linker.ld
limine_path := ../limine

c_source_files := $(shell find ./src -name '*.c')
cpp_source_files := $(shell find ./src -name '*.cpp')

asm_source_files := $(shell find ./src -name '*.asm')

override CFLAGS = -g -O1 -pipe\
	-Wall \
    -Wextra \
    -ffreestanding \
	-fno-use-cxa-atexit \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel

override LDFLAGS =      -Wl,-m,elf_x86_64 \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -T $(linker_script)

.PHONY: all clean run run_debug

all: $(kernel)

clean: 
	rm -r build/*

$(hdd): $(kernel)
	@rm $(hdd)
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$(hdd)
	@sgdisk $(hdd) -n 1:2048 -t 1:ef00 -m 1
	@../limine/limine bios-install $(hdd)
	@mformat -i $(hdd)@@1M
	@mmd -i $(hdd)@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	@mcopy -i $(hdd)@@1M $(kernel) ::/boot
	@mcopy -i $(hdd)@@1M src/limine.conf $(limine_path)/limine-bios.sys ::/boot/limine
	@mcopy -i $(hdd)@@1M $(limine_path)/BOOTX64.EFI ::/EFI/BOOT
	@mcopy -i $(hdd)@@1M $(limine_path)/BOOTIA32.EFI ::/EFI/BOOT

$(kernel): $(asm_source_files) $(c_source_files) $(cpp_source_files)
	@nasm -f elf64 $(asm_source_files) -o $(build_path)/asm.o
	@g++ $(CFLAGS) $(LDFLAGS) $(build_path)/asm.o $(c_source_files) $(cpp_source_files) -o $(kernel)

run: $(hdd)
	@qemu-system-x86_64 \
	-m 256M \
	-bios ./../ovmf/ovmf-code-x86_64.fd \
	-drive file=$(hdd),format=raw

run_debug: $(hdd)
	@qemu-system-x86_64 \
	-m 256M \
	-bios ./../ovmf/ovmf-code-x86_64.fd \
	-drive file=$(hdd),format=raw \
	-no-shutdown -no-reboot -d int,cpu_reset,guest_errors

