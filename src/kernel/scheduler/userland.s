global enter_userland

; 模拟processor入栈参数
; 使用iret实现特权级别转移，0 -> 3
; 保证当前stack：ss, esp, eflags, cs, eip
enter_userland:
	cli
	mov ax, 0x23	; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push 0x23		; SS, notice it uses same selector as above
	push esp		; ESP
	pushfd			; EFLAGS

	pop eax
	or eax, 0x200 ;enable IF in EFLAGS
	push eax

	push 0x1b		; CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
	lea eax, [user_start]		; EIP first
	push eax
	iretd
user_start:
	add esp, 4 ; fix stack
	mov eax, 0
	mov ebx, user_land_str
	int 0x80
.loop:
	jmp .loop
	ret

user_land_str: db 'hello from user land', 0x0d, 0
