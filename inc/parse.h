#pragma once

#include "defs.h"
#include <argp.h>
#include <elf.h>
#include <stdbool.h>

cliArgs get_args(int argc, char **argv);
void check_binary(cliArgs args);
