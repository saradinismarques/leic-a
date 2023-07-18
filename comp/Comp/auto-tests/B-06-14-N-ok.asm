segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 0
	push	dword 100
	push	dword 100
	pop	ecx
	pop	eax
	cdq
	idiv	ecx
	push	eax
	call	printi
	add	esp, 4
	push	dword 0
	pop	eax
	leave
	ret
	push	dword 0
	pop	eax
	leave
	ret
extern	printi
