global outb ; make the label outb visible outside this file
global inb

global outw;
global inw;

global outl;
global inl;

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address
outb:
    mov al, [esp + 8] ; move the data to be sent into the al register
    mov dx, [esp + 4] ; move the address of the I/O port into the dx register
    out dx, al        ; send the data to the I/O port
    ret               ; return to the calling function

inb:
    mov dx, [esp + 4]
    in al, dx
    ret

outw:
    mov ax, [esp + 8] ; move the data to be sent into the al register
    mov dx, [esp + 4] ; move the address of the I/O port into the dx register
    out dx, ax        ; send the data to the I/O por
    ret               ; return to the calling function

inw:
    mov dx, [esp + 4]
    in ax, dx
    ret

outl:
    mov eax, [esp + 8] ; move the data to be sent into the al register
    mov dx, [esp + 4] ; move the address of the I/O port into the dx register
    out dx, eax        ; send the data to the I/O por
    ret               ; return to the calling function

inl:
    mov dx, [esp + 4]
    in eax, dx
    ret