.syntax unified
.cpu cortex-m0
.align 2
.global asm_sum
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
fib:	
//initial handling, then loop.
//r0 is n on start
	cmp r0, #1
	blt base_case
	//else... could do reverse as well
	sub r0, #1
	call fib
	pop {r1}
	add r0, r1, r0
	
base_case:
	mov r0, #1
	push {r0}
	call fib

exit:
	mov pc, lr
