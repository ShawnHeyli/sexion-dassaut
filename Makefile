vpath %.c ./src
CC = gcc
INC_DIR:= inc
SRCS := isos_inject.c arg_parse.c header_parse.c 
OBJS=$(SRCS:%.c=obj/%.o)
DEPS=$(wildcard $(INC_DIR)/*.h)

LIBS = -lbfd
CFLAGS = -Wall -pedantic -Wextra -I$(INC_DIR) 

all: bin/isos_inject

obj/%.o: src/%.c $(DEPS)
	@$(CC) -c -o $@ $< $(CFLAGS)
	clang-tidy $< -checks=clang-analyzer-* -- I$(INC_DIR)

bin/isos_inject: $(OBJS)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: CLEAN

clean:
	rm -f obj/*.o bin/isos_inject
	rm -rf build_pipeline artifacts

###### CI Pipeline ######

SRC_FILES := $(SRCS)

%.dep: %.c
	@$(CC) -I$(INC_DIR) -MM -MF $@ $<
	
build_dependencies: $(SRC_FILES:%.c=%.dep)
	@cat $^ > make.test
	@rm $^
