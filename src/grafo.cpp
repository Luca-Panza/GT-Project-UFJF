#include "../includes/grafo.h"

// Construtor padrão
Grafo::Grafo() : numVertices(0), capacidade(0), raiz(0), solucaoOtima(-1) {}

// Construtor com parâmetros
Grafo::Grafo(int n, int cap) : numVertices(n), capacidade(cap), raiz(0), solucaoOtima(-1) {
    matrizAdj.resize(n, std::vector<double>(n, INFINITO));
    demandas.resize(n, 1);  // Demanda unitária por padrão
    
    // Diagonal principal = 0 (custo para si mesmo)
    for (int i = 0; i < n; i++) {
        matrizAdj[i][i] = 0;
    }
}

// Destrutor
Grafo::~Grafo() {}

// Carrega instância de arquivo no formato OR-Library
bool Grafo::carregarInstancia(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo '" << nomeArquivo << "'" << std::endl;
        return false;
    }

    // Extrair nome da instância do caminho
    size_t pos = nomeArquivo.find_last_of("/\\");
    nomeInstancia = (pos != std::string::npos) ? nomeArquivo.substr(pos + 1) : nomeArquivo;

    // Ler primeira linha: n Q
    arquivo >> numVertices >> capacidade;

    if (numVertices <= 0 || capacidade <= 0) {
        std::cerr << "Erro: Valores inválidos para n ou Q" << std::endl;
        return false;
    }

    // Inicializar estruturas
    matrizAdj.resize(numVertices, std::vector<double>(numVertices, INFINITO));
    demandas.resize(numVertices, 1);  // Demanda unitária
    raiz = 0;  // Primeiro vértice é a raiz

    // Ler matriz de distâncias
    // O formato usa largura fixa de 4 caracteres por valor
    // Quando valores pequenos precedem 1000 na diagonal, ficam concatenados (ex: "311000" = 31 e 1000)
    
    std::string conteudo;
    std::string linha;
    std::getline(arquivo, linha);  // Consumir resto da primeira linha

    // Ler todo o conteúdo restante, guardando a última linha separadamente para verificar se há solução ótima
    std::string ultimaLinha;
    while (std::getline(arquivo, linha)) {
        // Remover \r se houver (Windows line endings)
        if (!linha.empty() && linha.back() == '\r') {
            linha.pop_back();
        }
        // conteudo += linha + " ";

        if (!linha.empty()) {
            ultimaLinha = linha;  // Guardar última linha não vazia
            conteudo += linha + " ";
        }
    }
    arquivo.close();
    
    // Extrair valores considerando largura fixa e concatenações
    std::vector<double> valores;
    std::istringstream iss(conteudo);
    std::string token;
    
    while (iss >> token) {
        // Verificar se é valor concatenado com 1000
        // Pattern: número pequeno seguido de 1000 (ex: "311000", "601000")
        if (token.length() > 4 && token.length() <= 7) {
            // Verificar se termina com 1000
            if (token.substr(token.length() - 4) == "1000") {
                std::string antes = token.substr(0, token.length() - 4);
                if (!antes.empty()) {
                    try {
                        valores.push_back(std::stod(antes));
                    } catch (...) {}
                }
                valores.push_back(1000);
                continue;
            }
        }
        
        try {
            valores.push_back(std::stod(token));
        } catch (...) {
            // Ignorar tokens inválidos
        }
    }

    // Calcular quantos valores esperamos para a matriz n×n
    int esperado = numVertices * numVertices;
    
    if ((int)valores.size() < esperado) {
        std::cerr << "Erro: Dados insuficientes na matriz de distâncias" << std::endl;
        std::cerr << "Esperado: " << esperado << ", Encontrado: " << valores.size() << std::endl;
        return false;
    }

    // Preencher matriz de adjacência
    int idx = 0;
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            double custo = valores[idx++];
            // Valor 1000 na diagonal representa o próprio vértice (sem aresta)
            if (i == j) {
                matrizAdj[i][j] = 0;
            } else {
                matrizAdj[i][j] = custo;
            }
        }
    }

    // Verificar se há solução ótima na última linha do arquivo
    // A solução ótima aparece como uma linha separada com apenas um número
    solucaoOtima = -1;  // Padrão: não disponível
    
    if (!ultimaLinha.empty()) {
        // Verificar se a última linha contém apenas um número (possivelmente com espaços)
        std::istringstream ultimaIss(ultimaLinha);
        std::string primeiroToken;
        ultimaIss >> primeiroToken;


        std::cout << "Primeiro token: " << primeiroToken << std::endl;
        std::cout << "Valores: " << valores.size() << std::endl;
        std::cout << "Esperado: " << esperado << std::endl;
    
        // Se há apenas um token e é um número válido, é a solução ótima
        if (!primeiroToken.empty()) {
            try {
                solucaoOtima = std::stod(primeiroToken);
            } catch (...) {
                // Não é um número válido, não há solução ótima
            }
        }
    }

    return true;
}

// Getters
int Grafo::getNumVertices() const {
    return numVertices;
}

int Grafo::getCapacidade() const {
    return capacidade;
}

int Grafo::getRaiz() const {
    return raiz;
}

int Grafo::getDemanda(int vertice) const {
    if (vertice < 0 || vertice >= numVertices) {
        return 0;
    }
    return demandas[vertice];
}

double Grafo::getCusto(int origem, int destino) const {
    if (origem < 0 || origem >= numVertices || destino < 0 || destino >= numVertices) {
        return INFINITO;
    }
    return matrizAdj[origem][destino];
}

double Grafo::getSolucaoOtima() const {
    return solucaoOtima;
}

std::string Grafo::getNomeInstancia() const {
    return nomeInstancia;
}

const std::vector<std::vector<double>>& Grafo::getMatrizAdj() const {
    return matrizAdj;
}

// Setters
void Grafo::setRaiz(int r) {
    if (r >= 0 && r < numVertices) {
        raiz = r;
    }
}

void Grafo::setDemanda(int vertice, int demanda) {
    if (vertice >= 0 && vertice < numVertices && demanda >= 0) {
        demandas[vertice] = demanda;
    }
}

void Grafo::setCusto(int origem, int destino, double custo) {
    if (origem >= 0 && origem < numVertices && destino >= 0 && destino < numVertices) {
        matrizAdj[origem][destino] = custo;
    }
}

// Verifica se existe aresta
bool Grafo::existeAresta(int origem, int destino) const {
    if (origem < 0 || origem >= numVertices || destino < 0 || destino >= numVertices) {
        return false;
    }
    return matrizAdj[origem][destino] < INFINITO && origem != destino;
}

// Imprime informações do grafo
void Grafo::imprimir() const {
    std::cout << "=== Grafo CMST ===" << std::endl;
    std::cout << "Instância: " << nomeInstancia << std::endl;
    std::cout << "Vértices: " << numVertices << std::endl;
    std::cout << "Capacidade: " << capacidade << std::endl;
    std::cout << "Raiz: " << raiz << std::endl;
    if (solucaoOtima > 0) {
        std::cout << "Solução ótima conhecida: " << solucaoOtima << std::endl;
    }
    
    // Mostrar algumas arestas como exemplo
    std::cout << "\nPrimeiras arestas (exemplo):" << std::endl;
    int count = 0;
    for (int i = 0; i < numVertices && count < 10; i++) {
        for (int j = i + 1; j < numVertices && count < 10; j++) {
            if (existeAresta(i, j)) {
                std::cout << "  " << i << " -- " << j << " [custo=" << matrizAdj[i][j] << "]" << std::endl;
                count++;
            }
        }
    }
    std::cout << "..." << std::endl;
}
