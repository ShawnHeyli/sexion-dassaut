#include "defs.h"
#include "parse.h"
#include "utils.h"
#include <elf.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  long unsigned res = (args->address - offset) % 4096;
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

char *get_section_name(int index) {
  // Get the ELF header
  elfHeader *ehdr =
      (Elf64_Ehdr *)target.map; // if name smaller than OVERWRITTEN_SECTION_NAME

  // Get shstrtab section header
  sectionHeader *shstrtab =
      (sectionHeader *)((sectionHeader *)target.map + ehdr->e_shoff +
                        ehdr->e_shentsize * ehdr->e_shstrndx);
  char *name = (char *)((char *)target.map + shstrtab->sh_offset + index);
  return name;
}

//
void set_section_name(sectionHeader *section, char *name) {

  // Get the ELF header
  elfHeader *ehdr = (Elf64_Ehdr *)target.map;

  // Get shstrtab section header
  sectionHeader *shstrtab =
      (sectionHeader *)((sectionHeader *)target.map + ehdr->e_shoff +
                        ehdr->e_shentsize * ehdr->e_shstrndx);

  // Get the section name
  char *section_name =
      (char *)((char *)target.map + shstrtab->sh_offset + section->sh_name);

  if (strlen(name) > strlen(section_name)) {
    errx(EXIT_FAILURE, "Section name is too long");
  }

  // Set the section name
  strcpy(section_name, name);
}

sectionHeader *get_section_by_name(char *section_name) {
  // Get the ELF header
  elfHeader *ehdr = (Elf64_Ehdr *)target.map;

  // Get the section by name
  for (int i = 0; i < ehdr->e_shnum; i++) {
    sectionHeader *shdr =
        (sectionHeader *)((sectionHeader *)target.map + ehdr->e_shoff +
                          ehdr->e_shentsize * i);
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
  elfHeader *ehdr = (Elf64_Ehdr *)target.map;

  // Goto the section headers
  sectionHeader *shdr =
      (sectionHeader *)((sectionHeader *)target.map + ehdr->e_shoff);

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
