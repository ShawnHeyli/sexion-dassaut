#include "inject.h"
#include "parse.h"
#include <elf.h>
#include <stdio.h>

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

void print_section_header(sectionHeader header) {
  printf("Name               Type              Address            Offset\n");
  printf("Size               EntSize           Flags  Link  Info  Align\n");

  printf("%-18s ", get_section_name(header.sh_name));
  printf("%-18s", header.sh_type == 1 ? "PROG" : "UNKNOWN");
  printf("%-18lx ", header.sh_addr);
  printf("%lx\n", header.sh_offset);
  printf("%-18lx ", header.sh_size);
  printf("%-18lx ", header.sh_entsize);
  printf("%-5lu ", header.sh_flags);
  printf("%-5u ", header.sh_link);
  printf("%-5u ", header.sh_info);
  printf("%lu\n\n", header.sh_addralign);
}

void print_prog_header(progHeader header) {
  printf("Type           Offset         VirtAddr       PhysAddr\n");
  printf("FileSiz        MemSiz         Flags          Align\n");

  printf("%-14s ", header.p_type == 1 ? "LOAD" : "UNKNOWN");
  printf("%-14lx ", header.p_offset);
  printf("%-14lx ", header.p_vaddr);
  printf("%-14lx\n", header.p_paddr);
  printf("%-14lx ", header.p_filesz);
  printf("%-14lx ", header.p_memsz);
  printf("%-14u ", header.p_flags);
  printf("%lu\n\n", header.p_align);
}
