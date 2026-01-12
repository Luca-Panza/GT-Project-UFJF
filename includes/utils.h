#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random>
#include <chrono>
#include <fstream>

/**
 * Classe utilitária para gerenciamento de randomização
 */
class Randomizador {
private:
    static std::mt19937 gerador;
    static unsigned int sementeAtual;
    static bool inicializado;

public:
    /**
     * Inicializa o gerador com semente baseada em data/hora
     * @return A semente gerada
     */
    static unsigned int inicializar();

    /**
     * Inicializa o gerador com semente específica
     * @param semente Semente a ser usada
     */
    static void inicializar(unsigned int semente);

    /**
     * Retorna a semente atual
     */
    static unsigned int getSemente();

    /**
     * Gera um número inteiro aleatório em [min, max]
     */
    static int randInt(int min, int max);

    /**
     * Gera um número real aleatório em [0, 1)
     */
    static double randDouble();

    /**
     * Gera um número real aleatório em [min, max)
     */
    static double randDouble(double min, double max);

    /**
     * Retorna referência ao gerador (para uso com std::shuffle, etc.)
     */
    static std::mt19937& getGerador();
};

/**
 * Classe utilitária para medição de tempo
 */
class Cronometro {
private:
    std::chrono::high_resolution_clock::time_point inicio;
    std::chrono::high_resolution_clock::time_point fim;
    bool rodando;

public:
    Cronometro();

    /**
     * Inicia a contagem de tempo
     */
    void iniciar();

    /**
     * Para a contagem de tempo
     */
    void parar();

    /**
     * Retorna o tempo decorrido em segundos
     */
    double getTempoSegundos() const;

    /**
     * Retorna o tempo decorrido em milissegundos
     */
    double getTempoMilissegundos() const;
};

/**
 * Estrutura para armazenar resultado de uma execução
 */
struct ResultadoExecucao {
    std::string dataHora;
    std::string instancia;
    int numVertices;
    int capacidade;
    std::string algoritmo;
    double alpha;           // -1 se não aplicável
    int iteracoes;          // -1 se não aplicável
    int tamanhoBloco;       // -1 se não aplicável
    unsigned int semente;
    double tempoSegundos;
    double melhorSolucao;
    double mediaSolucoes;   // -1 se não aplicável
    double melhorAlpha;     // -1 se não aplicável
    double solucaoOtima;    // -1 se não disponível

    ResultadoExecucao();
};

/**
 * Classe para escrita de resultados em CSV
 */
class EscritorCSV {
private:
    std::string nomeArquivo;

    /**
     * Verifica se o arquivo existe e tem cabeçalho
     */
    bool arquivoExiste() const;

public:
    /**
     * Construtor
     * @param arquivo Nome do arquivo CSV
     */
    EscritorCSV(const std::string& arquivo);

    /**
     * Escreve o cabeçalho do CSV (se arquivo não existir)
     */
    void escreverCabecalho();

    /**
     * Escreve uma linha de resultado
     * @param resultado Estrutura com os dados da execução
     */
    void escreverResultado(const ResultadoExecucao& resultado);
};

/**
 * Funções auxiliares
 */
namespace Utils {
    /**
     * Retorna data/hora atual formatada
     */
    std::string getDataHoraAtual();

    /**
     * Formata um número double para string com precisão específica
     */
    std::string formatarDouble(double valor, int precisao = 2);
}

#endif // UTILS_H
