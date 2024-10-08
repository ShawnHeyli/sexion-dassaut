#pragma once

#include "defs.h"
#include <elf.h>

// Inject
int inject_section(cliArgs *args);

// Sorting
void modify_section_header(Elf64_Addr addr, sectionHeader *section,
                           long offset);
char *get_section_name(Elf64_Word index);
sectionHeader *get_section_by_name(char *section_name);
void set_section_name(sectionHeader *section, char *name);
void swap_sections(sectionHeader *section1, sectionHeader *section2);
void sort_section_headers(void);

// PT-NOTE
progHeader *get_pt_note(void);
void overwrite_pt_note(progHeader *phdr, sectionHeader shdr);

// GOT/Entrypoint
void modify_entrypoint(Elf64_Addr entrypoint);
void modify_got_entry(char *function, Elf64_Addr address);
