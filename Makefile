vpath %.c ./src
CC = gcc
INC_DIR:= inc
SRCS := isos_inject.c parse.c inject.c utils.c
OBJS=$(SRCS:%.c=obj/%.o)
DEPS=$(wildcard $(INC_DIR)/*.h)
LIBS = -lbfd
CFLAGS = -Wall -Wextra -Warray-bounds -Wsequence-point -Walloc-zero -Wnull-dereference -Wpointer-arith -Wcast-qual -Wcast-align=strict -I$(INC_DIR) 

all: bin/isos_inject bin/entrypoint bin/got_breaker

obj/%.o: src/%.c $(DEPS)
	@$(CC) -c -o $@ $< $(CFLAGS)

bin/isos_inject: $(OBJS)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

bin/entrypoint: entrypoint.s
	nasm -f bin $^ -o $@
	
bin/got_breaker: got_breaker.s
	nasm -f bin $^ -o $@

.PHONY: check clean

check:
	clang-tidy $(SRCS:%.c=src/%.c) $(DEPS) -checks="clang-analyzer-*" -- -I$(INC_DIR)
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         bin/isos_inject -a 0x40000 -b bin/entrypoint -e 1 -f bin/date -s "zoubida"
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         bin/isos_inject -a 0x40000 -b bin/got_breaker -e 0 -f bin/date -s "zoubida"
clean:
	rm -f obj/*.o make.test Makefile.pipeline
	find ./bin/ ! -name 'date.bak' -type f -exec rm -f {} +
	rm -rf build_pipeline

###### CI Pipeline ######

SRC_FILES := $(SRCS)
INCLUDE_DIR:= $(INC_DIR)

%.dep: %.c
	@$(CC) -I$(INCLUDE_DIR) -MM -MF $@ $<
	
build_dependencies: $(SRC_FILES:%.c=%.dep)
	@cat $^ > make.test
	@rm $^
