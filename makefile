# Definir ejecutables para cada archivo
JUEGO = juego
OBSERVADOR = observador
AMURRA = amurra_y_reclama

# Compilador y banderas de compilación
CC = g++
CFLAGS = -std=c++11

# Archivos fuente
SRCS_JUEGO = juego.cpp
SRCS_OBSERVADOR = observador.cpp
SRCS_AMURRA = amurra_y_reclama.cpp

# Regla principal para compilar los tres ejecutables
all: $(JUEGO) $(OBSERVADOR) $(AMURRA)

# Regla para compilar juego
$(JUEGO): $(SRCS_JUEGO)
	$(CC) $(CFLAGS) -o $(JUEGO) $(SRCS_JUEGO)

# Regla para compilar observador
$(OBSERVADOR): $(SRCS_OBSERVADOR)
	$(CC) $(CFLAGS) -o $(OBSERVADOR) $(SRCS_OBSERVADOR)

# Regla para compilar amurra_y_reclama
$(AMURRA): $(SRCS_AMURRA)
	$(CC) $(CFLAGS) -o $(AMURRA) $(SRCS_AMURRA)

# Limpiar archivos binarios
clean:
	rm -f $(JUEGO) $(OBSERVADOR) $(AMURRA)

# Ejecutar el programa principal
run-juego: $(JUEGO)
	./$(JUEGO)

# Ejecutar observador
run-observador: $(OBSERVADOR)
	./$(OBSERVADOR)

# Ejecutar amurra_y_reclama
run-amurra: $(AMURRA)
	./$(AMURRA)
