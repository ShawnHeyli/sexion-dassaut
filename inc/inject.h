#pragma once

#include "defs.h"
#include <elf.h>

long inject_section(cliArgs *args);
void modify_section_header(Elf64_Addr addr, sectionHeader *section, long offset);
char *get_section_name(int index);
sectionHeader *get_section_by_name(char *section_name);
void set_section_name(sectionHeader *section, char *name);
void swap_sections(sectionHeader *section1, sectionHeader *section2);
void sort_section_headers();
