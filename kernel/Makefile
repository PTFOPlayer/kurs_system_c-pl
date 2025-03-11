build_path := ./build
kernel := $(build_path)/kernel.bin
iso := $(build_path)/os.iso

linker_script := src/linker.ld
limine_path := ../limine

c_source_files := $(shell find ./src -name '*.c')
cpp_source_files := $(shell find ./src -name '*.cpp')

asm_source_files := $(shell find ./src -name '*.asm')

override CFLAGS = -g -O1 -pipe\
	-Wall \
    -Wextra \
    -ffreestanding \
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

.PHONY: clean all kernel run iso 

all: $(kernel)

clean: 
	rm -r build/

$(iso): $(kernel)
	@mkdir -p $(build_path)/iso
	@mkdir -p $(build_path)/iso/boot
	@mkdir -p $(build_path)/iso/boot/limine
	@mkdir -p $(build_path)/iso/EFI/BOOT
	@cp -v $(kernel) $(build_path)/iso/boot
	@cp -v src/limine.conf $(build_path)/iso/boot/limine
	@cp -v $(limine_path)/limine-bios.sys $(limine_path)/limine-bios-cd.bin $(limine_path)/limine-uefi-cd.bin $(build_path)/iso/boot/limine 
	@cp -v $(limine_path)/BOOTX64.EFI $(build_path)/iso/EFI/BOOT/
	@cp -v $(limine_path)/BOOTIA32.EFI $(build_path)/iso/EFI/BOOT/
	@xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(build_path)/iso -o $(iso)
	@../limine/limine bios-install $(iso)

$(kernel): $(c_source_files) $(cpp_source_files)
	@nasm -f elf64 $(asm_source_files) -o $(build_path)/asm.o
	@g++ $(CFLAGS) $(LDFLAGS) $(build_path)/asm.o $(c_source_files) $(cpp_source_files) -o $(kernel)

run: $(iso)
	@qemu-system-x86_64 \
	-m 256M \
	-drive if=pflash,unit=0,format=raw,file=./../ovmf/ovmf-code-x86_64.fd,readonly=on \
	-cdrom $(iso)

run_debug: $(iso)
	@qemu-system-x86_64 \
	-m 256M \
	-drive if=pflash,unit=0,format=raw,file=./../ovmf/ovmf-code-x86_64.fd,readonly=on \
	-no-shutdown -no-reboot -d cpu_reset -cdrom $(iso)