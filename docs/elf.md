## elf
- program headers(segments)，只有可执行文件才有，目标文件没有
- sector headers(sectors)
- segment可以认为是多个sector的组成，映射关系可以通过readelf -l来查看

## readelf
```shell
# display elf file header
readelf -h [object/execute file] 

# read elf section headers
readelf -S [object/execute file] 

# read elf symbol table information
readelf -s [object/execute file]  # symbol table
readelf -sD [object/execute file]  # dynamic symbol table

# display section content
readelf -x .text [object/execute file]

# string dump section
readelf -p .data [object/execute file]

# read program header table
readelf -l [execute file]
readelf -lW [execute file] # print entry on one line
```

## objdump
```shell
# section information
objdump -h [obj/execute file]
# disassemble code segement
objdump -M intel -d [obj/execute file]
# relocation table
objdump -r [obj/execute file]
# list all segement content
objdump -s
# list all symbols
objdump -t 
# display information only for secion
objdump -s -j .rodata [object/execute file]
objdump -s -d -j .got -j .got.plt -j .plt hello
```
