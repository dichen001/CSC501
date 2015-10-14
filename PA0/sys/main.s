	.file	"main.c"
	.section	.rodata
.LC0:
	.string	"esp:\t 0x%x \n"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	andl	$-16, %esp
	movl	$0, %eax
	addl	$15, %eax
	addl	$15, %eax
	shrl	$4, %eax
	sall	$4, %eax
	subl	%eax, %esp
#APP
	movl %esp, esp
#NO_APP
	subl	$8, %esp
	pushl	esp
	pushl	$.LC0
	call	kprintf
	addl	$16, %esp
	call	printtos
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.local	esp
	.comm	esp,4,4
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 20060404 (Red Hat 3.4.6-3)"
