ENTRY(loader)                /* the name of the entry label */

SECTIONS {
    . = 0xC0100000;          /* the code should be loaded at 1 MB */

    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.multiboot)
        *(.text)             /* all text sections from all files */
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .bss ALIGN(4K) : AT(ADDR(.bss)-0xC0000000)
    {
        *(.COMMON)
        *(.bss)
        *(.initial_stack)
    }

    .data ALIGN (0x1000) :  AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)             /* all data sections from all files */
    }
    end = .;
}