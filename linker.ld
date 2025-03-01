/* Linker script for Vibe OS */
ENTRY(_start)

SECTIONS {
    /* Start at 1MB, a common place for kernels to be loaded by the bootloader */
    . = 1M;

    .text : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    .rodata : ALIGN(4K) {
        *(.rodata)
    }

    .data : ALIGN(4K) {
        *(.data)
    }

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }
} 