#include "header_parse.h"
#include "arg_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_file_header(elfHeader file) {
  printf("ELF Header:\n");
  printf("Magic: ");
  for (int i = 0; i < 16; i++) {
    printf("%02x ", file.magic[i]);
  }
  printf("\n");
  printf("Type: %hx\n", file.type);
  printf("Machine: %hx\n", file.arch);
  printf("Version: 0x%x\n", file.version);
  printf("Entry point address: 0x%lx\n", file.entrypoint);
  printf("Start of program headers: %lu\n", file.ph_offset);
  printf("Start of section headers: %lu\n", file.sh_offset);
  printf("Flags: %x\n", file.flags);
  printf("Size of this header: %hu\n", file.eh_size);
  printf("Size of program headers: %hu\n", file.phe_size);
  printf("Number of program headers: %hu\n", file.phe_count);
  printf("Size of section headers: %hu\n", file.she_size);
  printf("Number of section headers: %hu\n", file.she_size);
  printf("Section header string table index: %hu\n", file.she_index);
}

int parse_prog_header(cliArgs args) {
  elfHeader file_header;
  progHeader prog_header;
  FILE *file = fopen(args.file, "rb");
  if (file == NULL) {
    perror("Error with binary");
    exit(EXIT_FAILURE);
  }

  // Get file header
  fread(&file_header, sizeof(file_header), 1, file);
  //print_file_header(file_header);

  // Getting program header offset (size of file headers)
  lseek(fileno(file), file_header.ph_offset, SEEK_SET); // Skipping file headers
  // Go over each program header & check type field to see if it's a PT_NOTE

  for (int i = 0; i < file_header.phe_count; i++) {
    fread(&prog_header, sizeof(prog_header), 1, file);
    if (prog_header.type == PT_NOTE) {
      //puts("Found PT_NOTE header\n");
      fclose(file);
      return i;
    }
  }

  //puts("Did not find a PT_NOTE section\n");
  fclose(file);
  return -1;
}
