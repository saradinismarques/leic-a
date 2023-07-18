segment	.bss
align	4
f:
	resb	4
segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 12
	push	dword 4
	lea	eax, [ebp+-4]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	push	dword 0
	lea	eax, [ebp+-8]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
segment	.rodata
align	4
_L1:
	db	32, 0
segment	.text
	push	dword $_L1
	call	prints
	add	esp, 4
	push	dword 0
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
align	4
_L2:
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setl	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L3
	push	dword 2
	push	dword [esp]
	lea	eax, [ebp+-8]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 3
	pop	ecx
	sal	dword [esp], cl
	pop	eax
	add	dword [esp], eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	add	dword [esp], eax
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	jmp	dword _L2
align	4
_L3:
segment	.rodata
align	4
_L4:
	db	32, 0
segment	.text
	push	dword $_L4
	call	prints
	add	esp, 4
	push	dword 0
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
align	4
_L5:
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setl	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L6
	lea	eax, [ebp+-8]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 3
	pop	ecx
	sal	dword [esp], cl
	pop	eax
	add	dword [esp], eax
	pop	eax
	push	dword [eax]
	call	printi
	add	esp, 4
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	add	dword [esp], eax
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	jmp	dword _L5
align	4
_L6:
segment	.rodata
align	4
_L7:
	db	10, 0
segment	.text
	push	dword $_L7
	call	prints
	add	esp, 4
	push	dword 0
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
align	4
_L8:
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setl	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L9
	push	dword 6
	lea	eax, [ebp+-8]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 3
	pop	ecx
	sal	dword [esp], cl
	pop	eax
	add	dword [esp], eax
	pop	eax
	push	dword [eax]
	pop	ecx
	pop	eax
	cdq
	idiv	ecx
	push	eax
	push	dword [esp]
	lea	eax, [ebp+-8]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 3
	pop	ecx
	sal	dword [esp], cl
	pop	eax
	add	dword [esp], eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	add	dword [esp], eax
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	jmp	dword _L8
align	4
_L9:
segment	.rodata
align	4
_L10:
	db	32, 0
segment	.text
	push	dword $_L10
	call	prints
	add	esp, 4
	push	dword 0
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
align	4
_L11:
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setl	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L12
	lea	eax, [ebp+-8]
	push	eax
	pop	eax
	push	dword [eax]
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 3
	pop	ecx
	sal	dword [esp], cl
	pop	eax
	add	dword [esp], eax
	pop	eax
	push	dword [eax]
	call	printi
	add	esp, 4
	lea	eax, [ebp+-12]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	add	dword [esp], eax
	push	dword [esp]
	lea	eax, [ebp+-12]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	jmp	dword _L11
align	4
_L12:
	push	dword 0
	pop	eax
	leave
	ret
	push	dword 0
	pop	eax
	leave
	ret
extern	printi
extern	prints
