#!/bin/bash

# Script para executar experimentos com o programa cmst
# Executa cada algoritmo 10 vezes para cada instância com seeds diferentes

# Diretório das instâncias
INSTANCIAS_DIR="capmstnew"

# Verificar se o executável existe
if [ ! -f "./cmst" ]; then
    echo "Erro: executável 'cmst' não encontrado!"
    echo "Por favor, compile o projeto primeiro usando 'make'"
    exit 1
fi

# Verificar se o diretório de instâncias existe
if [ ! -d "$INSTANCIAS_DIR" ]; then
    echo "Erro: diretório '$INSTANCIAS_DIR' não encontrado!"
    exit 1
fi

# Função para executar algoritmo guloso
executar_guloso() {
    local instancia=$1
    local execucao=$2
    
    echo "  [Guloso] Execução $execucao/10 - Instância: $instancia"
    ./cmst "$instancia" guloso
}

# Função para executar algoritmo randomizado
executar_randomizado() {
    local instancia=$1
    local execucao=$2
    local seed=$3
    local alpha=$4
    local iter=100
    
    echo "  [Randomizado] Execução $execucao/10 - Instância: $instancia - Seed: $seed"
    ./cmst "$instancia" randomizado --alpha $alpha --iter $iter --seed $seed
}

# Função para executar algoritmo reativo
executar_reativo() {
    local instancia=$1
    local execucao=$2
    local seed=$3
    local alphas="0.03,0.04,0.05"
    local iter=2000
    local bloco=50
    
    echo "  [Reativo] Execução $execucao/10 - Instância: $instancia - Seed: $seed"
    ./cmst "$instancia" reativo --alphas $alphas --iter $iter --bloco $bloco --seed $seed
}

# Contador de instâncias processadas
total_instancias=0

# Processar cada arquivo .dat no diretório de instâncias
for instancia in "$INSTANCIAS_DIR"/*.dat "$INSTANCIAS_DIR"/*.DAT; do
    # Verificar se o arquivo existe (evita problemas com glob que não encontra arquivos)
    if [ ! -f "$instancia" ]; then
        continue
    fi
    
    total_instancias=$((total_instancias + 1))
    nome_instancia=$(basename "$instancia")
    
    echo ""
    echo "=========================================="
    echo "Processando instância: $nome_instancia"
    echo "=========================================="
    
    # Executar algoritmo guloso 10 vezes
    echo ""
    echo ">>> Algoritmo Guloso (10 execuções)"
    for execucao in {1..10}; do
        executar_guloso "$instancia" $execucao
    done
    
    # Executar algoritmo randomizado 10 vezes com seeds diferentes
    echo ""
    echo ">>> Algoritmo Randomizado (10 execuções, alpha = 0.03 --iter 50)"
    for execucao in {1..10}; do
        seed=$((1000 + execucao))  # Seeds de 1001 a 1010
        executar_randomizado "$instancia" $execucao $seed 0.03
    done

    # Executar algoritmo randomizado 10 vezes com seeds diferentes
    echo ""
    echo ">>> Algoritmo Randomizado (10 execuções, alpha = 0.04 --iter 50)"
    for execucao in {1..10}; do
        seed=$((1000 + execucao))  # Seeds de 1001 a 1010
        executar_randomizado "$instancia" $execucao $seed 0.04
    done

    # Executar algoritmo randomizado 10 vezes com seeds diferentes
    echo ""
    echo ">>> Algoritmo Randomizado (10 execuções, alpha = 0.05 --iter 50)"
    for execucao in {1..10}; do
        seed=$((1000 + execucao))  # Seeds de 1001 a 1010
        executar_randomizado "$instancia" $execucao $seed 0.05
    done
    
    # Executar algoritmo reativo 10 vezes com seeds diferentes
    echo ""
    echo ">>> Algoritmo Reativo (10 execuções, --iter 500 --bloco 50)"
    for execucao in {1..10}; do
        seed=$((2000 + execucao))  # Seeds de 2001 a 2010
        executar_reativo "$instancia" $execucao $seed
    done
    
    echo ""
    echo "Instância $nome_instancia concluída!"
done

echo ""
echo "=========================================="
echo "Experimentos concluídos!"
echo "Total de instâncias processadas: $total_instancias"
echo "Resultados salvos em: resultados.csv"
echo "=========================================="

