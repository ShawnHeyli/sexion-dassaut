#define PACKAGE "isos-inject"
#define PACKAGE_VERSION "1.0.0"

#include "../inc/arg_parse.h"
#include <argp.h>
#include <bfd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  uint32_t p_type;
  uint32_t p_offset;
  uint32_t p_va;
  uint32_t p_pa;
  uint32_t p_filesz;
  uint32_t p_memsz;
  uint32_t p_flags;
  uint32_t p_align;
} elfHeader;

elfHeader parse_header_prog(cliArgs args) {
  elfHeader header;
  FILE *file = fopen(args.file, "r");
  fread(&header, sizeof(header), 1, file);
  printf("HEADER: %d", header.p_type);
  fclose(file);
  return header;
}

void check_binary(cliArgs args){
  bfd *file = bfd_openr(args.file, 0);
  if (!file){
    bfd_perror("Error with binary");
    exit(EXIT_FAILURE);
  }

  // Checking if binary is of format elf
  if (!bfd_check_format(file, bfd_object)) {
    bfd_perror("Error with binary");
    exit(EXIT_FAILURE);
  }

  printf("%d", bfd_get_arch(file));

  // Checking if binary is 64 bit

  const bfd_arch_info_type *arch_info = bfd_get_arch_info(file);
  if (arch_info->bits_per_address != 64) {
    fprintf(stderr, "Binary is not of 64 bits");
    exit(EXIT_FAILURE);
  }

  // Checking for exec rights on binary
  if (file->flags & EXEC_P >> 1) {
    fprintf(stderr, "Error with binary : Not executable");
    exit(EXIT_FAILURE);
  }

  bfd_close(file);
}

int main(int argc, char **argv) {
  cliArgs args = get_args(argc, argv);

  // printf("ARGS: %s %s %s %s %d\n", args.file, args.binary,
  // args.section, args.address, args.entry);

  bfd_init();


  parse_header_prog(args);


  return EXIT_SUCCESS;
}
