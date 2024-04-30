#pragma once
#include "parse.h"

void print_file_header(elfHeader header);
void print_section_header(sectionHeader header);
void print_prog_header(progHeader header);
void print_dyn_symbol(Elf64_Sym symb);
