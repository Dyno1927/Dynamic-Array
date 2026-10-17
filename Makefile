CC      = clang
CFLAGS  = -Wall -Wextra -g -O0 -Iinclude
SAN     = -fsanitize=address,undefined

build/vec: build/test.o build/vec.o
	$(CC) $(CFLAGS) -o $@ $^

build/test.o: tests/test.c include/vec.h
	$(CC) $(CFLAGS) -c tests/test.c -o $@

build/vec.o: src/vec.c include/vec.h
	$(CC) $(CFLAGS) -c src/vec.c -o $@

.PHONY: clean
clean:
	rm -f build/*.o build/vec
