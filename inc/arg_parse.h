#ifndef ARG_PARSE
#define ARG_PARSE

#include <stdint.h>
#define ARG_BUFFER_SIZE 256

#include <argp.h>
#include <stdbool.h>

typedef struct {
  char file[ARG_BUFFER_SIZE];
  char binary[ARG_BUFFER_SIZE];
  char section[ARG_BUFFER_SIZE];
  uint64_t address;
  bool entry;
} cliArgs;

cliArgs get_args(int argc, char **argv);

#endif
