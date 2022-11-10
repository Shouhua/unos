// paging.c -- Defines the interface for and structures relating to paging.
//             Written for JamesM's kernel development tutorials.
#include "kernel/mm/pmm.h"
#include "kernel/mm/vmm.h"
#include "lib/string.h"
#include "lib/log.h"
#include "kernel/isr.h"

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

// The current page directory;
pd_t* cur_dir = 0;

void page_fault(registers_t regs)
{
	// A page fault has occurred.
	// The faulting address is stored in the CR2 register.
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0"
				 : "=r"(faulting_address));

	// The error code gives us details of what happened.
	int present = !(regs.err_code & 0x1); // Page not present
	int rw = regs.err_code & 0x2;		  // Write operation?
	int us = regs.err_code & 0x4;		  // Processor was in user-mode?
	int reserved = regs.err_code & 0x8;	  // Overwritten CPU-reserved bits of page entry?
	// int id = regs.err_code & 0x10;		  // Caused by an instruction fetch?

	// Output an error message.
	printf("Page fault! ( ");
	if (present)
	{
		printf("present ");
	}
	if (rw)
	{
		printf("read-only ");
	}
	if (us)
	{
		printf("user-mode ");
	}
	if (reserved)
	{
		printf("reserved ");
	}
	printf(") at 0x%x\n", faulting_address);
	PANIC("Page fault");
}

bool switch_page_directory(pd_t *dir)
{
	if(!dir) return false;
	cur_dir = dir;
	asm volatile("mov %0, %%cr3" ::"r"((physical_addr)&dir->entries));
	return true;
}

void enable_paging(bool b)
{
	uint32_t cr0;
	asm volatile("mov %%cr0, %0"
				 : "=r"(cr0));
	if(b) { // enable paging
		cr0 |= 0x80000000; 
	} else { // disable paging
		cr0 &= 0x7FFFFFFF;
	}
	asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

pd_t* vmm_get_directory () {
	return cur_dir;
}

void vmm_map_page(void* phys, void* virt) {
	pd_t* dir = vmm_get_directory();
	pde_t* e = &dir->entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
	if((*e & PTE_PRESENT) != PTE_PRESENT) { // 不存在page directory entry
		pt_t* table = (pt_t*)pmm_alloc_block();
		if(!table) return;
		memset(table, 0, sizeof(pt_t));
		pde_t* dir_entry = &dir->entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
		*dir_entry = (*dir_entry & ~PDE_FRAME) | (physical_addr)table | PDE_PRESENT | PDE_WRITABLE;
	}
	pt_t* table = (pt_t*)PAGE_GET_PHYSICAL_ADDRESS(e);
	pte_t* page = &table->entries[PAGE_TABLE_INDEX((uint32_t)virt)];
	*page = (*page & ~PTE_FRAME) | (physical_addr)phys | PTE_PRESENT | PTE_WRITABLE;
}

void init_paging()
{
	#ifdef DEBUG
		INFO("[VMM] Vmm init begin");
	#endif
	// 4M identity map
	pt_t* identity_table = (pt_t*) pmm_alloc_block();
	if(!identity_table) return;
	memset(identity_table, 0, sizeof(pt_t));

	// 0x100000(1M) -> 0xC0000000(3G)
	pt_t* kernel_table = (pt_t*)pmm_alloc_block();
	if(!kernel_table) return;
	memset(kernel_table, 0, sizeof(pt_t));

	// NOTE: 这里如果给的256，0-0xFFFF映射，eip现在是比如0x104080，由于找不到映射，会出现page fault问题
	for(int i = 0, frame=0x0; i<1024;i++,frame+=4096) {
		pte_t page = 0;
		page = (page & ~PTE_FRAME) | frame | PTE_PRESENT; // frame类似0x1000，低12位都是0
		identity_table->entries[PAGE_TABLE_INDEX(frame)] = page;
	}

	for(int i=0, frame=0x000000, virt=0xc0000000;i<1024;i++,frame+=4096,virt+=4096) {
		pte_t page = 0;
		page = (page & ~PTE_FRAME) | frame | PTE_PRESENT;
		kernel_table->entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	pd_t* dir = (pd_t*)pmm_alloc_blocks(4);
	if(!dir) return;

	memset(dir, 0, sizeof(pd_t));

	pde_t* identity_pde = &dir->entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	*identity_pde = ((PDE_PRESENT | PDE_WRITABLE) & ~PDE_FRAME) | (physical_addr)identity_table;

	pde_t* kernel_pde = &dir->entries[PAGE_DIRECTORY_INDEX(0xC0000000)];
	*kernel_pde = (*kernel_pde & ~PDE_FRAME) | (physical_addr)kernel_table | PDE_PRESENT | PDE_WRITABLE;

	register_interrupt_handler(14, page_fault);
	switch_page_directory(dir);
	enable_paging(true);
}