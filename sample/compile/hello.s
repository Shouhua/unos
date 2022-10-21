	.text
	.globl _message
	.data
	.align 2
_message:
	.ascii "hello, world!\12\0"
	.globl _buf
	.zerofill __DATA,__common,_buf,1024,5
	.text
	.globl _main
_main:
	subl	$12, %esp
	call	___x86.get_pc_thunk.ax
L1$pb:
	subl	$12, %esp
	leal	_message-L1$pb(%eax), %eax
	pushl	%eax
	call	_printf
	addl	$16, %esp
	movl	$0, %eax
	addl	$12, %esp
	ret
	.weak_definition	___x86.get_pc_thunk.ax
	.private_extern	___x86.get_pc_thunk.ax
___x86.get_pc_thunk.ax:
	movl	(%esp), %eax
	ret
	.subsections_via_symbols
