	.align 2
	.global fib
;	.cpu cortex-m4
;	.syntax unified
	.thumb
.thumb_func

;input is r0
;output is r0.
;drop down frames until n = 0.
;you don't need the 0th fib number because it is 0. Adds nothing to the sum.
;climb back up the frames while summing the numbers until the last frame where it completes.
fib:
	push {lr}			;this has to be here in case n < 2.
	cmp r0, #1
	bgt deeper			;break if n > 1. Have to choose 1 so that fib(1) doesn't return 2.

	;initials
	;The program only reaches this once and it's to set up the root values
	mov r1, #1			;r1 = n-1
	mov r2, #0			;r2 = n-2.
	push {r1, r2}		;simply so I can use recursion on base without making an exception.

sum:					;phase two of le alg.
	pop {r1, r2}		;r1 and r2 are n-1 and n-2
	mov r3, r0			;ugly intermediary that allows me to change r0 without sacrificing the ability to update r1 with old r0
	add r0, r1, r2	;n = n-1 + n-2
	mov r2, r1			;set up n-2 for next iteration
	mov r1, r3			;set up n-1 for next iteration
									;n-2 = n-1 and n-1 = n (n is assumed to be n+1 in the next frame.)

	pop {lr}				;have to do this so I can push n-1, n-2 onto stack before moving lr into pc
	push {r1, r2}		;I don't actually need this and I don't think it's even a good idea but we have to use the stack so here it is.
	mov pc, lr			;return a frame -- also exit of program on last frame

deeper:
	sub r0, r0, #1	;could possibly be #2 instead. Would need to investigate what happens when you overshoot. a little up/down climb action might be needed
	bl fib					;Need to have something after for when the bottom is hit.
	;returns here after reaching the bottom of the chain.
	b sum

; is a stupid comment token.






; -----------------------------------------
; yee olde fib resides in the depths below.
; -----------------------------------------

;fib:
;	push {r3, r4, lr}
;	mov r1, r0

;	mov r2, #1
;	cmp r2, r4
;	blt subtract
;	mov r2, r4
;	b deepah
;subtract:
;	sub r4, r4, #2
;	bl fib
;	mov r3, r2
;	sub r0, r4, #1
;	bl fib

;deepah:
;	pop {r3, r4, pc}

	;should do the trick.
