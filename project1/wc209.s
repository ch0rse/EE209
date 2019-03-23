	.file	"wc209.c"
	.text
	.section	.rodata
.LC0:
	.string	"UNREACHABLE CODE!!!\n"
	.align 8
.LC1:
	.string	"Error: line %lu: unterminated comment\n"
.LC2:
	.string	"%lu %lu %lu\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%rdx, -88(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$1, -12(%rbp)
	movl	$0, -16(%rbp)
	movq	$0, -24(%rbp)
	movq	$0, -32(%rbp)
	movq	$0, -40(%rbp)
	movq	$0, -48(%rbp)
	movl	$0, -52(%rbp)
	jmp	.L2
.L54:
	cmpl	$6, -4(%rbp)
	ja	.L3
	movl	-4(%rbp), %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L5(%rip), %rax
	movl	(%rdx,%rax), %eax
	cltq
	leaq	.L5(%rip), %rdx
	addq	%rdx, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L5:
	.long	.L11-.L5
	.long	.L10-.L5
	.long	.L9-.L5
	.long	.L8-.L5
	.long	.L7-.L5
	.long	.L6-.L5
	.long	.L4-.L5
	.text
.L11:
	movq	$1, -48(%rbp)
	movl	$0, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L12
	movl	$0, -12(%rbp)
	jmp	.L3
.L12:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movsbq	-53(%rbp), %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L14
	cmpb	$10, -53(%rbp)
	je	.L14
	movl	$2, -4(%rbp)
	jmp	.L3
.L14:
	cmpb	$47, -53(%rbp)
	jne	.L15
	movl	$4, -4(%rbp)
	jmp	.L3
.L15:
	cmpb	$10, -53(%rbp)
	jne	.L16
	movl	$3, -4(%rbp)
	jmp	.L3
.L16:
	movl	$1, -4(%rbp)
	jmp	.L3
.L10:
	cmpl	$1, -8(%rbp)
	je	.L17
	cmpl	$4, -8(%rbp)
	je	.L17
	addq	$1, -40(%rbp)
.L17:
	addq	$1, -32(%rbp)
	movl	$1, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L18
	movl	$0, -12(%rbp)
	jmp	.L3
.L18:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movsbq	-53(%rbp), %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L20
	cmpb	$10, -53(%rbp)
	je	.L20
	movl	$2, -4(%rbp)
	jmp	.L3
.L20:
	cmpb	$47, -53(%rbp)
	jne	.L21
	movl	$4, -4(%rbp)
	jmp	.L3
.L21:
	cmpb	$10, -53(%rbp)
	jne	.L22
	movl	$3, -4(%rbp)
	jmp	.L3
.L22:
	movl	$1, -4(%rbp)
	jmp	.L3
.L9:
	addq	$1, -32(%rbp)
	movl	$2, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L23
	movl	$0, -12(%rbp)
	jmp	.L3
.L23:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movsbq	-53(%rbp), %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L25
	cmpb	$10, -53(%rbp)
	je	.L25
	movl	$2, -4(%rbp)
	jmp	.L3
.L25:
	cmpb	$47, -53(%rbp)
	jne	.L26
	movl	$4, -4(%rbp)
	jmp	.L3
.L26:
	cmpb	$10, -53(%rbp)
	jne	.L27
	movl	$3, -4(%rbp)
	jmp	.L3
.L27:
	movl	$1, -4(%rbp)
	jmp	.L3
.L8:
	addq	$1, -32(%rbp)
	addq	$1, -48(%rbp)
	movl	$3, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L28
	movl	$0, -12(%rbp)
	jmp	.L3
.L28:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movsbq	-53(%rbp), %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L30
	cmpb	$10, -53(%rbp)
	je	.L30
	movl	$2, -4(%rbp)
	jmp	.L3
.L30:
	cmpb	$47, -53(%rbp)
	jne	.L31
	movl	$4, -4(%rbp)
	jmp	.L3
.L31:
	cmpb	$10, -53(%rbp)
	jne	.L32
	movl	$3, -4(%rbp)
	jmp	.L3
.L32:
	movl	$1, -4(%rbp)
	jmp	.L3
.L7:
	cmpb	$42, -53(%rbp)
	je	.L33
	addq	$1, -32(%rbp)
.L33:
	cmpl	$0, -8(%rbp)
	je	.L34
	cmpl	$2, -8(%rbp)
	je	.L34
	cmpl	$3, -8(%rbp)
	jne	.L35
.L34:
	cmpb	$42, -53(%rbp)
	je	.L35
	addq	$1, -40(%rbp)
.L35:
	movl	$4, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L36
	movl	$0, -12(%rbp)
	jmp	.L3
.L36:
	cmpb	$42, -53(%rbp)
	jne	.L38
	movl	$5, -4(%rbp)
	jmp	.L3
.L38:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movsbq	-53(%rbp), %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L39
	cmpb	$10, -53(%rbp)
	je	.L39
	movl	$2, -4(%rbp)
	jmp	.L3
.L39:
	cmpb	$47, -53(%rbp)
	jne	.L40
	movl	$4, -4(%rbp)
	jmp	.L3
.L40:
	cmpb	$10, -53(%rbp)
	jne	.L41
	movl	$3, -4(%rbp)
	jmp	.L3
.L41:
	movl	$1, -4(%rbp)
	jmp	.L3
.L6:
	cmpl	$0, -16(%rbp)
	jne	.L42
	movq	-48(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	$1, -16(%rbp)
.L42:
	cmpl	$4, -8(%rbp)
	je	.L43
	cmpl	$5, -8(%rbp)
	je	.L43
	cmpl	$6, -8(%rbp)
	je	.L43
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$20, %edx
	movl	$1, %esi
	leaq	.LC0(%rip), %rdi
	call	fwrite@PLT
	movl	$-1, %edi
	call	exit@PLT
.L43:
	movl	$5, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L44
	movl	$0, -12(%rbp)
	movq	stderr(%rip), %rax
	movq	-24(%rbp), %rdx
	leaq	.LC1(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$1, %edi
	call	exit@PLT
.L44:
	cmpb	$42, -53(%rbp)
	jne	.L45
	movl	$6, -4(%rbp)
	jmp	.L3
.L45:
	cmpb	$10, -53(%rbp)
	jne	.L47
	addq	$1, -48(%rbp)
	addq	$1, -32(%rbp)
.L47:
	movl	$5, -4(%rbp)
	jmp	.L3
.L4:
	movl	$6, -8(%rbp)
	cmpb	$-1, -53(%rbp)
	jne	.L48
	movl	$0, -12(%rbp)
	movq	stderr(%rip), %rax
	movq	-24(%rbp), %rdx
	leaq	.LC1(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$1, %edi
	call	exit@PLT
.L48:
	cmpb	$47, -53(%rbp)
	jne	.L49
	movl	$0, -16(%rbp)
	movl	$2, -4(%rbp)
	jmp	.L55
.L49:
	cmpb	$42, -53(%rbp)
	jne	.L51
	movl	$6, -4(%rbp)
	jmp	.L55
.L51:
	cmpb	$10, -53(%rbp)
	jne	.L52
	addq	$1, -32(%rbp)
	addq	$1, -48(%rbp)
.L52:
	movl	$5, -4(%rbp)
.L55:
	nop
.L3:
	addl	$1, -52(%rbp)
.L2:
	call	getchar@PLT
	movb	%al, -53(%rbp)
	cmpb	$0, -53(%rbp)
	je	.L53
	cmpl	$0, -12(%rbp)
	jne	.L54
.L53:
	movq	-32(%rbp), %rcx
	movq	-40(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 8.2.0-13) 8.2.0"
	.section	.note.GNU-stack,"",@progbits
