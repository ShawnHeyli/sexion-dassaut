#include <err.h>
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

  long inject_offset = inject_section(&args);

  sectionHeader *section = get_section_by_name(".note.ABI-tag");
  if (section == NULL) {
    errx(EXIT_FAILURE, "Could not get section by name");
  }

  modify_section_header(args.address, section, inject_offset);

  set_section_name(section, args.section);
  sort_section_headers();
  // *section is unusable from this point on because the section headers have
  // been reordered

  sectionHeader *new_section = get_section_by_name(args.section);

  progHeader *phdr = get_pt_note();

  overwrite_pt_note(phdr, *new_section);
  if (section == NULL) {
    errx(EXIT_FAILURE, "Could not get pt_note");
  }

  deallocate_global_map();
  return EXIT_SUCCESS;
}
