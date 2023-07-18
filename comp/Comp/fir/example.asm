; TEXT
segment	.text
; ALIGN
align	4
; GLOBAL _main, :function
global	_main:function
; LABEL _main
_main:
; ENTER 8
	push	ebp
	mov	ebp, esp
	sub	esp, 8
; INT 0
	push	dword 0
; LOCAL -4
	lea	eax, [ebp+-4]
	push	eax
; STINT
	pop	ecx
	pop	eax
	mov	[ecx], eax
        ;; before body 
; INT 1
	push	dword 1
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; STINT
	pop	ecx
	pop	eax
	mov	[ecx], eax
; RODATA
segment	.rodata
; ALIGN
align	4
; LABEL _L2
_L2:
; SSTRING (value omitted -- see below)
	db	"Teste", 32, "para", 32, "a", 32, "fun", 195, 167, 195, 163, "o", 32, "factorial", 0
; TEXT
segment	.text
; ADDR _L2
	push	dword $_L2
; CALL prints
	call	prints
; TRASH 4
	add	esp, 4
; CALL println
	call	println
; CALL argc
	call	argc
; LDFVAL32
	push	eax
; INT 2
	push	dword 2
; EQ
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	sete	cl
	mov	[esp], ecx
; JZ _L3
	pop	eax
	cmp	eax, byte 0
	je	near _L3
; INT 1
	push	dword 1
; CALL argv
	call	argv
; TRASH 4
	add	esp, 4
; LDFVAL32
	push	eax
; CALL atoi
	call	atoi
; TRASH 4
	add	esp, 4
; LDFVAL32
	push	eax
; DUP32
	push	dword [esp]
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; STINT
	pop	ecx
	pop	eax
	mov	[ecx], eax
; TRASH 4
	add	esp, 4
; LABEL _L3
_L3:
        ;; after body 
; LABEL _main_L4
_main_L4:
; LOCAL -4
	lea	eax, [ebp+-4]
	push	eax
; LDINT
	pop	eax
	push	dword [eax]
; STFVAL32
	pop	eax
; LEAVE
	leave
; RET
	ret
; EXTERN argc
extern	argc
; EXTERN argv
extern	argv
; EXTERN atoi
extern	atoi
; EXTERN factorial
extern	factorial
; EXTERN println
extern	println
; EXTERN prints
extern	prints
