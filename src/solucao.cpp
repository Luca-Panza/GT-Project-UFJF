#include "../includes/solucao.h"
#include <algorithm>
#include <set>
#include <queue>
#include <sstream>

// Construtor
Solucao::Solucao(const Grafo* g) : grafo(g), custoTotal(0), valida(false) {
    if (grafo != nullptr) {
        int n = grafo->getNumVertices();
        pai.resize(n, -1);
        subarvore.resize(n, -1);
        // Raiz é pai de si mesma (ou -1)
        pai[grafo->getRaiz()] = -1;
        subarvore[grafo->getRaiz()] = 0;
    }
}

// Construtor de cópia
Solucao::Solucao(const Solucao& outra) 
    : grafo(outra.grafo), pai(outra.pai), subarvore(outra.subarvore),
      custoTotal(outra.custoTotal), valida(outra.valida) {}

// Operador de atribuição
Solucao& Solucao::operator=(const Solucao& outra) {
    if (this != &outra) {
        grafo = outra.grafo;
        pai = outra.pai;
        subarvore = outra.subarvore;
        custoTotal = outra.custoTotal;
        valida = outra.valida;
    }
    return *this;
}

// Destrutor
Solucao::~Solucao() {}

// Define o pai de um vértice
void Solucao::setPai(int vertice, int p) {
    if (vertice >= 0 && vertice < (int)pai.size()) {
        pai[vertice] = p;
        valida = false;  // Invalidar solução ao modificar
    }
}

// Obtém o pai de um vértice
int Solucao::getPai(int vertice) const {
    if (vertice >= 0 && vertice < (int)pai.size()) {
        return pai[vertice];
    }
    return -1;
}

// Obtém o vetor de pais
const std::vector<int>& Solucao::getVetorPais() const {
    return pai;
}

// Identifica as subárvores usando BFS a partir dos filhos da raiz
void Solucao::identificarSubarvores() {
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    // Resetar subárvores
    std::fill(subarvore.begin(), subarvore.end(), -1);
    subarvore[raiz] = 0;  // Raiz pertence à "subárvore 0"
    
    // Construir lista de adjacência da árvore
    std::vector<std::vector<int>> filhos(n);
    for (int i = 0; i < n; i++) {
        if (i != raiz && pai[i] != -1) {
            filhos[pai[i]].push_back(i);
        }
    }
    
    // Cada filho direto da raiz inicia uma subárvore
    int idSubarvore = 1;
    for (int filho : filhos[raiz]) {
        // BFS para marcar todos os vértices da subárvore
        std::queue<int> fila;
        fila.push(filho);
        subarvore[filho] = idSubarvore;
        
        while (!fila.empty()) {
            int atual = fila.front();
            fila.pop();
            
            for (int f : filhos[atual]) {
                subarvore[f] = idSubarvore;
                fila.push(f);
            }
        }
        idSubarvore++;
    }
}

// Calcula o custo total da solução
double Solucao::calcularCusto() {
    custoTotal = 0;
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    for (int i = 0; i < n; i++) {
        if (i != raiz && pai[i] != -1) {
            custoTotal += grafo->getCusto(i, pai[i]);
        }
    }
    
    return custoTotal;
}

// Retorna o custo total
double Solucao::getCustoTotal() const {
    return custoTotal;
}

// Verifica se a solução é viável
bool Solucao::verificarViabilidade() {
    identificarSubarvores();
    
    int capacidade = grafo->getCapacidade();
    int numSub = getNumSubarvores();
    
    // Verificar demanda de cada subárvore
    for (int s = 1; s <= numSub; s++) {
        int demanda = getDemandaSubarvore(s);
        if (demanda > capacidade) {
            valida = false;
            return false;
        }
    }
    
    valida = true;
    return true;
}

// Retorna se a solução é válida
bool Solucao::isValida() const {
    return valida;
}

// Calcula a demanda de uma subárvore
int Solucao::getDemandaSubarvore(int idSubarvore) const {
    int demanda = 0;
    int n = grafo->getNumVertices();
    
    for (int i = 0; i < n; i++) {
        if (subarvore[i] == idSubarvore) {
            demanda += grafo->getDemanda(i);
        }
    }
    
    return demanda;
}

// Retorna o número de subárvores
int Solucao::getNumSubarvores() const {
    std::set<int> subs;
    for (int s : subarvore) {
        if (s > 0) {  // Ignorar raiz (0) e não conectados (-1)
            subs.insert(s);
        }
    }
    return subs.size();
}

// Retorna a subárvore de um vértice
int Solucao::getSubarvore(int vertice) const {
    if (vertice >= 0 && vertice < (int)subarvore.size()) {
        return subarvore[vertice];
    }
    return -1;
}

// Obtém os vértices de uma subárvore
std::vector<int> Solucao::getVerticesSubarvore(int idSubarvore) const {
    std::vector<int> vertices;
    int n = grafo->getNumVertices();
    
    for (int i = 0; i < n; i++) {
        if (subarvore[i] == idSubarvore) {
            vertices.push_back(i);
        }
    }
    
    return vertices;
}

// Imprime no formato CS Academy
void Solucao::imprimirCSAcademy() const {
    std::cout << toStringCSAcademy();
}

// Retorna string no formato CS Academy
std::string Solucao::toStringCSAcademy() const {
    std::stringstream ss;
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    for (int i = 0; i < n; i++) {
        if (i != raiz && pai[i] != -1) {
            ss << pai[i] << " " << i << "\n";
        }
    }
    
    return ss.str();
}

// Imprime informações detalhadas
void Solucao::imprimir() const {
    std::cout << "=== Solução CMST ===" << std::endl;
    std::cout << "Custo total: " << custoTotal << std::endl;
    std::cout << "Válida: " << (valida ? "Sim" : "Não") << std::endl;
    
    int numSub = getNumSubarvores();
    std::cout << "Número de subárvores: " << numSub << std::endl;
    
    // Mostrar cada subárvore
    for (int s = 1; s <= numSub; s++) {
        std::vector<int> vertices = getVerticesSubarvore(s);
        if (!vertices.empty()) {
            std::cout << "  Subárvore " << s << ": {";
            for (size_t i = 0; i < vertices.size(); i++) {
                std::cout << vertices[i];
                if (i < vertices.size() - 1) std::cout << ", ";
            }
            std::cout << "} - Demanda: " << getDemandaSubarvore(s) << std::endl;
        }
    }
    
    std::cout << "\nFormato CS Academy:" << std::endl;
    imprimirCSAcademy();
}

// Limpa a solução
void Solucao::limpar() {
    int raiz = grafo->getRaiz();
    std::fill(pai.begin(), pai.end(), -1);
    std::fill(subarvore.begin(), subarvore.end(), -1);
    pai[raiz] = -1;
    subarvore[raiz] = 0;
    custoTotal = 0;
    valida = false;
}

// Verifica se todos os vértices estão conectados
bool Solucao::estaCompleta() const {
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    for (int i = 0; i < n; i++) {
        if (i != raiz && pai[i] == -1) {
            return false;
        }
    }
    return true;
}
