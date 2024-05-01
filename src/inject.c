#include "defs.h"
#include "parse.h"
#include "utils.h"
#include <bfd.h>
#include <elf.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define OVERWRITTEN_SECTION_NAME ".overwritten"

extern fileMapping target;
extern fileMapping payload;

long inject_section(cliArgs *args) {

  long offset = target.sb.st_size;

  // Append binary at the end of target.file
  fseek(target.file, 0, SEEK_END);
  fseek(payload.file, 0, SEEK_SET);

  char buffer[256];
  size_t bytes;
  while ((bytes = fread(buffer, 1, sizeof(buffer), payload.file)) > 0) {
    fwrite(buffer, 1, bytes, target.file);
  }

  long unsigned res = (offset - args->address) % 4096;
  if (res != 0) {
    args->address += res;
  }

  // We can return the size of the original document
  // because we appended at the end of the file
  return offset;
}

void modify_section_header(Elf64_Addr addr, sectionHeader *section,
                           long offset) {
  // sh_type to SHT_PROGBITS
  section->sh_type = SHT_PROGBITS;

  // sh_addr to new address
  section->sh_addr = addr;
  // sh_offset to offset
  section->sh_offset = offset;

  // sh_size to size of inject file
  section->sh_size = payload.sb.st_size;

  // sh_addralgin to 16
  section->sh_addralign = 16;

  // sh_flags add SHF_EXECINSTR
  section->sh_flags |= SHF_EXECINSTR;
}

char *get_section_name(Elf64_Word index) {

  // Get the ELF header
  elfHeader *ehdr =
      (elfHeader *)target.map; // if name smaller than OVERWRITTEN_SECTION_NAME

  // Get shstrtab section header
  sectionHeader *shstrtab =
      (sectionHeader *)((uint64_t)target.map + ehdr->e_shoff +
                        (long)ehdr->e_shentsize * ehdr->e_shstrndx);
  char *name = (char *)((uint64_t)target.map + shstrtab->sh_offset + index);
  return name;
}

//
void set_section_name(sectionHeader *section, char *name) {
  // Get the ELF header
  elfHeader *ehdr = (elfHeader *)target.map;

  // Get shstrtab section header
  sectionHeader *shstrtab =
      (sectionHeader *)((uint64_t)target.map + ehdr->e_shoff +
                        (long)ehdr->e_shentsize * ehdr->e_shstrndx);

  // Get the section name
  char *section_name =
      (char *)((uint64_t)target.map + shstrtab->sh_offset + section->sh_name);

  if (strlen(name) > strlen(section_name)) {
    errx(EXIT_FAILURE, "Section name is too long");
  }

  // Copy the new name
  while (*name != '\0') {
    *section_name = *name;
    section_name++;
    name++;
  }

  *section_name = '\0';
}

sectionHeader *get_section_by_name(char *section_name) {
  // Get the ELF header
  elfHeader *ehdr = (elfHeader *)target.map;

  // Get the section by name
  for (int i = 0; i < ehdr->e_shnum; i++) {
    sectionHeader *shdr =
        (sectionHeader *)((uint64_t)target.map + ehdr->e_shoff +
                          (long)ehdr->e_shentsize * i);
    char *name = get_section_name(shdr->sh_name);
    if (strcmp(name, section_name) == 0) {
      sectionHeader *section = shdr;
      return section;
    }
  }

  return NULL;
}

void swap_sections(sectionHeader *section1, sectionHeader *section2) {
  // Swap in memory
  sectionHeader temp = *section1;
  *section1 = *section2;
  *section2 = temp;
}

// We sort by address with a bubble sort
// the section paramater is only section out of order at the beginning
void sort_section_headers() {
  // Get the ELF header
  elfHeader *ehdr = (elfHeader *)target.map;

  // Goto the section headers
  sectionHeader *shdr = (sectionHeader *)((uint64_t)target.map + ehdr->e_shoff);

  // Bubble sort
  int index = 0;
  bool found_section = false;
  for (index = 0; index < ehdr->e_shnum - 1; index++) {
    sectionHeader *section1 = shdr + index;
    sectionHeader *section2 = shdr + index + 1;
    if (section1->sh_offset > section2->sh_offset) {
      swap_sections(section1, section2);
      found_section = true;
    } else if (found_section && section1->sh_offset > section2->sh_offset) {
      break;
    }
  }

  // We need to decrement shstrndx if we move it back
  if (ehdr->e_shstrndx >= index) {
    ehdr->e_shstrndx--;
  }

  // We need to decrement the link attribute if we moved
  // the symtab and dyntab otherwise section won't be able
  // to find them
  for (int i = 0; i < index; i++) {
    sectionHeader *section = shdr + i;
    if (section->sh_link > 0) {
      section->sh_link--;
    }
  }
}

progHeader *get_pt_note() {
  // Get the ELF header
  elfHeader *ehdr = (elfHeader *)target.map;

  // Get the program headers
  progHeader *phdr = (progHeader *)((uint64_t)target.map + ehdr->e_phoff);

  // Get the PT_NOTE program header
  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr->p_type == PT_NOTE) {
      return phdr;
    }
    phdr++;
  }

  return NULL;
}

void overwrite_pt_note(progHeader *phdr, sectionHeader shdr) {
  phdr->p_type = PT_LOAD;
  phdr->p_offset = shdr.sh_offset;
  phdr->p_vaddr = shdr.sh_addr;
  phdr->p_paddr = shdr.sh_addr;
  phdr->p_filesz = shdr.sh_size;
  phdr->p_memsz = shdr.sh_size;
  phdr->p_flags |= PF_X;
  phdr->p_align = 4096; // 0x1000
}

void modify_entrypoint(Elf64_Addr entrypoint) {
  elfHeader *ehdr = (elfHeader *)target.map;
  ehdr->e_entry = entrypoint;
}

void modify_got_entry(char *function, Elf64_Addr address) {
  // Get the dynamic section
  sectionHeader *dynsym = get_section_by_name(".dynsym");
  if (dynsym == NULL) {
    deallocate_global_map();
    errx(EXIT_FAILURE, "Could not get section by name");
  }
  sectionHeader *dynstr = get_section_by_name(".dynstr");
  if (dynstr == NULL) {
    deallocate_global_map();
    errx(EXIT_FAILURE, "Could not get section by name");
  }
  sectionHeader *rela_plt = get_section_by_name(".rela.plt");
  if (rela_plt == NULL) {
    deallocate_global_map();
    errx(EXIT_FAILURE, "Could not get section by name");
  }
  sectionHeader *got_plt = get_section_by_name(".got.plt");
  if (got_plt == NULL) {
    deallocate_global_map();
    errx(EXIT_FAILURE, "Could not get section by name");
  }

  Elf64_Sym *symbol_table =
      (Elf64_Sym *)((uint64_t)target.map + dynsym->sh_offset);
  Elf64_Rela *rela = (Elf64_Rela *)((uint64_t)target.map + rela_plt->sh_offset);
  Elf64_Addr *got = (Elf64_Addr *)((uint64_t)target.map + got_plt->sh_offset);

  // Find the rela.plt entry for the target
  unsigned int symbol_idx = 0;
  for (unsigned int i = 0; i < rela_plt->sh_size / sizeof(Elf64_Rela); i++) {
    Elf64_Rela *entry = rela + i;
    Elf64_Sym *symbol = symbol_table + ELF64_R_SYM(entry->r_info);
    char *name = (char *)target.map + dynstr->sh_offset + symbol->st_name;
    if (strcmp(name, function) == 0) {
      // printf("Replaced entry at %d: %s\n", i, name);
      symbol_idx = i;
      break;
    }
  }

  // Modify the GOT entry
  // printf("Replaced entry at %d: %s   0x%lx -> 0x%lx\n", symbol_idx, function,
  // got[symbol_idx + 3], address);
  got[symbol_idx + 3] = address;
}
