#include <string.h>
#define PACKAGE "isos-inject"
#define PACKAGE_VERSION "1.0.0"

#include <argp.h>
#include <bfd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARG_BUFFER_SIZE 256

static struct argp_option options[] = {
    {"file", 'f', "FILE", 0, "ELF file that will be analyzed"},
    {"binary", 'b', "BINARY_FILE", 0,
     "Binary file that we will inject code into"},
    {"section", 's', "SECTION", 0, "Injected section's name"},
    {"address", 'a', "ADDRESS", 0, "Base address of the injected code"},
    {"entry", 'e', 0, 0, "Whether the entry point should be changed"},
    {"help", 'h', 0, 0, "Display this help message and exit."},
    {0}};

struct arguments {
  char file[ARG_BUFFER_SIZE];
  char binary[ARG_BUFFER_SIZE];
  char section[ARG_BUFFER_SIZE];
  char address[ARG_BUFFER_SIZE];
  bool entry;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'f':
    strncpy(arguments->file, arg, ARG_BUFFER_SIZE);
    break;
  case 'b':
    strncpy(arguments->binary, arg, ARG_BUFFER_SIZE);
    break;
  case 's':
    strncpy(arguments->section, arg, ARG_BUFFER_SIZE);
    break;
  case 'a':
    strncpy(arguments->address, arg, ARG_BUFFER_SIZE);
    break;
  case 'e':
    arguments->entry = true;
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
      strncpy(arguments->file, arg, ARG_BUFFER_SIZE);
      break;
    case 1:
      strncpy(arguments->binary, arg, ARG_BUFFER_SIZE);
      break;
    case 2:
      strncpy(arguments->section, arg, ARG_BUFFER_SIZE);
      break;
    case 3:
      strncpy(arguments->address, arg, ARG_BUFFER_SIZE);
      break;
    case 4:
      arguments->entry = true;
      break;
    }

    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static char args_doc[] = "ARG1 ARG2";
static char doc[] = "Argument 1 is the ELF file that will be analyzed. "
                    "Argument 2 is the binary ";

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
  struct arguments arguments;
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // printf("ARGS: %s %s %s %s %d\n", arguments.file, arguments.binary,
  // arguments.section, arguments.address, arguments.entry);

  bfd_init();

  bfd *file = bfd_openr(arguments.file, 0);
  if (!file)
    return EXIT_FAILURE;

  // Checking if binary is of format elf
  if (!bfd_check_format(file, bfd_object)) {
    bfd_perror("Error with binary");
    exit(EXIT_FAILURE);
  }

  // Checking if binary is 64 bit
  if (strcmp(bfd_printable_name(file), "i386:x86-64")) {
    fprintf(stderr, "Binary is not of 64 bits");
    exit(EXIT_FAILURE);
  }

  // Checking for exec rights on binary
  if (access(arguments.file, X_OK)) {
    perror("Error with binary");
    exit(EXIT_FAILURE);
  }

  bfd_close(file);

  return EXIT_SUCCESS;
}
