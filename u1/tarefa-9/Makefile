CC = gcc
CFLAGS = -Wall -Wextra -fopenmp
TARGETS = tarefa9_2listas tarefa9_Mlistas

# Número padrão de listas para a versão generalizada
M ?= 5

.PHONY: all clean run run-2listas run-Mlistas

all: $(TARGETS)

tarefa9_2listas: tarefa9_2listas.c
	$(CC) $(CFLAGS) $< -o $@

tarefa9_Mlistas: tarefa9_Mlistas.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	@echo "=== Executando versão com 2 listas (Named Critical Sections) ==="
	./tarefa9_2listas
	@echo ""
	@echo "=== Executando versão com M listas (Locks Explícitos, M=$(M)) ==="
	./tarefa9_Mlistas $(M)

run-2listas: tarefa9_2listas
	./tarefa9_2listas

run-Mlistas: tarefa9_Mlistas
	./tarefa9_Mlistas $(M)

clean:
	rm -f $(TARGETS)
