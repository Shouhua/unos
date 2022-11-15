#ifndef __VMM_H__
#define __VMM_H__

#include "lib/stdint.h"
#include "kernel/isr.h"

#define PTE_PRESENT 	  0x1
#define	PTE_WRITABLE 	  0x2		  
#define	PTE_USER 		  0x4			  
#define	PTE_WRITETHOUGH   0x8	  
#define	PTE_NOT_CACHEABLE 0x10 
#define	PTE_ACCESSED      0x20	  
#define	PTE_DIRTY         0x40		  
#define	PTE_PAT           0x80			  
#define	PTE_CPU_GLOBAL 	  0x100	  
#define	PTE_LV4_GLOBAL    0x200	  
#define	PTE_FRAME    			0x7FFFF000	  

#define	PDE_PRESENT				0x1			
#define	PDE_WRITABLE			0x2			
#define	PDE_USER				0x4			
#define	PDE_PWT					0x8			
#define	PDE_PCD					0x10		
#define	PDE_ACCESSED			0x20		
#define	PDE_DIRTY				0x40		
#define	PDE_4MB					0x80		
#define	PDE_CPU_GLOBAL			0x100		
#define	PDE_LV4_GLOBAL			0x200		
#define	PDE_FRAME    			0x7FFFF000	  

typedef uint32_t pte_t;
typedef uint32_t pde_t;

typedef struct pt
{
	pte_t entries[1024];
} pt_t;

typedef struct pd
{
	pde_t entries[1024];
} pd_t;

extern pd_t* cur_dir;

void init_paging();

void* virt2phys(void* virt_addr);
void vmm_map_page(void* phys, void* virt);
bool switch_page_directory(pd_t *);
void enable_paging(bool);
void page_fault_handler(registers_t* regs);

#endif
