### --------------------------------------------------------------------
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	
.section ".rodata"

scanfFormat:
	.asciz "%20s"

printfFormat:
	.asciz "%d\n"
### --------------------------------------------------------------------

        .section ".data"

ele_cnt:
	.word 0

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:

	pushl   %ebp
	movl    %esp, %ebp

input:

	## dc number stack initialized. %esp = %ebp
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp

	## check if user input EOF
	cmp	$EOF, %eax
	je	quit
	
	## check if first character is digit
	movl $buffer, %edi
	xor %ecx,%ecx
	movb (%edi), %cl
	pushl %ecx
	call isdigit
	add $4, %esp

	test %eax,%eax
	jz operator

pushval:
	## push a number onto the stack
	pushl $buffer
	call atoi
	add $4, %esp
	pushl %eax

	# increment ele_cnt
	call incr_cnt

operator:
	## handle operators, if not handle commands such as p,q,f...
	movl $buffer, %edi
	xor %ecx,%ecx
	movb (%edi), %cl

	## remember, all of these operation jumps returns back to input loop
	cmp $'+',%cl # +
	je oper_add

	cmp $'-',%cl  # - 
	je oper_sub

	cmp $'*',%cl # *
	je oper_mul

	#cmp %ecx, '/'
	#je oper_div

	#cmp %ecx, '%'
	#je oper_mod

	#cmp %ecx, '^'
	#je oper_pow

command:
	cmp $'q',%cl #
	je quit

	cmp $'p',%cl  #
	je print_top

	cmp $'f',%cl  #
	je print_stack

	cmp $'c',%cl  #
	je clear_stack

	cmp $'d',%cl  #
	je duplicate_top

	cmp $'r',%cl  #
	je reverse_top

	## unhandled case
	jmp input

oper_add:
	pop %edi
	pop %esi
	add %esi, %edi
	pushl %edi
	call dec_cnt
	jmp input

oper_sub:
	pop %edi
	pop %esi
	sub %esi, %edi
	pushl %edi
	call dec_cnt
	jmp input

oper_mul:
	pop %edi
	pop %esi
	imul %esi, %edi
	pushl %edi
	call dec_cnt
	jmp input


print_top:
	movl (%esp),%ecx
	pushl %ecx
	pushl $printfFormat
	call printf
	add $8, %esp
	jmp input

print_stack:
	# %edi is iterator
	xor %edi,%edi
	loop:
		cmp %edi,(ele_cnt)
		jbe input
		movl (%esp,%edi,4), %ecx
		pushl %ecx
		pushl $printfFormat
		call printf
		add $8, %esp
		inc %edi
		jmp loop

clear_stack:
	movl (ele_cnt), %edi
	imul $4,%edi
	add %edi,%esp
	xor %edi,%edi
	movl %edi, (ele_cnt)
	jmp input

duplicate_top:
	pop %ecx
	push %ecx
	push %ecx
	call incr_cnt
	jmp input

reverse_top:
	pop %edi
	pop %esi
	push %edi
	push %esi
	jmp input


incr_cnt:
	movl (ele_cnt), %edx
	inc %edx
	movl %edx, (ele_cnt)
	ret

dec_cnt:
	movl (ele_cnt), %edx
	dec %edx
	movl %edx, (ele_cnt)
	ret


quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret
