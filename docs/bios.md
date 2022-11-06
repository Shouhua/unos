## [general x86 real mode memory map](https://wiki.osdev.org/Memory_Map_(x86))
```
start        end        description
0x00000000 - 0x000003ff real mode interrupt vector table
0x00000400 - 0x000004ff BIOS data area
0x00000500 - 0x00007bff unused
0x00007c00 - 0x00007dff our bootloader
0x00007e00 - 0x0007ffff unused
0x00080000 - 0x0009ffff EBDA(Extended BIOS Data Area)
0x000a0000 - 0x000bffff video ram(vram) memory
0x000b0000 - 0x000b7777 monochrome video memory
0x000b8000 - 0x000bffff color video memory
0x000c0000 - 0x000c7fff video rom BIOS
0x000c8000 - 0x000effff BIOS shadow area
0x000f0000 - 0x000fffff system BIOS
```
## CHS
header starts 0  

cylinder starts 0  

sector starts 1  

## read sector
- print(video teletype output)
```
int 0x10/ah=0x0e
al = character to write
bh = page number(should be 0)
bl = foreground color(graphics mode only)

xor bx, bx
mov ah, 0x0e
mov al, 'H'
int 0x10
```
- reset floppy disk
```
int 0x13/ah=0x0  
ah = 0x0  
dl = dirive to reset  

returns:  
ah = status code  
cf(carry flag) is clear if success, it is set if failure
```
- read sectors into memory
```
int 0x13/ah=0x02
ah = 0x02
al = number of sectors to read
ch = low eight bits of cylinder number
cl = sector number(bits 0-5). Bits 6-7 are for hard disk only
dh = head number
dl = drive number(bit 7 set for hard disks)
es:bx = buffer to read sectors to

returns:
ah = status code
al = number of sectors read
cf = set if failure, cleared is successfull
```
- wait any key pressed
```
int 0x18
```
- warm boot
```
int 0x19/ah=0
```
- [get memory size](http://www.brokenthorn.com/Resources/OSDev17.html)
```
int 0x12
cf = clear if successful
ax = number of kb conventional memory
ah = status if error(0x80: invalid command; 0x86)upsupported function
only return WORD size value, so it is limited to 0xffff(65535 decimal)
```
- get memory map
```
Input
eax = 0x0000e820
ebx = continuation value or 0 to start at beginning of map
ecx = size of buffer for result(must be >= 20 bytes)
edx = 0x534d4150h('SMAP)
es:di = buffer for result

Output
cf = clear if successful
eax = 0x534d4150h('SMAP)
ebx = offset of next entry to copy from or 0 if done
ecx = acutual length returned in bytes
es:di	= buffer filled
if error, ah contains error code

0x0000000000000000	0x000000000009f000	1
0x000000000009f000	0x0000000000001000	2
0x00000000000e8000	0x0000000000018000	2
0x0000000000100000	0x0000000001ef0000	1
0x0000000001ff0000	0x0000000000100000	3
0x00000000fffc0000	0x0000000000040000	2
```

# fat12
TODO
0x7c00 + 512 MBR(master boot record)

1. load Root Directory to 0x7c00+0x0200(512)
bpbReservedSectors 1
bpbRootEntries 224


2. find file by loopping RD

3. load file 

4. run code
