global enable_blink
global disable_blink

disable_blink:
	mov  dx,3DAh             ; reset the flip-flop
	in   al,dx

	mov  dx,3C0h             ; index 0x10  (20h + 10h)?
	mov  al,30h
	out  dx,al

	inc  dx                  ;  clear bit 3 to disable blink
	in   al,dx
	and  al,(~(1<<3))
	dec  dx
	out  dx,al
	ret

enable_blink:
	mov  dx,3DAh             ; reset the flip-flop
	in   al,dx

	mov  dx,3C0h             ; index 0x10  (20h + 10h)?
	mov  al,30h
	out  dx,al

	inc  dx                  ;  clear bit 3 to disable blink
	in   al, dx
	or   al, 0x08
	dec  dx
	out  dx, al
	ret

