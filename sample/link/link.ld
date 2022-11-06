ENTRY(loader)
. = 0x100000;
SECTIONS {
	.mydata BLOCK(0x20) :  {
		*(.data)
	}
	/**
		AT符号用于指定加载的物理地址
		使用objdump -h main.elf 或者 readelf -l main.elf
		查看虚拟地址和物理地址分别为0x00100040, 0x000fb040
		BLOCK代表section需要对齐
	*/
	.mytext BLOCK(0x20): 
		AT(ADDR(.mytext) - 0x5000) 
	{
		*(.text)
	}
	.mybss : {
		*(.bss)
	}
}