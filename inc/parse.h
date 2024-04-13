#pragma once

#include "defs.h"
#include <argp.h>
#include <stdbool.h>
#include <stdint.h>
#include <elf.h>

cliArgs get_args(int argc, char **argv);
void check_binary(cliArgs args);
int get_pt_note(cliArgs args);
