- lodsb ds:si > al
- stosb al > es:di
- cmpsb ds:si & es:di  
```
MOV SI, STR1
MOV DI, STR2
MOV CX, 100
CLD; Clear the direction flag

REPE CMPSB
```
- movsb ds:si > es:di
- xchg 