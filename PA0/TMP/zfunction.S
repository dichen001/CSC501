	.file	"zfunction.c"
	.text
.globl zfunction
	.type	zfunction, @function
zfunction:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	andl	$0xfffff00f, %eax
	sall	$8, %eax
	leave
	ret
	.size	zfunction, .-zfunction
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 20060404 (Red Hat 3.4.6-3)"
