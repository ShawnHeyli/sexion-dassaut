#pragma once
#include <elf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

typedef struct {
  void *map;      // mapped file
  FILE *file;     // related file
  struct stat sb; // stats about the file (mostly for munmap)
} fileMapping;

typedef struct {
  char *file;
  char *binary;
  char *section;
  uint64_t address;
  bool entry;
} cliArgs;

typedef Elf64_Ehdr elfHeader;
typedef Elf64_Phdr progHeader;
typedef Elf64_Shdr sectionHeader;
