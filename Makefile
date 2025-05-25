# Compilador y flags
CXX = g++
CXXFLAGS = -O3 -march=native -Wall -Wextra -std=c++20
LDFLAGS =

# Directorios
BUILD_DIR = build
SRC_DIR = src
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Obtener archivos fuente y generar nombres de objetos
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Nombre del ejecutable
TARGET = $(BIN_DIR)/cgl

# Regla principal
all: directories $(TARGET)

# Crear directorios necesarios
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(dir $(OBJS))

# Compilar el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilar archivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Limpiar archivos generados
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean directories

# Incluir dependencias
-include $(DEPS)