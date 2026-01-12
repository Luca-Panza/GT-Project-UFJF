#include "../includes/utils.h"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>
#include <climits>

// ==================== Randomizador ====================

// Variáveis estáticas
std::mt19937 Randomizador::gerador;
unsigned int Randomizador::sementeAtual = 0;
bool Randomizador::inicializado = false;

unsigned int Randomizador::inicializar() {
    auto agora = std::chrono::high_resolution_clock::now();
    auto duracao = agora.time_since_epoch();
    sementeAtual = static_cast<unsigned int>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(duracao).count() % UINT_MAX
    );
    gerador.seed(sementeAtual);
    inicializado = true;
    return sementeAtual;
}

void Randomizador::inicializar(unsigned int semente) {
    sementeAtual = semente;
    gerador.seed(sementeAtual);
    inicializado = true;
}

unsigned int Randomizador::getSemente() {
    if (!inicializado) {
        inicializar();
    }
    return sementeAtual;
}

int Randomizador::randInt(int min, int max) {
    if (!inicializado) {
        inicializar();
    }
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gerador);
}

double Randomizador::randDouble() {
    if (!inicializado) {
        inicializar();
    }
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gerador);
}

double Randomizador::randDouble(double min, double max) {
    if (!inicializado) {
        inicializar();
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gerador);
}

std::mt19937& Randomizador::getGerador() {
    if (!inicializado) {
        inicializar();
    }
    return gerador;
}

// ==================== Cronômetro ====================

Cronometro::Cronometro() : rodando(false) {}

void Cronometro::iniciar() {
    inicio = std::chrono::high_resolution_clock::now();
    rodando = true;
}

void Cronometro::parar() {
    fim = std::chrono::high_resolution_clock::now();
    rodando = false;
}

double Cronometro::getTempoSegundos() const {
    auto fimTempo = rodando ? std::chrono::high_resolution_clock::now() : fim;
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fimTempo - inicio);
    return duracao.count() / 1000000.0;
}

double Cronometro::getTempoMilissegundos() const {
    auto fimTempo = rodando ? std::chrono::high_resolution_clock::now() : fim;
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fimTempo - inicio);
    return duracao.count() / 1000.0;
}

// ==================== ResultadoExecucao ====================

ResultadoExecucao::ResultadoExecucao()
    : numVertices(0), capacidade(0), alpha(-1), iteracoes(-1),
      tamanhoBloco(-1), semente(0), tempoSegundos(0),
      melhorSolucao(0), mediaSolucoes(-1), melhorAlpha(-1), solucaoOtima(-1) {}

// ==================== EscritorCSV ====================

EscritorCSV::EscritorCSV(const std::string& arquivo) : nomeArquivo(arquivo) {}

bool EscritorCSV::arquivoExiste() const {
    std::ifstream f(nomeArquivo);
    return f.good();
}

void EscritorCSV::escreverCabecalho() {
    if (arquivoExiste()) {
        return;  // Arquivo já existe, não sobrescrever cabeçalho
    }
    
    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao criar arquivo CSV: " << nomeArquivo << std::endl;
        return;
    }
    
    arquivo << "data_hora,instancia,n_vertices,capacidade,algoritmo,alpha,iteracoes,"
            << "tamanho_bloco,semente,tempo_segundos,melhor_solucao,media_solucoes,"
            << "melhor_alpha,solucao_otima" << std::endl;
    
    arquivo.close();
}

void EscritorCSV::escreverResultado(const ResultadoExecucao& r) {
    // Garantir que o cabeçalho existe
    escreverCabecalho();
    
    std::ofstream arquivo(nomeArquivo, std::ios::app);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir arquivo CSV: " << nomeArquivo << std::endl;
        return;
    }
    
    arquivo << r.dataHora << ","
            << r.instancia << ","
            << r.numVertices << ","
            << r.capacidade << ","
            << r.algoritmo << ",";
    
    // Alpha (pode ser vazio)
    if (r.alpha >= 0) {
        arquivo << Utils::formatarDouble(r.alpha, 2);
    }
    arquivo << ",";
    
    // Iterações (pode ser vazio)
    if (r.iteracoes >= 0) {
        arquivo << r.iteracoes;
    }
    arquivo << ",";
    
    // Tamanho do bloco (pode ser vazio)
    if (r.tamanhoBloco >= 0) {
        arquivo << r.tamanhoBloco;
    }
    arquivo << ",";
    
    arquivo << r.semente << ","
            << Utils::formatarDouble(r.tempoSegundos, 6) << ","
            << Utils::formatarDouble(r.melhorSolucao, 2) << ",";
    
    // Média das soluções (pode ser vazio)
    if (r.mediaSolucoes >= 0) {
        arquivo << Utils::formatarDouble(r.mediaSolucoes, 2);
    }
    arquivo << ",";
    
    // Melhor alpha (pode ser vazio)
    if (r.melhorAlpha >= 0) {
        arquivo << Utils::formatarDouble(r.melhorAlpha, 2);
    }
    arquivo << ",";
    
    // Solução ótima (pode ser vazio)
    if (r.solucaoOtima >= 0) {
        arquivo << Utils::formatarDouble(r.solucaoOtima, 2);
    }
    
    arquivo << std::endl;
    arquivo.close();
}

// ==================== Utils ====================

std::string Utils::getDataHoraAtual() {
    auto agora = std::chrono::system_clock::now();
    auto tempo = std::chrono::system_clock::to_time_t(agora);
    std::tm* tm = std::localtime(&tempo);
    
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Utils::formatarDouble(double valor, int precisao) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precisao) << valor;
    return ss.str();
}
