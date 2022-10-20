[Intel Pentium Instruction](https://faydoc.tripod.com/cpu/)

- lodsb ds:si > al (memory > registry)
- stosb al > es:di (registry > memory)
- cmpsb ds:si & es:di  
```
MOV SI, STR1
MOV DI, STR2
MOV CX, 100
CLD; Clear the direction flag

REPE CMPSB
```
- movsb ds:si > es:di
- xchg ax, cx