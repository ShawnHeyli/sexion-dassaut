#include "parse.h"
#include <bfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern fileMapping target;

static struct argp_option options[] = {
    {"file", 'f', "FILE", 0, "ELF file that we inject code into", 0},
    {"to-inject", 'b', "BINARY_FILE", 0,
     "Binary file that we will inject into the file", 0},
    {"section-name", 's', "SECTION", 0, "Injected section's name", 0},
    {"base-addr", 'a', "ADDRESS", 0, "Base address of the injected code", 0},
    {"modify-entry", 'e', "ENTRY", 0,
     "Whether the entry point should be changed", 0},
    {"help", 'h', 0, 0, "Display this help message and exit.", 0},
    {0}};

static char args_doc[] = "ARG1 ARG2 ARG3 ARG4 ARG5 ARG6";
static char doc[] =
    "Arg 1 is the ELF file that will have binary injected into"
    "Arg 2 is the binary that will be injected"
    "Arg 3 is the name of the inject section"
    "Arg 4 is the address at which the section will be injected"
    "Arg 5 is whether the entry point of the program shall be changed";

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  cliArgs *args = state->input;

  switch (key) {
  case 'f':
    args->file = arg;
    break;
  case 'b':
    args->binary = arg;
    break;
  case 's':
    args->section = arg;
    break;
  case 'a':
    args->address = strtoul(arg, NULL, 16);
    break;
  case 'e':
    args->entry = (strcmp("1", arg) == 0) ? true : false;
    break;
  case 'h':
    argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
    break;
  case ARGP_KEY_ARG:
    if (state->arg_num > 5) {
      fprintf(stderr, "Too many arguments !");
      argp_usage(state);
    }

    switch (state->arg_num) {
    case 0:
      args->file = arg;
      break;
    case 1:
      args->binary = arg;
      break;
    case 2:
      args->section = arg;
      break;
    case 3:
      args->address = strtoul(arg, NULL, 16);
      break;
    case 4:
      args->entry = (strcmp("1", arg) == 0) ? true : false;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }

    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

cliArgs get_args(int argc, char **argv) {
  cliArgs args;
  argp_parse(&argp, argc, argv, 0, 0, &args);
  return args;
}

bfd *get_bfd_file(const char *file) {
  bfd *bfd = bfd_openr(file, 0);
  if (!bfd) {
    bfd_perror("Error with binary");
    exit(EXIT_FAILURE);
  }
  return bfd;
}

void check_binary(cliArgs args) {
  bfd *file = get_bfd_file(args.file);

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
  lseek(fileno(file), (long)file_header.e_phoff,
        SEEK_SET); // Skipping file headers

  // Go over each program header & check type field to see if it's a PT_NOTE
  for (int i = 0; i < file_header.e_phnum; i++) {
    fread(&prog_header, sizeof(prog_header), 1, file);
    if (prog_header.p_type == PT_NOTE) {
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
