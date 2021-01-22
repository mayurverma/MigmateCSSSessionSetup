
    .thumb
    .thumb_func

    .section .test,"ax",%progbits
    .globl HardFault_Handler
    .type  HardFault_Handler, %function
HardFault_Handler:
	mrs		r0, msp
	mov		r1, lr
	ldr		r2, =InitHardFaultHandler
	bx		r2

    .pool

