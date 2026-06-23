CC = gcc
CFLAGS = -Wall -Wextra -O3 -fopenmp
TARGET = pi_sincronizacao

.PHONY: all clean run

all: $(TARGET)

$(TARGET): pi_sincronizacao.c
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
