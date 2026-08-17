.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.global g_pfnVectors
.global Reset_Handler
.global Default_Handler

.extern main

.extern vPortSVCHandler
.extern xPortPendSVHandler
.extern xPortSysTickHandler

.extern _estack
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss

/* ============================================================
 * Cortex-M4 interrupt vector table
 * ============================================================ */

.section .isr_vector,"a",%progbits
.align 2
.type g_pfnVectors, %object

g_pfnVectors:
    .word   _estack                  /* 0  Initial SP */
    .word   Reset_Handler            /* 1  Reset */
    .word   Default_Handler          /* 2  NMI */
    .word   Default_Handler          /* 3  HardFault */
    .word   Default_Handler          /* 4  MemManage */
    .word   Default_Handler          /* 5  BusFault */
    .word   Default_Handler          /* 6  UsageFault */
    .word   0                        /* 7  Reserved */
    .word   0                        /* 8  Reserved */
    .word   0                        /* 9  Reserved */
    .word   0                        /* 10 Reserved */
    .word   vPortSVCHandler          /* 11 SVCall */
    .word   Default_Handler          /* 12 DebugMonitor */
    .word   0                        /* 13 Reserved */
    .word   xPortPendSVHandler       /* 14 PendSV */
    .word   xPortSysTickHandler      /* 15 SysTick */

.size g_pfnVectors, . - g_pfnVectors


/* ============================================================
 * Reset handler
 * ============================================================ */

.section .text.Reset_Handler,"ax",%progbits
.align 2
.thumb_func
.type Reset_Handler, %function

Reset_Handler:

    /* --------------------------------------------------------
     * Set VTOR = address of our vector table.
     *
     * FreeRTOS uses SCB->VTOR to find the vector table when
     * starting the first task.
     * -------------------------------------------------------- */

    ldr     r0, =0xE000ED08          /* SCB->VTOR */
    ldr     r1, =g_pfnVectors
    str     r1, [r0]
    dsb
    isb

    /* --------------------------------------------------------
     * Copy initialized .data from FLASH to RAM
     * -------------------------------------------------------- */

    ldr     r0, =_sidata
    ldr     r1, =_sdata
    ldr     r2, =_edata

.Ldata_copy:
    cmp     r1, r2
    bcs     .Lbss_zero
    ldr     r3, [r0], #4
    str     r3, [r1], #4
    b       .Ldata_copy

    /* --------------------------------------------------------
     * Zero-fill .bss
     * -------------------------------------------------------- */

.Lbss_zero:
    ldr     r1, =_sbss
    ldr     r2, =_ebss
    movs    r3, #0

.Lbss_loop:
    cmp     r1, r2
    bcs     .Lcall_main
    str     r3, [r1], #4
    b       .Lbss_loop

    /* --------------------------------------------------------
     * Enter C/C++ main
     * -------------------------------------------------------- */

.Lcall_main:
    bl      main

    /* main() should never return */
.Lhang:
    b       .Lhang

.size Reset_Handler, . - Reset_Handler


/* ============================================================
 * Default exception handler
 * ============================================================ */

.section .text.Default_Handler,"ax",%progbits
.align 2
.thumb_func
.type Default_Handler, %function

Default_Handler:
    b       Default_Handler

.size Default_Handler, . - Default_Handler

.end
