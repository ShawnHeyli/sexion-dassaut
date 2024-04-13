#include "defs.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

extern fileMapping target;
extern fileMapping payload;

int inject_section(cliArgs *args) {

  long offset = ftell(target.file);

  // Append binary at the end of target.file
  fseek(target.file, 0, SEEK_END);
  fseek(payload.file, 0, SEEK_SET);

  char buffer[256];
  size_t bytes;
  while ((bytes = fread(buffer, 1, sizeof(buffer), payload.file)) > 0) {
    fwrite(buffer, 1, bytes, target.file);
  }

  long unsigned res = (args->address - offset) % 4096;
  if (res != 0) {
    args->address += res;
  }

  printf("%lu\n", args->address);
  return offset;
}

void modify_section_header(cliArgs args, Elf64_Shdr *section, long offset) {
  // sh_type to SHT_PROGBITS
  section->sh_type = SHT_PROGBITS;

  // sh_addr to new address
  section->sh_addr = args.address;
  // sh_offset to offset
  section->sh_offset = offset;

  // sh_size to size of inject file
  section->sh_size = payload.sb.st_size;

  // sh_addralgin to 16
  section->sh_addralign = 16;

  // sh_flags add SHF_EXECINSTR
  section->sh_flags |= SHF_EXECINSTR;
}
