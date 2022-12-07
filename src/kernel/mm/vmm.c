// paging.c -- Defines the interface for and structures relating to paging.
//             Written for JamesM's kernel development tutorials.
#include "kernel/mm/pmm.h"
#include "kernel/mm/vmm.h"
#include "lib/string.h"
#include "lib/log.h"
#include "kernel/mm/malloc.h"

#define PAGE_DIRECTORY_INDEX(x) (((uint32_t)(x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((uint32_t)(x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) ((uint32_t)x & ~0xfff)

// The current page directory;
pd_t* cur_dir = 0;
pd_t* kpage_dir;

void* virt2phys(pd_t* dir, void* virt_addr) {
	if(!dir) {
		// 下面式子virt_addr: 0xc010d050,得到结果0x43414
		// NOTICE: 因为使用指针，所以得到的值是(0xC010D050 - 0xC0000000)/4, C语言经典指针计算
		// uint32_t t = (uint32_t*)virt_addr - (uint32_t*)0xC0000000;
		return (void*)(virt_addr - 0xC0000000);
	}
	uint32_t page_dir_idx = PAGE_DIRECTORY_INDEX(virt_addr);
	uint32_t page_tbl_idx = PAGE_TABLE_INDEX(virt_addr);
	uint32_t page_frame_offset = PAGE_GET_PHYSICAL_ADDRESS(virt_addr);
	pde_t table = dir->entries[page_dir_idx];
	uint32_t frame = ((pt_t*)(table|0xFFFFFFD00))->entries[page_tbl_idx];
	t = (t << 12) + page_frame_offset;
	return (void*)t;
}

inline pte_t* vmm_pt_lookup_entry(pt_t* p, uint32_t virt_addr)
{
	if(p) return &p->entries[PAGE_TABLE_INDEX(virt_addr)];
	return 0;
}

inline pde_t* vmm_pd_lookup_entry(pd_t* p, uint32_t virt_addr)
{
	if(p) return &p->entries[PAGE_DIRECTORY_INDEX(virt_addr)];
	return 0;
}

void vmm_flush_tlb_entry(uint32_t virt_addr)
{
    asm volatile("invlpg (%0)" ::"r"(virt_addr) : "memory");
}

// TODO 目前分配单位是4K，很不科学
// sz暂时为4k块的个数
bool
vmm_alloc_page(pd_t* dir, void* va, size_t sz, uint16_t attr) 
{
	if(!dir) PANIC("[VMM] page directory address error");
	// uint32_t page_count = sz / PMM_BLOCK_SIZE;
	uint32_t page_count = sz - 1;
	void* p = pmm_alloc_blocks(page_count + 1);
	for(uint32_t i=0; i<=page_count; page_count++) {
		vmm_map_page(dir, p + i * PMM_BLOCK_SIZE, va + i * PMM_BLOCK_SIZE, attr);
	}
	return true;
}
// bool vmm_alloc_page(pte_t* e) 
// {
// 	void* p = pmm_alloc_block();
// 	if(!p) return false;
// 	*e = (*e & ~PTE_FRAME) | (uint32_t)p | PTE_PRESENT;
// 	return true;
// }

void vmm_free_page(pte_t* e)
{
	void* p = (void*)(*e & 0xFFFFF000);
	if(p) pmm_free_block(p);
	*e &= ~PTE_PRESENT;
}

void page_fault_handler(register_t* regs)
{
	// A page fault has occurred.
	// The faulting address is stored in the CR2 register.
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0"
				 : "=r"(faulting_address));

	// The error code gives us details of what happened.
	int present = !(regs->err_code & 0x1); // Page not present
	int rw = regs->err_code & 0x2;		  // Write operation?
	int us = regs->err_code & 0x4;		  // Processor was in user-mode?
	int reserved = regs->err_code & 0x8;	  // Overwritten CPU-reserved bits of page entry?
	// int id = regs->err_code & 0x10;		  // Caused by an instruction fetch?

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

// NOTICE: 是否需要区分pde和pte的attr
void vmm_map_page(pd_t* dir, void* phys, void* virt, uint32_t attr) {
	pde_t* e = &dir->entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
	if((*e & PTE_PRESENT) != PTE_PRESENT) { // 不存在page directory entry
		pt_t* table = (pt_t*)pmm_alloc_block();
		if(!table) return;
		memset(table, 0, sizeof(pt_t));
		pde_t* dir_entry = &dir->entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
		*dir_entry = ((physical_addr)table & 0xFFFFF000) | attr;
	}
	pt_t* table = (pt_t*)PAGE_GET_PHYSICAL_ADDRESS(*e);
	pte_t* page = &table->entries[PAGE_TABLE_INDEX((uint32_t)virt)];
	*page = ((physical_addr)phys & 0xFFFFFF000) | attr;
}

void init_paging()
{
	#ifdef DEBUG
		INFO("[VMM] Vmm init begin");
	#endif
	// 4M identity map 0-4M -> 0-4M
	pt_t* identity_table = (pt_t*) pmm_alloc_block();
	if(!identity_table) return;
	memset(identity_table, 0, sizeof(pt_t));

	// 0xC0000000-0xC0400000 -> 0M-4M
	pt_t* kernel_table = (pt_t*)pmm_alloc_block();
	if(!kernel_table) return;
	memset(kernel_table, 0, sizeof(pt_t));

	// NOTE: 这里如果给的256，0-0xFFFF映射，eip现在是比如0x104080，由于找不到映射，会出现page fault问题
	for(int i = 0, frame=0x0; i<1024; i++,frame+=4096) {
		pte_t page = (frame & 0xFFFFF000) | PTE_PRESENT | PTE_WRITABLE | PTE_USER; // frame类似0x1000，低12位都是0
		identity_table->entries[PAGE_TABLE_INDEX(frame)] = page;
	}

	for(int i=0,frame=0x000000,virt=0xC0000000; i<1024; i++,frame+=4096,virt+=4096) {
		// 添加PTE_USER为了在userland里面可以使用相关虚拟地址, 不然page fault
		pte_t page = (frame & 0xFFFFF000) | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
		kernel_table->entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	kpage_dir = (pd_t*)pmm_alloc_block();
	if(!kpage_dir) return;

	memset(kpage_dir, 0, sizeof(pd_t));

	pde_t* identity_pde = &kpage_dir->entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	*identity_pde = ((physical_addr)identity_table & 0xFFFFF000) | PDE_PRESENT | PDE_WRITABLE | PTE_USER;

	pde_t* kernel_pde = &kpage_dir->entries[PAGE_DIRECTORY_INDEX(0xC0000000)];
	*kernel_pde = ((physical_addr)kernel_table & 0xFFFFF000) | PDE_PRESENT | PDE_WRITABLE | PTE_USER;

	register_interrupt_handler(14, page_fault_handler);
	switch_page_directory(kpage_dir);
	enable_paging(true);
}
/*
 * Copy a page directory
 * */
// void copy_page_directory(pd_t * dst, pd_t * src) {
//     for(uint32_t i = 0; i < 1024; i++) {
//         if(kpage_dir->ref_tables[i] == src->ref_tables[i]) {
//             // Link kernel pages
//             dst->tables[i] = src->tables[i];
//             dst->ref_tables[i] = src->ref_tables[i];
//         }
//         else {
//             // For non-kernel pages, copy the pages (for example, when forking process, you don't want the parent process mess with child process's memory)
//             dst->ref_tables[i] = copy_page_table(src, dst, i, src->ref_tables[i]);
//             uint32_t phys = (uint32_t)virtual2phys(src, dst->ref_tables[i]);
//             dst->tables[i].frame = phys >> 12;
//             dst->tables[i].user = 1;
//             dst->tables[i].rw = 1;
//             dst->tables[i].present = 1;
//         }
//     }
// }
/*
 * Copy a page directory
 * */
// pt_t* copy_page_table(pd_t* src_page_dir, pd_t* dst_page_dir, uint32_t page_dir_idx, pd_t* src) {
//     pt_t* table = (pt_t*)(sizeof(pt_t));
//     for(int i = 0; i < 1024; i++) {
//         if(!table->pages[i].frame)
//             continue;
//         // Source frame's virtual address
//         uint32_t src_virtual_address = (page_dir_idx << 22) | (i << 12) | (0);
//         // Destination frame's virtual address
//         uint32_t dst_virtual_address = src_virtual_address;
//         // Temporary virtual address in current virtual address space
//         uint32_t tmp_virtual_address = 0;

//         // Allocate a frame in destination page table
//         allocate_page(dst_page_dir, dst_virtual_address, 0, 0, 1);
//         // Now I want tmp_virtual_address and dst_virtual_address both points to the same frame
//         allocate_page(src_page_dir, tmp_virtual_address, (uint32_t)virtual2phys(dst_page_dir, (void*)dst_virtual_address), 0, 1);
//         if (src->pages[i].present) table->pages[i].present = 1;
//         if (src->pages[i].rw)      table->pages[i].rw = 1;
//         if (src->pages[i].user)    table->pages[i].user = 1;
//         if (src->pages[i].accessed)table->pages[i].accessed = 1;
//         if (src->pages[i].dirty)   table->pages[i].dirty = 1;
//         memcpy((void*)tmp_virtual_address, (void*)src_virtual_address, PAGE_SIZE);
//         // Unlink frame
//         free_page(src_page_dir, tmp_virtual_address, 0);
//     }
//     return table;
// }