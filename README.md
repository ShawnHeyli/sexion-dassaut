# Sexion d'assaut

This tool can statically modify an ELF binary to run arbitrary code when the original executable is run.

[![A presentation video about the tool]()](./presentation.mov)

## The goal
This repository contains a school project focused on exploring and modifying the ELF structure of binaries. 
The primary goal is to understand the internal workings of ELF files and demonstrate the ability to statically modify a binary to alter its behavior.

# Examples

### Modify the entrypoint
```cp bin/date.bak bin/date && bear -- make all && bin/isos_inject -f bin/date -b bin/entrypoint -s ".injected" -a 0x40000 -e 1 && chmod 755 bin/date && bin/date```

### Modify the GOT
```cp bin/date.bak bin/date && bear -- make all && bin/isos_inject -f bin/date -b bin/got_breaker -s ".injected" -a 0x40000 -e 0 && chmod 755 bin/date && bin/date```
