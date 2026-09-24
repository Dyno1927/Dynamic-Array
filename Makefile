CC      = gcc
CFLAGS  = -Wall -Wextra -g -O0 -Iinclude
SAN     = -fsanitize=address,undefined

build/vec: build/main.o build/vec.o
	$(CC) $(CFLAGS) -o $@ $^

build/main.o: src/main.c include/vec.h
	$(CC) $(CFLAGS) -c src/main.c -o $@

build/vec.o: src/vec.c include/vec.h
	$(CC) $(CFLAGS) -c src/vec.c -o $@

.PHONY: clean
clean:
	rm -f build/*.o build/vec
