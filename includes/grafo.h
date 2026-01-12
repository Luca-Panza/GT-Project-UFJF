#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>

#define INFINITO std::numeric_limits<double>::infinity()

/**
 * Classe Grafo para o problema CMST (Capacitated Minimum Spanning Tree)
 * Armazena grafo completo com matriz de adjacência
 */
class Grafo {
private:
    int numVertices;                              // Número de vértices
    int capacidade;                               // Capacidade máxima das subárvores (Q)
    int raiz;                                     // Vértice raiz (default = 0)
    std::vector<std::vector<double>> matrizAdj;  // Matriz de adjacência (custos)
    std::vector<int> demandas;                    // Demanda de cada vértice (default = 1)
    double solucaoOtima;                          // Solução ótima conhecida (se disponível)
    std::string nomeInstancia;                    // Nome do arquivo de instância

public:
    /**
     * Construtor padrão
     */
    Grafo();

    /**
     * Construtor com número de vértices e capacidade
     * @param n Número de vértices
     * @param cap Capacidade máxima das subárvores
     */
    Grafo(int n, int cap);

    /**
     * Destrutor
     */
    ~Grafo();

    /**
     * Carrega instância de arquivo no formato OR-Library
     * Formato:
     *   Linha 1: n Q (número de vértices e capacidade)
     *   Linhas seguintes: Matriz de distâncias n×n
     *   Última linha (opcional): Valor da solução ótima
     * @param nomeArquivo Caminho do arquivo
     * @return true se carregou com sucesso
     */
    bool carregarInstancia(const std::string& nomeArquivo);

    // Getters
    int getNumVertices() const;
    int getCapacidade() const;
    int getRaiz() const;
    int getDemanda(int vertice) const;
    double getCusto(int origem, int destino) const;
    double getSolucaoOtima() const;
    std::string getNomeInstancia() const;
    const std::vector<std::vector<double>>& getMatrizAdj() const;

    // Setters
    void setRaiz(int r);
    void setDemanda(int vertice, int demanda);
    void setCusto(int origem, int destino, double custo);

    /**
     * Verifica se existe aresta entre dois vértices
     */
    bool existeAresta(int origem, int destino) const;

    /**
     * Imprime informações do grafo
     */
    void imprimir() const;
};

#endif // GRAFO_H
