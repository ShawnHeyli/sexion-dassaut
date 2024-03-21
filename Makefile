CC = gcc
CFLAGS = -Wall -pedantic
LIBS = -liberty -lz -lbfd

all: bin/isos_inject

obj/%.o: src/%.c
		$(CC) -c -o $@ $< $(CFLAGS)

bin/isos_inject: obj/isos_inject.o 
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
		rm -f obj/*.o bin/isos_inject
