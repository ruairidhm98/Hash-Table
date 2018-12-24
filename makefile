CC=clang
EXE=test
CFLAGS=-Werror -Wall 
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

