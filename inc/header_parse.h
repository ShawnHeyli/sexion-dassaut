#include "arg_parse.h"
#include <elf.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
  unsigned char magic[16]; /* Magic number and other info */
  uint16_t type;           /* Object file type */
  uint16_t arch;           /* Architecture */
  uint32_t version;        /* Object file version */
  uint64_t entrypoint;     /* Entry point virtual address */
  uint64_t ph_offset;      /* Program header table file offset */
  uint64_t sh_offset;      /* Section header table file offset */
  uint32_t flags;          /* Processor-specific flags */
  uint16_t eh_size;        /* ELF header size in bytes */
  uint16_t phe_size;       /* Program header table entry size */
  uint16_t phe_count;      /* Program header table entry count */
  uint16_t she_size;       /* Section header table entry size */
  uint16_t she_count;      /* Section header table entry count */
  uint16_t she_index;      /* Section header string table index */
} elfHeader;

typedef struct {
  uint32_t type;      /* Segment type */
  uint64_t offset;    /* Segment file offset */
  uint32_t virt_addr; /* Segment virtual address */
  uint32_t phys_addr; /* Segment physical address */
  uint32_t file_size; /* Segment size in file */
  uint32_t mem_size;  /* Segment size in memory */
  uint32_t flags;     /* Segment flags */
  uint32_t align;     /* Segment alignment */
} progHeader;

void print_file_header(elfHeader header);
int get_pt_note(cliArgs args);
