# Makefile para o projeto CMST
# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -march=native -flto

# Diretórios
SRC_DIR = src
INC_DIR = includes
BUILD_DIR = build

# Arquivos fonte e objetos
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/grafo.cpp $(SRC_DIR)/solucao.cpp $(SRC_DIR)/algoritmos.cpp $(SRC_DIR)/utils.cpp
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/grafo.o $(BUILD_DIR)/solucao.o $(BUILD_DIR)/algoritmos.o $(BUILD_DIR)/utils.o

# Executável
TARGET = cmst

# Regra padrão
all: $(BUILD_DIR) $(TARGET)

# Criar diretório build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Linkar executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilar arquivos objeto
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/grafo.h $(INC_DIR)/solucao.h $(INC_DIR)/algoritmos.h $(INC_DIR)/utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/grafo.o: $(SRC_DIR)/grafo.cpp $(INC_DIR)/grafo.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/solucao.o: $(SRC_DIR)/solucao.cpp $(INC_DIR)/solucao.h $(INC_DIR)/grafo.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/algoritmos.o: $(SRC_DIR)/algoritmos.cpp $(INC_DIR)/algoritmos.h $(INC_DIR)/grafo.h $(INC_DIR)/solucao.h $(INC_DIR)/utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(INC_DIR)/utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpar arquivos compilados
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild completo
rebuild: clean all

.PHONY: all clean rebuild
