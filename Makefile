vpath %.c ./src
CC = gcc
INC_DIR:= inc
SRCS := isos_inject.c parse.c inject.c utils.c
OBJS=$(SRCS:%.c=obj/%.o)
DEPS=$(wildcard $(INC_DIR)/*.h)
LIBS = -lbfd
CFLAGS = -Wall -g -pedantic -Wextra -I$(INC_DIR) 

all: bin/isos_inject bin/inject

obj/%.o: src/%.c $(DEPS)
	@$(CC) -c -o $@ $< $(CFLAGS)

bin/isos_inject: $(OBJS)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

bin/inject: inject.s
	nasm -f bin $^ -o $@

.PHONY: check clean

check:
	clang-tidy $(SRCS:%.c=src/%.c) $(DEPS) -checks=clang-analyzer-*,bugprone-\*,modernize-\*,performance-\*,readability-\*,-readability-magic-numbers -- -I$(INC_DIR)
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ./bin/isos_inject
clean:
	rm -f obj/*.o bin/isos_inject make.test Makefile.pipeline
	rm -rf build_pipeline

###### CI Pipeline ######

SRC_FILES := $(SRCS)
INCLUDE_DIR:= $(INC_DIR)

%.dep: %.c
	@$(CC) -I$(INCLUDE_DIR) -MM -MF $@ $<
	
build_dependencies: $(SRC_FILES:%.c=%.dep)
	@cat $^ > make.test
	@rm $^
