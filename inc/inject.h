#pragma once

#include "defs.h"
#include <elf.h>

int inject_section(cliArgs *args);
void modify_section_header(Elf64_Addr addr, sectionHeader *section, long offset);
char *get_section_name(Elf64_Word index);
sectionHeader *get_section_by_name(char *section_name);
void set_section_name(sectionHeader *section, char *name);
void swap_sections(sectionHeader *section1, sectionHeader *section2);
void sort_section_headers();

progHeader *get_pt_note();
void overwrite_pt_note(progHeader *phdr, sectionHeader shdr);

void modify_entrypoint(Elf64_Addr entrypoint);
