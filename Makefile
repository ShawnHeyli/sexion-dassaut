CC = gcc
CFLAGS = -Wall -pedantic -Iinc
LIBS = -liberty -lz -lbfd
SRC_FILES := src/isos-inject.c

all: bin/isos_inject

obj/%.o: src/%.c
		$(CC) -c -o $@ $< $(CFLAGS)

bin/isos_inject: obj/isos_inject.o obj/arg_parse.o inc/arg_parse.h
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
		rm -f obj/*.o bin/isos_inject

build_dependencies: $(SRC_FILES:.c=.dep)
		@cat $^ > make.test
		@rm $^

%.dep: %.c
		@gcc -MM -MF $@ $<
