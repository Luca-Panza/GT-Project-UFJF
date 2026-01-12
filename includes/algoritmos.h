#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include <vector>
#include <string>
#include "grafo.h"
#include "solucao.h"
#include "utils.h"

/**
 * Estrutura para representar um candidato (aresta) na construção gulosa
 */
struct Candidato {
    int vertice;        // Vértice a ser conectado
    int pai;            // Vértice pai (onde será conectado)
    double custo;       // Custo da aresta
    int subarvore;      // Subárvore do pai (se aplicável)

    Candidato(int v = -1, int p = -1, double c = 0, int s = -1)
        : vertice(v), pai(p), custo(c), subarvore(s) {}

    // Ordenação por custo (menor primeiro)
    bool operator<(const Candidato& outro) const {
        return custo < outro.custo;
    }
};

/**
 * Classe que implementa os algoritmos heurísticos para CMST
 */
class Algoritmos {
private:
    const Grafo* grafo;

    /**
     * Calcula a demanda atual de uma subárvore na solução parcial
     * @param solucao Solução parcial
     * @param subarvoreId ID da subárvore
     * @return Demanda total da subárvore
     */
    int calcularDemandaSubarvore(const Solucao& solucao, int subarvoreId) const;

    /**
     * Encontra a subárvore de um vértice (subindo até a raiz)
     * @param solucao Solução atual
     * @param vertice Vértice para encontrar a subárvore
     * @return ID da subárvore (1, 2, ...) ou 0 se for raiz
     */
    int encontrarSubarvore(const Solucao& solucao, int vertice) const;

    /**
     * Verifica se adicionar um vértice a uma subárvore respeita a capacidade
     * @param solucao Solução atual
     * @param vertice Vértice a ser adicionado
     * @param paiVertice Pai do vértice (onde será conectado)
     * @return true se a adição respeita a capacidade
     */
    bool podeAdicionar(const Solucao& solucao, int vertice, int paiVertice) const;

    /**
     * Gera lista de candidatos viáveis para o próximo passo da construção
     * @param solucao Solução parcial atual
     * @param conectados Vetor indicando vértices já conectados
     * @return Lista de candidatos ordenada por custo
     */
    std::vector<Candidato> gerarCandidatos(const Solucao& solucao, 
                                            const std::vector<bool>& conectados) const;

    /**
     * Constrói solução usando abordagem gulosa pura
     * @return Solução construída
     */
    Solucao construirGuloso() const;

    /**
     * Constrói solução usando abordagem gulosa randomizada
     * @param alpha Parâmetro de aleatoriedade [0, 1]
     * @return Solução construída
     */
    Solucao construirGulosoRandomizado(double alpha) const;

public:
    /**
     * Construtor
     * @param g Ponteiro para o grafo
     */
    Algoritmos(const Grafo* g);

    /**
     * Destrutor
     */
    ~Algoritmos();

    /**
     * Executa o algoritmo guloso
     * @param resultado Estrutura para armazenar resultados
     * @return Melhor solução encontrada
     */
    Solucao executarGuloso(ResultadoExecucao& resultado);

    /**
     * Executa o algoritmo guloso randomizado
     * @param alpha Parâmetro de aleatoriedade [0, 1]
     * @param numIteracoes Número de iterações
     * @param resultado Estrutura para armazenar resultados
     * @return Melhor solução encontrada
     */
    Solucao executarGulosoRandomizado(double alpha, int numIteracoes, 
                                       ResultadoExecucao& resultado);

    /**
     * Executa o algoritmo guloso randomizado reativo
     * @param alphas Vetor de valores de alpha
     * @param numIteracoes Número total de iterações
     * @param tamanhoBloco Tamanho do bloco para atualização de probabilidades
     * @param resultado Estrutura para armazenar resultados
     * @return Melhor solução encontrada
     */
    Solucao executarGulosoReativo(const std::vector<double>& alphas,
                                   int numIteracoes, int tamanhoBloco,
                                   ResultadoExecucao& resultado);
};

#endif // ALGORITMOS_H
