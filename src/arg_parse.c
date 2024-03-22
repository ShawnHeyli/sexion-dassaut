#include <string.h>
#define PACKAGE "isos-inject"
#define PACKAGE_VERSION "1.0.0"

#include <argp.h>
#include "../inc/arg_parse.h"
#include <bfd.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


static struct argp_option options[] = {
    {"file", 'f', "FILE", 0, "ELF file that will be analyzed"},
    {"binary", 'b', "BINARY_FILE", 0,
     "Binary file that we will inject code into"},
    {"section", 's', "SECTION", 0, "Injected section's name"},
    {"address", 'a', "ADDRESS", 0, "Base address of the injected code"},
    {"entry", 'e', 0, 0, "Whether the entry point should be changed"},
    {"help", 'h', 0, 0, "Display this help message and exit."},
    {0}
};

static char args_doc[] = "ARG1 ARG2";
static char doc[] = "Argument 1 is the ELF file that will be analyzed. "
                    "Argument 2 is the binary ";

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  cliArgs *args = state->input;

  switch (key) {
  case 'f':
    strncpy(args->file, arg, ARG_BUFFER_SIZE);
    break;
  case 'b':
    strncpy(args->binary, arg, ARG_BUFFER_SIZE);
    break;
  case 's':
    strncpy(args->section, arg, ARG_BUFFER_SIZE);
    break;
  case 'a':
    strncpy(args->address, arg, ARG_BUFFER_SIZE);
    break;
  case 'e':
    args->entry = true;
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
      strncpy(args->file, arg, ARG_BUFFER_SIZE);
      break;
    case 1:
      strncpy(args->binary, arg, ARG_BUFFER_SIZE);
      break;
    case 2:
      strncpy(args->section, arg, ARG_BUFFER_SIZE);
      break;
    case 3:
      strncpy(args->address, arg, ARG_BUFFER_SIZE);
      break;
    case 4:
      args->entry = true;
      break;
    }

    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}


static struct argp argp = {options, parse_opt, args_doc, doc};

cliArgs get_args(int argc, char** argv){
  cliArgs args;
  argp_parse(&argp, argc, argv, 0, 0, &args);
  return args;
}
