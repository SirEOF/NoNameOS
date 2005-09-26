#ifndef _KERNEL_KERNEL_H_
#define _KERNEL_KERNEL_H_

#include <sys/types.h>

#define AMOS_MAJOR_VERSION 0
#define AMOS_MINOR_VERSION 2
#define AMOS_PATCH_VERSION 0


#define sti() __asm__ __volatile__ ("sti")

#define cli() __asm__ __volatile__ ("cli")

struct AOUT_SYSTEM_TABLE
{
    DWORD tabsize;
    DWORD strsize;
    DWORD addr;
    DWORD reserved;
};

struct ELF_SECTION_HEADER_TABLE
{
    DWORD num;
    DWORD size;
    DWORD addr;
    DWORD shndx;
};

struct MULTIBOOT_INFO
{
    DWORD flags;
    DWORD mem_lower;
    DWORD mem_upper;
    BYTE  boot_device[4];
    DWORD cmdline;
    DWORD mods_count;
    DWORD mods_addr;
    union
    {
	  struct AOUT_SYSTEM_TABLE aout_sym;
	  struct ELF_SECTION_HEADER_TABLE elf_sec;
    } u;
    DWORD mmap_length;
    DWORD mmap_addr;

    DWORD drives_length;
    DWORD drives_addr;
    DWORD config_table;
    DWORD boot_loader_name;
    DWORD apm_table;
    DWORD vbe_control_info;
    DWORD vbe_mode_info;
    WORD  vbe_mode;
    WORD  vbe_interface_seg;
    WORD  vbe_interface_off;
    WORD  vbe_interface_len;
};

BYTE * memset( BYTE *, BYTE, int );

BYTE inportb( WORD );

void outportb( WORD, BYTE );

#endif
