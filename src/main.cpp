#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include "../includes/grafo.h"
#include "../includes/solucao.h"
#include "../includes/algoritmos.h"
#include "../includes/utils.h"

// Nome do arquivo CSV de resultados
const std::string ARQUIVO_CSV = "resultados.csv";

/**
 * Imprime ajuda de uso do programa
 */
void imprimirAjuda(const char* nomePrograma) {
    std::cout << "Uso: " << nomePrograma << " <instancia> <algoritmo> [opcoes]\n\n";
    std::cout << "Algoritmos disponíveis:\n";
    std::cout << "  guloso       - Algoritmo guloso determinístico\n";
    std::cout << "  randomizado  - Algoritmo guloso randomizado\n";
    std::cout << "  reativo      - Algoritmo guloso randomizado reativo\n\n";
    
    std::cout << "Opções para 'randomizado':\n";
    std::cout << "  --alpha <valor>   - Valor de alpha (0.0 a 1.0), default: 0.3\n";
    std::cout << "  --iter <num>      - Número de iterações, default: 30\n";
    std::cout << "  --seed <num>      - Semente de randomização (opcional)\n\n";
    
    std::cout << "Opções para 'reativo':\n";
    std::cout << "  --alphas <lista>  - Lista de alphas separados por vírgula, default: 0.1,0.3,0.5\n";
    std::cout << "  --iter <num>      - Número de iterações, default: 300\n";
    std::cout << "  --bloco <num>     - Tamanho do bloco, default: 30\n";
    std::cout << "  --seed <num>      - Semente de randomização (opcional)\n\n";
    
    std::cout << "Opções gerais:\n";
    std::cout << "  --csv <arquivo>   - Arquivo CSV de saída, default: resultados.csv\n";
    std::cout << "  --verbose         - Modo verboso (imprime detalhes)\n";
    std::cout << "  --help            - Mostra esta ajuda\n\n";
    
    std::cout << "Exemplos:\n";
    std::cout << "  " << nomePrograma << " capmstnew/TC4001.DAT guloso\n";
    std::cout << "  " << nomePrograma << " capmstnew/TC4001.DAT randomizado --alpha 0.3 --iter 30\n";
    std::cout << "  " << nomePrograma << " capmstnew/TC4001.DAT reativo --alphas 0.1,0.2,0.3 --iter 300 --bloco 30\n";
}

/**
 * Parseia uma string de alphas separados por vírgula
 */
std::vector<double> parsearAlphas(const std::string& str) {
    std::vector<double> alphas;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        try {
            double alpha = std::stod(item);
            if (alpha >= 0.0 && alpha <= 1.0) {
                alphas.push_back(alpha);
            }
        } catch (...) {
            // Ignorar valores inválidos
        }
    }
    
    return alphas;
}

int main(int argc, char* argv[]) {
    // Verificar argumentos mínimos
    if (argc < 3) {
        imprimirAjuda(argv[0]);
        return 1;
    }
    
    // Verificar se é pedido de ajuda
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            imprimirAjuda(argv[0]);
            return 0;
        }
    }
    
    // Parâmetros
    std::string arquivoInstancia = argv[1];
    std::string algoritmo = argv[2];
    std::string arquivoCSV = ARQUIVO_CSV;
    
    // Parâmetros do randomizado
    double alpha = 0.3;
    int numIteracoes = 30;
    
    // Parâmetros do reativo
    std::vector<double> alphas = {0.1, 0.3, 0.5};
    int tamanhoBloco = 30;
    
    // Opções
    bool usarSemente = false;
    unsigned int semente = 0;
    bool verbose = false;
    
    // Parsear argumentos opcionais
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--alpha") == 0 && i + 1 < argc) {
            alpha = std::stod(argv[++i]);
        } else if (strcmp(argv[i], "--alphas") == 0 && i + 1 < argc) {
            alphas = parsearAlphas(argv[++i]);
        } else if (strcmp(argv[i], "--iter") == 0 && i + 1 < argc) {
            numIteracoes = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--bloco") == 0 && i + 1 < argc) {
            tamanhoBloco = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            semente = std::stoul(argv[++i]);
            usarSemente = true;
        } else if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) {
            arquivoCSV = argv[++i];
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        }
    }
    
    // Ajustar iterações padrão para reativo
    if (algoritmo == "reativo" && numIteracoes == 30) {
        numIteracoes = 300;  // Default para reativo é 300
    }
    
    // Inicializar randomização
    if (usarSemente) {
        Randomizador::inicializar(semente);
    } else {
        semente = Randomizador::inicializar();
    }
    
    // Carregar instância
    Grafo grafo;
    if (!grafo.carregarInstancia(arquivoInstancia)) {
        std::cerr << "Erro ao carregar instância: " << arquivoInstancia << std::endl;
        return 1;
    }
    
    if (verbose) {
        std::cout << "=== Configuração ===" << std::endl;
        std::cout << "Instância: " << arquivoInstancia << std::endl;
        std::cout << "Algoritmo: " << algoritmo << std::endl;
        std::cout << "Semente: " << semente << std::endl;
        grafo.imprimir();
        std::cout << std::endl;
    }
    
    // Criar estruturas
    Algoritmos alg(&grafo);
    ResultadoExecucao resultado;
    Solucao melhorSolucao(&grafo);
    
    // Executar algoritmo
    if (algoritmo == "guloso") {
        if (verbose) {
            std::cout << "Executando algoritmo guloso..." << std::endl;
        }
        melhorSolucao = alg.executarGuloso(resultado);
        
    } else if (algoritmo == "randomizado") {
        if (verbose) {
            std::cout << "Executando algoritmo randomizado..." << std::endl;
            std::cout << "  Alpha: " << alpha << std::endl;
            std::cout << "  Iterações: " << numIteracoes << std::endl;
        }
        melhorSolucao = alg.executarGulosoRandomizado(alpha, numIteracoes, resultado);
        
    } else if (algoritmo == "reativo") {
        if (verbose) {
            std::cout << "Executando algoritmo reativo..." << std::endl;
            std::cout << "  Alphas: ";
            for (size_t i = 0; i < alphas.size(); i++) {
                std::cout << alphas[i];
                if (i < alphas.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
            std::cout << "  Iterações: " << numIteracoes << std::endl;
            std::cout << "  Tamanho do bloco: " << tamanhoBloco << std::endl;
        }
        melhorSolucao = alg.executarGulosoReativo(alphas, numIteracoes, tamanhoBloco, resultado);
        
    } else {
        std::cerr << "Algoritmo desconhecido: " << algoritmo << std::endl;
        imprimirAjuda(argv[0]);
        return 1;
    }
    
    // Salvar resultado em CSV
    EscritorCSV escritor(arquivoCSV);
    escritor.escreverResultado(resultado);
    
    // Imprimir resultado
    std::cout << "\n=== Resultado ===" << std::endl;
    std::cout << "Algoritmo: " << resultado.algoritmo << std::endl;
    std::cout << "Semente: " << resultado.semente << std::endl;
    std::cout << "Tempo: " << Utils::formatarDouble(resultado.tempoSegundos, 6) << " segundos" << std::endl;
    std::cout << "Melhor solução: " << Utils::formatarDouble(resultado.melhorSolucao, 2) << std::endl;
    
    if (resultado.mediaSolucoes >= 0) {
        std::cout << "Média das soluções: " << Utils::formatarDouble(resultado.mediaSolucoes, 2) << std::endl;
    }
    
    if (resultado.melhorAlpha >= 0) {
        std::cout << "Melhor alpha: " << Utils::formatarDouble(resultado.melhorAlpha, 2) << std::endl;
    }
    
    if (resultado.solucaoOtima > 0) {
        double desvio = ((resultado.melhorSolucao - resultado.solucaoOtima) / resultado.solucaoOtima) * 100;
        std::cout << "Solução ótima conhecida: " << Utils::formatarDouble(resultado.solucaoOtima, 2) << std::endl;
        std::cout << "Desvio percentual: " << Utils::formatarDouble(desvio, 2) << "%" << std::endl;
    }
    
    std::cout << "Solução válida: " << (melhorSolucao.isValida() ? "Sim" : "Não") << std::endl;
    std::cout << "Resultado salvo em: " << arquivoCSV << std::endl;
    
    // Imprimir solução no formato CS Academy
    std::cout << "\n=== Solução (formato CS Academy) ===" << std::endl;
    melhorSolucao.imprimirCSAcademy();
    
    if (verbose) {
        std::cout << "\n=== Detalhes da Solução ===" << std::endl;
        melhorSolucao.imprimir();
    }
    
    return 0;
}
