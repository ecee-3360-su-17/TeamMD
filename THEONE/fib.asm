	.align 2
	.global fib
;	.cpu cortex-m4
;	.syntax unified
	.thumb
.thumb_func

fib:
	push {r3, r4, lr}
	mov r1, r0

	mov r2, #1
	cmp r2, r4
	blt subtract
	mov r2, r4
	b deepah
subtract:
	sub r4, r4, #2
	bl fib
	mov r3, r2
	sub r0, r4, #1
	bl fib

deepah:
	pop {pc, r4, r3}

	;should do the trick.

;input is r0
;output is r0.
;seems like a pain.
fib:

