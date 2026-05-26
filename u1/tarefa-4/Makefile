CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp
LDFLAGS = -lm -fopenmp

MEMORY_SRC = memory_bound.c
COMPUTE_SRC = compute_bound.c

MEMORY_BIN = memory_bound
COMPUTE_BIN = compute_bound

all: $(MEMORY_BIN) $(COMPUTE_BIN)

$(MEMORY_BIN): $(MEMORY_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(COMPUTE_BIN): $(COMPUTE_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run-memory: $(MEMORY_BIN)
	./$(MEMORY_BIN)

run-compute: $(COMPUTE_BIN)
	./$(COMPUTE_BIN)

run: all
	@echo "=== Memory-bound ==="
	@./$(MEMORY_BIN)
	@echo ""
	@echo "=== Compute-bound ==="
	@./$(COMPUTE_BIN)

clean:
	rm -f $(MEMORY_BIN) $(COMPUTE_BIN)

.PHONY: all run run-memory run-compute clean
