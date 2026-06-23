CC = gcc
CFLAGS = -Wall -O3 -fopenmp
TARGET = sim_fluid
TARGET_3D = sim_fluid_3d

all: $(TARGET) $(TARGET_3D)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

$(TARGET_3D): main_3d.c
	$(CC) $(CFLAGS) -o $(TARGET_3D) main_3d.c

clean:
	rm -f $(TARGET) $(TARGET_3D)

run: $(TARGET)
	@echo "\n=== Executando 2D com particionamento padrão ==="
	./$(TARGET)
	@echo "\n=== Executando 2D com particionamento estático (Blocos) ==="
	OMP_SCHEDULE="static" ./$(TARGET)
	@echo "\n=== Executando 2D com particionamento dinâmico ==="
	OMP_SCHEDULE="dynamic" ./$(TARGET)

run3d: $(TARGET_3D)
	@echo "\n=== Executando 3D com particionamento padrão ==="
	./$(TARGET_3D)
	@echo "\n=== Executando 3D com particionamento estático ==="
	OMP_SCHEDULE="static" ./$(TARGET_3D)
	@echo "\n=== Executando 3D com particionamento dinâmico ==="
	OMP_SCHEDULE="dynamic" ./$(TARGET_3D)
