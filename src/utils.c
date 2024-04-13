#include <stdio.h>
#include "parse.h"

void print_file_header(elfHeader header) {
  printf("ELF Header:\n");
  printf("Magic: ");
  for (int i = 0; i < 16; i++) {
    printf("%02x ", header.e_ident[i]);
  }
  printf("%ul\n", header.e_type);
  printf("Entry point address: %lx\n", header.e_entry);
  printf("Start of program headers: %lu\n", header.e_phoff);
  printf("Start of section headers: %lu\n", header.e_shoff);
  printf("Flags: %u\n", header.e_flags);
  printf("Size of this header: %u\n", header.e_ehsize);
  printf("Size of program headers: %u\n", header.e_phentsize);
  printf("Number of program headers: %u\n", header.e_phnum);
  printf("Size of section headers: %u\n", header.e_shentsize);
  printf("Number of section headers: %u\n", header.e_shnum);
  printf("Section header string table index: %u\n", header.e_shstrndx);
}

void print_section_header(Elf64_Shdr header) {
  printf("Section Header:\n");
  printf("Name: %u\n", header.sh_name);
  printf("Type: %u\n", header.sh_type);
  printf("Flags: %lu\n", header.sh_flags);
  printf("Address: %lu\n", header.sh_addr);
  printf("Offset: %lu\n", header.sh_offset);
  printf("Size: %lu\n", header.sh_size);
  printf("Link: %u\n", header.sh_link);
  printf("Info: %u\n", header.sh_info);
  printf("Address alignment: %lu\n", header.sh_addralign);
  printf("Entry size: %lu\n", header.sh_entsize);
}
