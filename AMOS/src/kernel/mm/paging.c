#include <kernel/mm/paging.h>
#include <kernel/mm/physical.h>
#include <kernel/kernel.h>
#include <kernel/console.h>
#include <kernel/isr.h>

struct PAGE_DIRECTORY * pagedirectory;

struct PAGE_DIRECTORY_ENTRY * paging_getPageDirectoryEntry( DWORD linearAddress )
{
	return &pagedirectory->entry[ GET_DIRECTORY_INDEX(linearAddress) ];
}

void paging_clearDirectory()
{
	int i=0;
	
	memset( (BYTE *)pagedirectory, 0x00, sizeof(struct PAGE_DIRECTORY) );
	
	for( i=0 ; i<PAGE_ENTRYS; i++ )
	{
		struct PAGE_DIRECTORY_ENTRY * pde = &pagedirectory->entry[i];
		pde->present = FALSE;
		pde->readwrite = READWRITE;
		pde->user = SUPERVISOR;
	}
}

void paging_setPageDirectoryEntry( DWORD linearAddress, DWORD ptAddress )
{
	struct PAGE_DIRECTORY_ENTRY * pde = paging_getPageDirectoryEntry( linearAddress );
	memset( (BYTE *)ptAddress, 0x00, sizeof(struct PAGE_TABLE) );
	
	pde->present = TRUE;
	pde->readwrite = READWRITE;
	pde->user = SUPERVISOR;
	pde->writethrough = 0;
	pde->cachedisabled = 0;
	pde->accessed = 0;
	pde->reserved = 0;
	pde->pagesize = 0;
	pde->globalpage = 0;
	pde->available = 0;
	pde->address = ptAddress >> TABLE_SHIFT;
}

struct PAGE_TABLE_ENTRY * paging_getPageTableEntry( DWORD linearAddress )
{
	struct PAGE_DIRECTORY_ENTRY * pde = paging_getPageDirectoryEntry( linearAddress );
	struct PAGE_TABLE * pt = (struct PAGE_TABLE *)(pde->address << TABLE_SHIFT);
	if( pt == NULL )
	{
		int i;
		pt = (struct PAGE_TABLE *)physical_pageAlloc();
		paging_setPageDirectoryEntry( linearAddress, (DWORD)pt );
		for( i=0 ; i<PAGE_ENTRYS ; i++ )
			paging_setPageTableEntry( linearAddress+SIZE_4KB, 0L, FALSE );
	}
	return (struct PAGE_TABLE_ENTRY *)&pt->entry[ GET_TABLE_INDEX(linearAddress) ];
}

// maps a linear address to a physical address
void paging_setPageTableEntry( DWORD linearAddress, DWORD physicalAddress, BOOL present )
{
	struct PAGE_TABLE_ENTRY * pte = paging_getPageTableEntry( PAGE_ALIGN( linearAddress ) );

	pte->present = present;
	pte->readwrite = READWRITE;
	pte->user = SUPERVISOR;
	pte->writethrough = 0;
	pte->cachedisabled = 0;
	pte->accessed = 0;
	pte->dirty = 0;
	pte->attributeindex = 0;
	pte->globalpage = 0;
	pte->available = 0;
	pte->address = PAGE_ALIGN( physicalAddress ) >> TABLE_SHIFT;
}

void paging_handler( struct REGISTERS * reg )
{
	DWORD address;
	__asm__ __volatile__ ( "movl %%cr2, %0" : "=r" (address) );
	kprintf( "paging_handler() - General Protection Fault at %x\n", address );
}

extern void start;
extern void end;

void paging_init()
{
	DWORD physicalAddress;
	DWORD linearAddress;

	pagedirectory = (struct PAGE_DIRECTORY *)physical_pageAlloc();

	// clear out the page directory...
	paging_clearDirectory();
		
	// identity map bottom 4MB's
	for( physicalAddress=0L ; physicalAddress<(1024*SIZE_4KB) ; physicalAddress+=SIZE_4KB )
		paging_setPageTableEntry( physicalAddress, physicalAddress, TRUE );		

	// map the kernel's virtual address to its physical memory location
	linearAddress = (DWORD)&start;
	for( physicalAddress=V2P(&start); physicalAddress<V2P(&end) ; physicalAddress+=SIZE_4KB )
	{
		paging_setPageTableEntry( linearAddress, physicalAddress, TRUE );
		linearAddress += SIZE_4KB;		
	}

	// Enable Paging...
	__asm__ __volatile__ ( "movl %%eax, %%cr3" :: "r" ( pagedirectory ) );
	__asm__ __volatile__ ( "movl %cr0, %eax" );
	__asm__ __volatile__ ( "orl $0x80000000, %eax" );
	__asm__ __volatile__ ( "movl %eax, %cr0" );
}