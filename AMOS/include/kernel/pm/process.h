#ifndef _KERNEL_PM_PROCESS_H_
#define _KERNEL_PM_PROCESS_H_

#include <sys/types.h>
#include <kernel/fs/vfs.h>

#define PROCESS_CODEADDRESS		(void *)0x10000000
#define PROCESS_STACKADDRESS	(void *)0x20000000
#define PROCESS_STACKSIZE		SIZE_4KB

struct PROCESS_STACK
{
	DWORD ds;
	DWORD es;
	DWORD fs;
	DWORD gs;
	DWORD edi;
	DWORD esi;
	DWORD ebp;
	DWORD esp;
	DWORD ebx;
	DWORD edx;
	DWORD ecx;
	DWORD eax;
	DWORD intnumber;
	DWORD errorcode;
	DWORD eip;
	DWORD cs;
	DWORD eflags;
	//DWORD esp0;
	//DWORD ss0;
};

enum
{
	READY=0,
	RUNNING,
	BLOCKED,
};

struct PROCESS_INFO
{
	int id;
	int tick_slice;
	int state;
	DWORD current_esp;
	void * stack;
	struct PAGE_DIRECTORY * page_dir;
	struct VFS_HANDLE * console;
	//struct MM_HEAP heap;	
};

int process_spawn( char *, struct VFS_HANDLE * );

void process_destroy( struct PROCESS_INFO * );

struct PROCESS_INFO * process_create( void (*thread)() );

#endif 
