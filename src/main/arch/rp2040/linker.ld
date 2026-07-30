/* --- RP2040 Minimal Linker Script --- */
ENTRY(irq_handler_reset)

MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2M
    SRAM(rw)  : ORIGIN = 0x20000000, LENGTH = 264K
}

/* ============================================================
            SRAM layout

            0x20000000
            +----------------+
            | data           |
            | bss            |
            | vector RAM     |
            +----------------+
            | heap           |
            | grows upward   |
            | (remaining RAM)|
            +----------------+
            | PSP core0      |
            | grows downward |
            +----------------+
            | MSP core0      |
            +----------------+
            | MSP core1      |
            +----------------+
            0x20042000
============================================================ */

STACK0_SIZE = 5K;
MSP0_SIZE   = 1K;
MSP1_SIZE   = 1K;

STACK_END = ORIGIN(SRAM) + LENGTH(SRAM);

MSP1_START = STACK_END - MSP1_SIZE;
MSP0_START = MSP1_START - MSP0_SIZE;
STACK0_START = MSP0_START - STACK0_SIZE;

SECTIONS
{
    /* --- Vector table + checksum --- */
    .vectors : ALIGN(4)
    {
        KEEP(*(.checksum))
        _vector_table = .;
        KEEP(*(.vector_table))
        _vector_table_end = .;
    } > FLASH

    /* --- Code --- */
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

    /* --- BSS --- */

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

    /* --- Vector table relocated to RAM --- */

    .vector_table_ram ALIGN(256):
    {
        . = ALIGN(256);
        KEEP(*(.vector_table_ram))
    } > SRAM

    /* --- Heap (remaining SRAM before stacks) --- */
    _sheap = ALIGN(8);
    _eheap = STACK0_START;

    /*
     * Reserve stacks at the end of SRAM.
     * The linker places them explicitly, independently
     * from the current location counter.
     */

     /* --- Core0 PSP (main thread stack) --- */

     .stack0 STACK0_START (NOLOAD) :
     ALIGN(4)
     {
         _sstack0 = .;
         . = . + STACK0_SIZE;
         _estack0 = .;
     } > SRAM

     /* --- Core0 MSP --- */

     .msp0 MSP0_START (NOLOAD) :
     ALIGN(4)
     {
         _smsp0 = .;
         . = . + MSP0_SIZE;
         _emsp0 = .;
     } > SRAM

     /* --- Core1 MSP --- */

     .msp1 MSP1_START (NOLOAD) :
     ALIGN(4)
     {
         _smsp1 = .;
         . = . + MSP1_SIZE;
         _emsp1 = .;
     } > SRAM

    _esram = ORIGIN(SRAM) + LENGTH(SRAM);
}



/* --- Public symbols --- */

PROVIDE(_sstack = _sstack0);
PROVIDE(_estack = _estack0);

PROVIDE(sheap = _sheap);
PROVIDE(eheap = _eheap);
PROVIDE(heap_size = _eheap - _sheap);

ASSERT(STACK0_START >= _ebss, "SRAM overflow: heap and stacks overlap")