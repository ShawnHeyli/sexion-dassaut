#include <sys/stat.h>
#define PACKAGE "isos-inject"
#define PACKAGE_VERSION "1.0.0"

#include "arg_parse.h"
#include "header_parse.h"
#include <argp.h>
#include <bfd.h>
#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void check_binary(cliArgs args) {
  bfd *file = bfd_openr(args.file, 0);
  if (!file) {
    bfd_perror("Error with binary");
    bfd_close(file);
    exit(EXIT_FAILURE);
  }

  // Checking if binary is of format elf
  if (!bfd_check_format(file, bfd_object)) {
    bfd_perror("Error with binary");
    bfd_close(file);
    exit(EXIT_FAILURE);
  }

  // Checking if binary is 64 bit
  const bfd_arch_info_type *arch_info = bfd_get_arch_info(file);
  if (arch_info->bits_per_address != 64) {
    fprintf(stderr, "Binary is not of 64 bits");
    bfd_close(file);
    exit(EXIT_FAILURE);
  }

  // Checking for exec rights on binary
  if (file->flags & EXEC_P >> 1) {
    fprintf(stderr, "Error with binary : Not executable");
    bfd_close(file);
    exit(EXIT_FAILURE);
  }

  bfd_close(file);
}

int inject_section(cliArgs *args) {
  FILE *file = fopen(args->file, "ab");
  if (file == NULL) {
    perror("Error with binary");
    exit(EXIT_FAILURE);
  }
  FILE *inject = fopen(args->binary, "rb");
  if (inject == NULL) {
    perror("Error with binary");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  long offset = ftell(file);

  // Append binary at the end of file
  fseek(file, 0, SEEK_END);
  fseek(inject, 0, SEEK_SET);

  char buffer[256];
  size_t bytes;
  while ((bytes = fread(buffer, 1, sizeof(buffer), inject)) > 0) {
    fwrite(buffer, 1, bytes, file);
  }

  long unsigned res = (args->address - offset) % 4096;
  if (res != 0) {
    args->address += res;
  }

  fclose(file);
  fclose(inject);
  return offset;
}

int main(int argc, char **argv) {
  cliArgs args = get_args(argc, argv);

  // printf("ARGS: %s %s %s %lu %d\n", args.file, args.binary, args.section,
  //        args.address, args.entry);

  bfd_init();
  check_binary(args); // Will crash if error

  int pt_note_index = get_pt_note(args);
  int inject_offset = inject_section(&args);

  return EXIT_SUCCESS;
}
