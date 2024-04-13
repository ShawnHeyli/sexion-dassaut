#include "defs.h"
#include "parse.h"
#include "utils.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OVERWRITTEN_SECTION_NAME ".overwritten"

extern fileMapping target;
extern fileMapping payload;

int inject_section(cliArgs *args) {

  long offset = ftell(target.file);

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

  return offset;
}

void modify_section_header(cliArgs args, Elf64_Shdr *section, long offset) {
  // sh_type to SHT_PROGBITS
  section->sh_type = SHT_PROGBITS;

  // sh_addr to new address
  section->sh_addr = args.address;
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
      (sectionHeader *)((char *)target.map + ehdr->e_shoff +
                        ehdr->e_shentsize * ehdr->e_shstrndx);
  char *name = (char *)((char *)target.map + shstrtab->sh_offset + index);
  return name;
}

//
void set_section_name(sectionHeader *section, char *name) {
  // if name smaller than OVERWRITTEN_SECTION_NAME
  if (strlen(name) > strlen(OVERWRITTEN_SECTION_NAME)) {
    errx(EXIT_FAILURE, "Section name is too long");
  }

  // Get the ELF header
  elfHeader *ehdr = (Elf64_Ehdr *)target.map;

  // Get shstrtab section header
  sectionHeader *shstrtab =
      (sectionHeader *)((char *)target.map + ehdr->e_shoff +
                        ehdr->e_shentsize * ehdr->e_shstrndx);

  // Get the section name
  char *section_name =
      (char *)((char *)target.map + shstrtab->sh_offset + section->sh_name);

  // Set the section name
  strcpy(section_name, name);
}

sectionHeader *get_section_by_name(char *section_name) {
  // Get the ELF header
  elfHeader *ehdr = (Elf64_Ehdr *)target.map;

  // Get the section by name
  for (int i = 0; i < ehdr->e_shnum; i++) {
    sectionHeader *shdr = (sectionHeader *)((char *)target.map + ehdr->e_shoff +
                                            ehdr->e_shentsize * i);
    char *name = get_section_name(shdr->sh_name);
    if (strcmp(name, section_name) == 0) {
      printf("%s\n", name);
      sectionHeader *section = shdr;
      return section;
    }
  }

  return NULL;
}

void swap_sections(sectionHeader *section1, sectionHeader *section2) {
  // Get the ELF header
  elfHeader *ehdr = (Elf64_Ehdr *)target.map;

  // Get the section headers
  for (int i = 0; i < ehdr->e_shnum; i++) {
    sectionHeader *shdr = (sectionHeader *)((char *)target.map + ehdr->e_shoff +
                                            ehdr->e_shentsize * i);
    if (shdr->sh_offset == section1->sh_offset) {
      shdr->sh_offset = section2->sh_offset;
    } else if (shdr->sh_offset == section2->sh_offset) {
      shdr->sh_offset = section1->sh_offset;
    }
  }

  // Swap the sections
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
  sectionHeader *shdr = (sectionHeader *)((char *)target.map + ehdr->e_shoff);

  // Bubble sort
  for (int i = 0; i < ehdr->e_shnum; i++) {
    for (int j = 0; j < ehdr->e_shnum - i - 1; j++) {
      sectionHeader *section1 = shdr + j;
      sectionHeader *section2 = shdr + j + 1;
      if (section1->sh_offset > section2->sh_offset) {
        swap_sections(section1, section2);
      }
    }
  }

  // Reset the.shstrtab section index to the correct value
  for (int i = 0; i < ehdr->e_shnum; i++) {
    sectionHeader *shdr = (sectionHeader *)((char *)target.map + ehdr->e_shoff +
                                            ehdr->e_shentsize * i);
    // .shstrtab is the only section with sh_flags = 0 and sh_type = SHT_STRTAB
    if (shdr->sh_type == SHT_STRTAB && shdr->sh_flags == 0) {
      printf("shstrndx: %d\n", i);
      ehdr->e_shstrndx = i;
      break;
    }
  }
}
