CC = gcc
CFLAGS_COMMON = -Wall -Wextra -lm
SRC = main.c

# Binários para cada nível de otimização
BIN_O0 = ilp_O0
BIN_O2 = ilp_O2
BIN_O3 = ilp_O3

all: $(BIN_O0) $(BIN_O2) $(BIN_O3)

$(BIN_O0): $(SRC)
	$(CC) -O0 $(CFLAGS_COMMON) -o $@ $<

$(BIN_O2): $(SRC)
	$(CC) -O2 $(CFLAGS_COMMON) -o $@ $<

$(BIN_O3): $(SRC)
	$(CC) -O3 $(CFLAGS_COMMON) -o $@ $<

run: all
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════╗"
	@echo "║           COMPILADO COM -O0 (sem otimização)             ║"
	@echo "╚═══════════════════════════════════════════════════════════╝"
	@./$(BIN_O0)
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════╗"
	@echo "║           COMPILADO COM -O2 (otimização média)           ║"
	@echo "╚═══════════════════════════════════════════════════════════╝"
	@./$(BIN_O2)
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════╗"
	@echo "║           COMPILADO COM -O3 (otimização agressiva)       ║"
	@echo "╚═══════════════════════════════════════════════════════════╝"
	@./$(BIN_O3)

clean:
	rm -f $(BIN_O0) $(BIN_O2) $(BIN_O3)

.PHONY: all run clean
