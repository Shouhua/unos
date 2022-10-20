## CHS
header starts 0
cylinder starts 0
sector starts 1

# fat12检索
0x7c00 + 512 MBR(master boot record)

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
1. load Root Directory to 0x7c00+0x0200(512)
bpbReservedSectors 1
bpbRootEntries 224


2. find file by loopping RD

3. load file 

4. run code