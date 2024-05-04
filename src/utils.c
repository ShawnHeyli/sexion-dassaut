#include "inject.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

extern fileMapping target;
extern fileMapping payload;

void deallocate_global_map(void) {
  if (munmap(target.map, target.sb.st_size) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }
  fclose(target.file);
}

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
  printf("%-5lx ", header.sh_flags);
  printf("%-5x ", header.sh_link);
  printf("%-5x ", header.sh_info);
  printf("%lx\n\n", header.sh_addralign);
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
  printf("%-14x ", header.p_flags);
  printf("%lx\n\n", header.p_align);
}

void print_dyn_symbol(Elf64_Sym symb) {
  printf("Name: %u\n", symb.st_name);
  printf("Value: %lx\n", symb.st_value);
  printf("Size: %lx\n", symb.st_size);
  printf("Info: %x\n", symb.st_info);
  printf("Other: %x\n", symb.st_other);
  printf("Section index: %x\n", symb.st_shndx);
}
