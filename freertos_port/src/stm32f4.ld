ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* ========================================================
     * Interrupt vector table
     * ======================================================== */

    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* ========================================================
     * Program code and read-only data
     * ======================================================== */

    .text :
    {
        . = ALIGN(4);

        *(.text.Reset_Handler)
        *(.text*)
        *(.rodata*)

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ========================================================
     * Initialized data
     * Stored in FLASH, copied to RAM by Reset_Handler.
     * ======================================================== */

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data*)

        . = ALIGN(4);

        _edata = .;
    } > RAM AT > FLASH

    /* ========================================================
     * Zero-initialized data
     * ======================================================== */

    .bss :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);

        _ebss = .;
    } > RAM

    /* ========================================================
     * End of RAM
     * ======================================================== */

    . = ALIGN(8);

    end = .;
    _end = .;
    __end__ = .;

    /* Don't discard these symbols/sections accidentally. */
    PROVIDE(_heap_start = .);
}
