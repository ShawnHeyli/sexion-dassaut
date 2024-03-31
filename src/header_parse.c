#include "../inc/header_parse.h"
#include "../inc/arg_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_file_header(elfHeader header) {
  printf("ELF Header:\n");
  printf("Magic: ");
  for (int i = 0; i < 16; i++) {
    printf("%02x ", header.magic[i]);
  }
  printf("\n");
  printf("Type: %hx\n", header.type);
  printf("Machine: %hx\n", header.arch);
  printf("Version: 0x%x\n", header.version);
  printf("Entry point address: 0x%lx\n", header.entrypoint);
  printf("Start of program headers: %lu\n", header.ph_offset);
  printf("Start of section headers: %lu\n", header.sh_offset);
  printf("Flags: %x\n", header.flags);
  printf("Size of this header: %hu\n", header.eh_size);
  printf("Size of program headers: %hu\n", header.phe_size);
  printf("Number of program headers: %hu\n", header.phe_count);
  printf("Size of section headers: %hu\n", header.she_size);
  printf("Number of section headers: %hu\n", header.she_size);
  printf("Section header string table index: %hu\n", header.she_index);
}

int get_pt_note(cliArgs args) {
  elfHeader file_header;
  progHeader prog_header;
  FILE *file = fopen(args.file, "rb");
  if (file == NULL) {
    perror("Error with binary");
    exit(EXIT_FAILURE);
  }

  // Get file header
  fread(&file_header, sizeof(file_header), 1, file);
  // print_file_header(file_header);

  // Getting program header offset (size of file headers)
  lseek(fileno(file), (long)file_header.ph_offset,
        SEEK_SET); // Skipping file headers

  // Go over each program header & check type field to see if it's a PT_NOTE
  for (int i = 0; i < file_header.phe_count; i++) {
    fread(&prog_header, sizeof(prog_header), 1, file);
    if (prog_header.type == PT_NOTE) {
      // puts("Found PT_NOTE header\n");
      // printf("%d", i);
      fclose(file);
      return i;
    }
  }

  // puts("Did not find a PT_NOTE section\n");
  fclose(file);
  return -1;
}
