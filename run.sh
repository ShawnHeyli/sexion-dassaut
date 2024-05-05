### ANALYZE ###
cp bin/date.bak bin/date && instrumented_binary_analyze/isos_addr_san -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_analyze/isos_mem_san -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_analyze/isos_ub_san -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"

cp bin/date.bak bin/date && instrumented_binary_analyze/isos_addr_san -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_analyze/isos_mem_san -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_analyze/isos_ub_san -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"

### BUILD ###
cp bin/date.bak bin/date && instrumented_binary_build/isos_addr_san -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_build/isos_mem_san -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_build/isos_ub_san -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"

cp bin/date.bak bin/date && instrumented_binary_build/isos_addr_san -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_build/isos_mem_san -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"
cp bin/date.bak bin/date && instrumented_binary_build/isos_ub_san -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"

