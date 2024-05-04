### Modify the entrypoint
```cp bin/date.bak bin/date && bear -- make all && bin/isos_inject -f bin/date -b bin/entrypoint -s ".zoubida" -a 0x40000 -e 1 && chmod 755 bin/date && bin/date```

### Modify the GOT
```cp bin/date.bak bin/date && bear -- make all && bin/isos_inject -f bin/date -b bin/got_breaker -s ".zoubida" -a 0x40000 -e 0 && chmod 755 bin/date && bin/date```
