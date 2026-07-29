/* --- RP2040 Minimal Linker Script --- */
ENTRY(irq_handler_reset)

MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2M
    SRAM(rw)  : ORIGIN = 0x20000000, LENGTH = 264K
}

/* --- Sections --- */
SECTIONS
{
    /* --- Vector table + optional checksum --- */
    .vectors : ALIGN(4)
    {
        KEEP(*(.checksum))
        _vector_table = .;
        KEEP(*(.vector_table))
        _vector_table_end = .;
    } > FLASH

    /* --- Code and read-only data --- */
    .text : ALIGN(4)
    {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.init)
        *(.fini)
    } > FLASH

    /* --- Constructor arrays --- */
    .preinit_array : ALIGN(4)
    {
        __preinit_array_start = .;
        *(.preinit_array)
        *(SORT_BY_INIT_PRIORITY(.preinit_array.*))
        __preinit_array_end = .;
    } > FLASH

    .init_array : ALIGN(4)
    {
        __init_array_start = .;
        *(.init_array)
        *(SORT_BY_INIT_PRIORITY(.init_array.*))
        __init_array_end = .;
    } > FLASH

    .fini_array : ALIGN(4)
    {
        __fini_array_start = .;
        *(.fini_array)
        *(SORT_BY_INIT_PRIORITY(.fini_array.*))
        __fini_array_end = .;
    } > FLASH

    /* --- Initialized data --- */
    _la_data = LOADADDR(.data);
    .data : ALIGN(4)
    {
        _sdata = .;
        *(.data)
        *(.data.*)
        _edata = .;
    } > SRAM AT> FLASH

    /* --- Uninitialized data --- */
    .bss : ALIGN(4)
    {
        _sbss = .;
        __bss_start__ = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end__ = .;
        _ebss = .;
    } > SRAM

    .vector_table_ram ALIGN(256):
    {
        . = ALIGN(256);
        KEEP(*(.vector_table_ram))
    }> SRAM

    /* --- MSP (core0) --- */
    .msp0 (NOLOAD) : ALIGN(4)
    {
        _smsp0 = .;
        . = . + 2K;
        _emsp0 = .;
    } > SRAM

    /* --- MSP (core1) --- */
    .msp1 (NOLOAD) : ALIGN(4)
    {
        _smsp1 = .;
        . = . + 2K;
        _emsp1 = .;
    } > SRAM

    /* --- Stack 0 (core0) --- */
    .stack0 (NOLOAD) : ALIGN(4)
    {
        _sstack0 = .;
        . = . + 5K;
        _estack0 = .;
    } > SRAM

    /* --- Heap (remaining SRAM) --- */
    .heap (NOLOAD) : ALIGN(8)
    {
        _sheap = .;
        . = ORIGIN(SRAM) + LENGTH(SRAM);
        _eheap = .;
        _esram = .;
    } > SRAM
}

/* Provide convenient symbols */
PROVIDE(_sstack = _sstack0);  /* bottom of stack0 */
PROVIDE(_estack = _estack0);  /* top of stack0 */