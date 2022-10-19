## elf
- program headers(segments)，只有可执行文件才有，目标文件没有
- sector headers(sectors)
- segment可以认为是多个sector的组成，映射关系可以通过readelf -l来查看

## readelf
```
# read elf header information
readelf -h [object/execute file] 
# read elf section header information
readelf -S [object/execute file] 
# read elf symbol table information
readelf -s [object/execute file] 
# read secton content
readelf -p .shstrtab [object/execute file]
# read program header
readelf -l [execute file]
```

## objdump
```
# section informations
objdump -h [obj/execute file]
# disassemble code segement
objdump -d [obj/execute file]
# relation table
objdump -r [obj/execute file]
```