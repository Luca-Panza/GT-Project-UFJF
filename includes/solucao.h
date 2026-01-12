#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <vector>
#include <string>
#include <iostream>
#include "grafo.h"

/**
 * Classe que representa uma solução para o problema CMST
 * Uma solução é uma árvore geradora enraizada onde cada subárvore
 * conectada diretamente à raiz respeita a capacidade máxima
 */
class Solucao {
private:
    const Grafo* grafo;                    // Referência ao grafo
    std::vector<int> pai;                  // pai[i] = pai do vértice i na árvore (-1 para raiz)
    std::vector<int> subarvore;            // subarvore[i] = ID da subárvore do vértice i
    double custoTotal;                     // Custo total da solução
    bool valida;                           // Indica se a solução é válida

    /**
     * Identifica as subárvores (DFS a partir dos filhos da raiz)
     */
    void identificarSubarvores();

public:
    /**
     * Construtor
     * @param g Ponteiro para o grafo
     */
    Solucao(const Grafo* g);

    /**
     * Construtor de cópia
     */
    Solucao(const Solucao& outra);

    /**
     * Operador de atribuição
     */
    Solucao& operator=(const Solucao& outra);

    /**
     * Destrutor
     */
    ~Solucao();

    /**
     * Define o pai de um vértice
     * @param vertice Vértice
     * @param p Pai do vértice
     */
    void setPai(int vertice, int p);

    /**
     * Obtém o pai de um vértice
     * @param vertice Vértice
     * @return Pai do vértice (-1 se for raiz)
     */
    int getPai(int vertice) const;

    /**
     * Obtém o vetor de pais completo
     */
    const std::vector<int>& getVetorPais() const;

    /**
     * Calcula e retorna o custo total da solução
     */
    double calcularCusto();

    /**
     * Retorna o custo total (deve chamar calcularCusto antes)
     */
    double getCustoTotal() const;

    /**
     * Verifica se a solução é viável (respeita capacidade)
     * @return true se todas as subárvores respeitam a capacidade
     */
    bool verificarViabilidade();

    /**
     * Retorna se a solução é válida
     */
    bool isValida() const;

    /**
     * Calcula a demanda total de uma subárvore
     * @param idSubarvore ID da subárvore
     * @return Soma das demandas dos vértices na subárvore
     */
    int getDemandaSubarvore(int idSubarvore) const;

    /**
     * Retorna o número de subárvores
     */
    int getNumSubarvores() const;

    /**
     * Retorna a subárvore de um vértice
     */
    int getSubarvore(int vertice) const;

    /**
     * Obtém os vértices de uma subárvore específica
     */
    std::vector<int> getVerticesSubarvore(int idSubarvore) const;

    /**
     * Imprime a solução no formato CS Academy (para visualização)
     * Formato: origem destino (uma aresta por linha)
     */
    void imprimirCSAcademy() const;

    /**
     * Imprime informações detalhadas da solução
     */
    void imprimir() const;

    /**
     * Retorna string no formato CS Academy
     */
    std::string toStringCSAcademy() const;

    /**
     * Limpa a solução
     */
    void limpar();

    /**
     * Verifica se todos os vértices estão conectados
     */
    bool estaCompleta() const;
};

#endif // SOLUCAO_H
