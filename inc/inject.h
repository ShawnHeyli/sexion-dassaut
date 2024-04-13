#pragma once

#include "defs.h"

int inject_section(cliArgs *args);
void modify_section_header(cliArgs args, Elf64_Shdr *section, long offset);
