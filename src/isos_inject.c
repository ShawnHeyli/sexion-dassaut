#include <sys/stat.h>
#define PACKAGE "isos-inject"
#define PACKAGE_VERSION "1.0.0"

#include "defs.h"
#include "inject.h"
#include "parse.h"
#include "utils.h"
#include <argp.h>
#include <assert.h>
#include <bfd.h>
#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

fileMapping target;
fileMapping payload;

Elf64_Shdr *get_section_by_name(cliArgs args, char *section_name, long offset) {
  // Get the ELF header
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)target.map;
  print_file_header(*ehdr);
  puts("\n");

  // Get shstrtab section header
  Elf64_Shdr *shstrtab = (Elf64_Shdr *)((char *)target.map + ehdr->e_shoff +
                                        ehdr->e_shentsize * ehdr->e_shstrndx);
  print_section_header(*shstrtab);
  puts("\n");

  // Get the section by name
  Elf64_Shdr *section = NULL;
  for (int i = 0; i < ehdr->e_shnum; i++) {
    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)target.map + ehdr->e_shoff +
                                      ehdr->e_shentsize * i);
    char *name =
        (char *)((char *)target.map + shstrtab->sh_offset + shdr->sh_name);
    if (strcmp(name, section_name) == 0) {
      section = shdr;

      puts("BEFORE\n");
      print_section_header(*section);

      // sh_type to SHT_PROGBITS
      section->sh_type = SHT_PROGBITS;
      puts("sh_type modified to SHT_PROGBITS");

      // sh_addr to new address
      section->sh_addr = args.address;
      puts("sh_addr modified to new address");
      // sh_offset to offset
      section->sh_offset = offset;
      puts("sh_offset modified to offset");
      // sh_size to size of inject file
      section->sh_size = target.sb.st_size;
      puts("sh_size modified to size of inject file");

      // sh_addralgin to 16
      section->sh_addralign = 16;
      puts("sh_addralign modified to 16");

      // sh_flags add SHF_EXECINSTR
      section->sh_flags |= SHF_EXECINSTR;
      puts("sh_flags modified to SHF_EXECINSTR");

      puts("AFTER\n");
      print_section_header(*section);

      return section;
    }
  }
  return NULL;
}

void set_global_map(const char *path_target, const char *path_inject) {
  FILE *file_target = fopen(path_target, "rb+");
  if (file_target == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  // Get the file size
  struct stat sb_target;
  if (fstat(fileno(file_target), &sb_target) == -1) {
    perror("fstat");
    exit(EXIT_FAILURE);
  }

  // Memory-map the file (RW)
  void *map_target = mmap(NULL, sb_target.st_size, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fileno(file_target), 0);
  if (map_target == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  target =
      (fileMapping){.map = map_target, .file = file_target, .sb = sb_target};

  FILE *file_inject = fopen(path_inject, "rb+");
  if (file_inject == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  // Get the file size
  struct stat sb_inject;
  if (fstat(fileno(file_inject), &sb_inject) == -1) {
    perror("fstat");
    exit(EXIT_FAILURE);
  }

  // Memory-map the file (RW)
  void *map_inject = mmap(NULL, sb_inject.st_size, PROT_READ, MAP_PRIVATE,
                          fileno(file_inject), 0);
  if (map_inject == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  payload =
      (fileMapping){.map = map_inject, .file = file_inject, .sb = sb_inject};
}

void deallocate_global_map() {
  if (munmap(target.map, target.sb.st_size) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }
  fclose(target.file);
}

void init(cliArgs args) {
  set_global_map(args.file, args.binary);
  bfd_init(); // Could not find what kind of error it returns (noted as void in
              // source code, but not in header ??)
  check_binary(args);
}

int main(int argc, char **argv) {
  cliArgs args = get_args(argc, argv);
  init(args);

  // int pt_note_index = get_pt_note(args);
  int inject_offset = inject_section(&args);

  Elf64_Shdr *section =
      get_section_by_name(args, ".note.ABI-tag", inject_offset);

  deallocate_global_map();
  return EXIT_SUCCESS;
}
