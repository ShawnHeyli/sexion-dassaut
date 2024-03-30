#define PACKAGE "isos-inject"
#define PACKAGE_VERSION "1.0.0"

#include "../inc/arg_parse.h"
#include <argp.h>
#include <bfd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct argp_option options[] = {
    {"file", 'f', "FILE", 0, "ELF file that will be analyzed", 0},
    {"binary", 'b', "BINARY_FILE", 0,
     "Binary file that we will inject code into", 0},
    {"section", 's', "SECTION", 0, "Injected section's name", 0},
    {"address", 'a', "ADDRESS", 0, "Base address of the injected code", 0},
    {"entry", 'e', 0, 0, "Whether the entry point should be changed", 0},
    {"help", 'h', 0, 0, "Display this help message and exit.", 0},
    {0}};

static char args_doc[] = "ARG1 ARG2";
static char doc[] = "Argument 1 is the ELF file that will be analyzed. "
                    "Argument 2 is the binary ";

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
      args->entry = true;
      break;
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
