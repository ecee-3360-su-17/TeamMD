.cpu cortex-m0
.syntax unified
.align 2
.global fib
.thumb
.thumb_func

	/*
	int fib(int n){
	if (n <= 1)
		return 1;
	return fib(n-1) + fib(n-2);
	}
	*/
	/* start from the nth fib. Move downward, creating n frames for all of the
	   fib numbers and count upwards as the frames close */
	/* seems like garbage. Tons of memory needed to do this. */
//MLA, MLS, MUL (MLU is the normal multiply)
/*
fib:
//initial handling, then loop.
//r0 is n on start
	cmp r0, #1
	blt base_case
	//else...
	sub r0, #2	// decrement n by 2.
	call fib		//try, try again
	//returns here after reaching n < 1. (r0 -> 0)

	add r0, r1, r0

base_case:
	mov r1, #1
	mov pc, lr	//return

exit:
	mov pc, lr
*/
//------------------------------------------------------------------------------
fib:
	push {r3, r4, lr}
	mov r1, r0

	movs r2, #1
	cmp r2, r4
	blt subtract
	mov r2, r4
	b deepah
subtract:
	subs r4, r4, #2
	bl fib
	mov r3, r2
	subs r0, r4, #1
	bl fib

deepah:
	pop {pc, r4, r3}

	//should do the trick.
