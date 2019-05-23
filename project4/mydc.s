### --------------------------------------------------------------------
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	
.section ".rodata"

scanfFormat:
	.asciz "%s"

printfFormat:
	.asciz "%d\n"
### --------------------------------------------------------------------

        .section ".data"
		ele_cnt DD 0

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
	movb %(edi), %ecx
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
	push %eax

	## increment ele_cnt
	inc ele_cnt
	jmp input

operator:
	## handle operators, if not handle commands such as p,q,f...
	movl $buffer, %edi
	movb %(edi), %ecx

	## remember, all of these operation jumps returns back to input loop
	cmp %ecx, '+'
	je oper_add

	cmp %ecx, '-'
	je oper_sub

	cmp %ecx, '*'
	je oper_mul

	#cmp %ecx, '/'
	#je oper_div

	#cmp %ecx, '%'
	#je oper_mod

	#cmp %ecx, '^'
	#je oper_pow

command:
	cmp %ecx, 'q'
	je quit

	cmp %ecx, 'p'
	je print_top

	## unhandled case
	jmp input

oper_add:
	popl $edi
	popl %esi
	add %esi, %edi
	pushl %edi
	jmp input

oper_sub:
	popl $edi
	popl %esi
	sub %esi, %edi
	pushl %edi
	jmp input

oper_mul:
	popl $edi
	popl %esi
	imul %esi, %edi
	pushl %edi
	jmp input


print_top:
	movl %ecx, %(esp)
	pushl %ecx
	pushl $printfFormat
	call printf
	add $8, %esp

quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret