ENTRY(loader)

SECTIONS {
    . = 0x100000;
    .boot_text ALIGN(4K) : {
        *(.multiboot)
        build/obj/boot/*.o(.text)
    }
    .boot_bss ALIGN(4K) : {
        build/obj/boot/*.o(.bss)
    }
    .boot_data ALIGN(4K) : {
        build/obj/boot/*.o(.data)
    }
    .boot_rodata ALIGN(4K) : {
        build/obj/boot/*.o(.rodata)
    }

    . += 0xC0000000;
    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000)
    {
        __kernel_start = .;
        build/obj/kernel/*.o(.text)
        build/obj/lib/*.o(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000)
    {
        build/obj/kernel/*.o(.rodata)
        build/obj/lib/*.o(.rotext)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        build/obj/kernel/*.o(.COMMON)
        build/obj/kernel/*.o(.bss)
        build/obj/lib/*.o(.COMMON)
        build/obj/lib/*.o(.bss)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000)
    {
        build/obj/kernel/*.o(.data)
        build/obj/lib/*.o(.data)
    }
    __kernel_end = ALIGN(4k);
}