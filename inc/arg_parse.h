#ifndef ARG_PARSE
#define ARG_PARSE

#include <argp.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char *file;
  char *binary;
  char *section;
  uint64_t address;
  bool entry;
} cliArgs;

cliArgs get_args(int argc, char **argv);

#endif
