CC=clang
EXE=test
CFLAGS=-Werror -Wall 
OBJ=tshashtable.o
CFILE=tshashtable.c

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

