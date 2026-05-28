CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp
LDFLAGS = -fopenmp

SRC = prime_count.c
BIN = prime_count

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
