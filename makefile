CC=gcc
EXE=test
CFLAGS=-Wall -std=c99
OBJ=hashmap.o
CFILE=hashmap.c

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFILE)

.PHONY: clean run

clean:
	rm *.o
	rm $(EXE)

run:
	./$(EXE)

