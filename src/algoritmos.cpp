#include "../includes/algoritmos.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <map>

// Construtor
Algoritmos::Algoritmos(const Grafo* g) : grafo(g) {}

// Destrutor
Algoritmos::~Algoritmos() {}

// Calcula a demanda atual de uma subárvore
int Algoritmos::calcularDemandaSubarvore(const Solucao& solucao, int subarvoreId) const {
    if (subarvoreId <= 0) return 0;
    
    int demanda = 0;
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    // Primeiro, encontrar o filho direto da raiz que inicia esta subárvore
    // Depois, percorrer todos os descendentes
    std::vector<bool> visitado(n, false);
    std::vector<int> fila;
    
    // Encontrar todos os vértices da subárvore
    for (int i = 0; i < n; i++) {
        if (i != raiz && !visitado[i]) {
            // Verificar se este vértice pertence à subárvore
            int atual = i;
            std::vector<int> caminho;
            
            while (atual != -1 && atual != raiz && !visitado[atual]) {
                caminho.push_back(atual);
                atual = solucao.getPai(atual);
            }
            
            // Se chegou na raiz, verificar a qual subárvore pertence
            if (atual == raiz && !caminho.empty()) {
                int filhoRaiz = caminho.back();  // Filho direto da raiz
                
                // Cada filho direto da raiz inicia uma subárvore diferente
                // Verificar se é a subárvore que estamos procurando
                // Para isso, contar qual é o índice deste filho
                int contadorFilhos = 0;
                for (int j = 0; j < n; j++) {
                    if (solucao.getPai(j) == raiz) {
                        contadorFilhos++;
                        if (j == filhoRaiz && contadorFilhos == subarvoreId) {
                            // Este é o caminho certo, adicionar todos à demanda
                            for (int v : caminho) {
                                if (!visitado[v]) {
                                    demanda += grafo->getDemanda(v);
                                    visitado[v] = true;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return demanda;
}

// Encontra a subárvore de um vértice
int Algoritmos::encontrarSubarvore(const Solucao& solucao, int vertice) const {
    int raiz = grafo->getRaiz();
    
    if (vertice == raiz) return 0;
    
    // Subir até encontrar o filho direto da raiz
    int atual = vertice;
    while (atual != -1 && atual != raiz) {
        int pai = solucao.getPai(atual);
        if (pai == raiz) {
            // Encontramos o filho direto da raiz
            // Retornar o índice desta subárvore
            int n = grafo->getNumVertices();
            int idx = 1;
            for (int i = 0; i < n; i++) {
                if (solucao.getPai(i) == raiz) {
                    if (i == atual) return idx;
                    idx++;
                }
            }
        }
        atual = pai;
    }
    
    return -1;  // Não conectado
}

// Verifica se pode adicionar um vértice respeitando a capacidade
bool Algoritmos::podeAdicionar(const Solucao& solucao, int vertice, int paiVertice) const {
    int raiz = grafo->getRaiz();
    int capacidade = grafo->getCapacidade();
    int demandaVertice = grafo->getDemanda(vertice);
    
    // Se o pai é a raiz, sempre pode adicionar (nova subárvore)
    if (paiVertice == raiz) {
        return demandaVertice <= capacidade;
    }
    
    // Encontrar a qual subárvore o pai pertence e calcular demanda atual
    int n = grafo->getNumVertices();
    
    // Subir até a raiz para encontrar o filho direto da raiz
    int atual = paiVertice;
    while (atual != -1 && atual != raiz) {
        int pai = solucao.getPai(atual);
        if (pai == raiz) {
            break;  // atual é o filho direto da raiz
        }
        atual = pai;
    }
    
    if (atual == -1 || atual == raiz) {
        return true;  // Pai não conectado ou é a raiz
    }
    
    // Calcular demanda atual da subárvore usando BFS
    int demandaAtual = 0;
    std::vector<bool> visitado(n, false);
    std::vector<int> fila;
    fila.push_back(atual);
    visitado[atual] = true;
    
    // Construir árvore de adjacência
    std::vector<std::vector<int>> filhos(n);
    for (int i = 0; i < n; i++) {
        int pai = solucao.getPai(i);
        if (pai != -1 && pai != i) {
            filhos[pai].push_back(i);
        }
    }
    
    // BFS para somar demandas
    size_t idx = 0;
    while (idx < fila.size()) {
        int v = fila[idx++];
        demandaAtual += grafo->getDemanda(v);
        
        for (int filho : filhos[v]) {
            if (!visitado[filho]) {
                visitado[filho] = true;
                fila.push_back(filho);
            }
        }
    }
    
    return (demandaAtual + demandaVertice) <= capacidade;
}

// Gera lista de candidatos viáveis
std::vector<Candidato> Algoritmos::gerarCandidatos(const Solucao& solucao, 
                                                     const std::vector<bool>& conectados) const {
    std::vector<Candidato> candidatos;
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    // Para cada vértice não conectado
    for (int v = 0; v < n; v++) {
        if (conectados[v]) continue;
        
        // Tentar conectar a cada vértice já conectado
        for (int p = 0; p < n; p++) {
            if (!conectados[p]) continue;
            
            // Verificar se existe aresta
            if (!grafo->existeAresta(v, p)) continue;
            
            // Verificar se respeita a capacidade
            if (!podeAdicionar(solucao, v, p)) continue;
            
            // Candidato válido
            double custo = grafo->getCusto(v, p);
            int subarvore = encontrarSubarvore(solucao, p);
            candidatos.emplace_back(v, p, custo, subarvore);
        }
    }
    
    // Ordenar por custo (menor primeiro)
    std::sort(candidatos.begin(), candidatos.end());
    
    return candidatos;
}

// Construção gulosa pura
Solucao Algoritmos::construirGuloso() const {
    Solucao solucao(grafo);
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    // List de elementos que foram conectados. Cada posiçao corresponde a um vértice. Se o vértice foi conectado, o valor é true, caso contrário, false.
    std::vector<bool> conectados(n, false);

    // A raiz é sempre conectada
    conectados[raiz] = true;
    int numConectados = 1;
    
    while (numConectados < n) {
        // Gerar candidatos
        std::vector<Candidato> candidatos = gerarCandidatos(solucao, conectados);
        
        if (candidatos.empty()) {
            // Não há candidatos viáveis - tentar forçar conexão com a raiz
            bool conectouAlgum = false;
            for (int v = 0; v < n; v++) {
                if (!conectados[v] && grafo->existeAresta(v, raiz)) {
                    // Verificar capacidade (nova subárvore)
                    if (grafo->getDemanda(v) <= grafo->getCapacidade()) {
                        solucao.setPai(v, raiz);
                        conectados[v] = true;
                        numConectados++;
                        conectouAlgum = true;
                        break;
                    }
                }
            }
            
            if (!conectouAlgum) {
                // Problema: não consegue conectar todos os vértices
                break;
            }
        } else {
            // Escolher o melhor candidato (menor custo)
            const Candidato& melhor = candidatos[0];
            solucao.setPai(melhor.vertice, melhor.pai);
            conectados[melhor.vertice] = true;
            numConectados++;
        }
    }
    
    solucao.calcularCusto();
    solucao.verificarViabilidade();
    
    return solucao;
}

// Construção gulosa randomizada
Solucao Algoritmos::construirGulosoRandomizado(double alpha) const {
    Solucao solucao(grafo);
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    std::vector<bool> conectados(n, false);
    conectados[raiz] = true;
    int numConectados = 1;
    
    while (numConectados < n) {
        // Gerar candidatos
        std::vector<Candidato> candidatos = gerarCandidatos(solucao, conectados);
        
        if (candidatos.empty()) {
            // Não há candidatos viáveis - tentar forçar conexão com a raiz
            std::vector<int> naoConectados;
            for (int v = 0; v < n; v++) {
                if (!conectados[v] && grafo->existeAresta(v, raiz)) {
                    if (grafo->getDemanda(v) <= grafo->getCapacidade()) {
                        naoConectados.push_back(v);
                    }
                }
            }
            
            if (naoConectados.empty()) {
                break;  // Não consegue continuar
            }
            
            // Escolher aleatoriamente entre os não conectados
            int idx = Randomizador::randInt(0, naoConectados.size() - 1);
            int v = naoConectados[idx];
            solucao.setPai(v, raiz);
            conectados[v] = true;
            numConectados++;
        } else {
            // Construir Lista Restrita de Candidatos (LRC)
            double custoMin = candidatos.front().custo;
            double custoMax = candidatos.back().custo;
            double limiar = custoMin + alpha * (custoMax - custoMin);
            
            // Filtrar candidatos dentro do limiar
            std::vector<Candidato> lrc;
            for (const auto& c : candidatos) {
                if (c.custo <= limiar) {
                    lrc.push_back(c);
                }
            }
            
            if (lrc.empty()) {
                lrc.push_back(candidatos[0]);  // Pelo menos o melhor
            }
            
            // Escolher aleatoriamente da LRC
            int idx = Randomizador::randInt(0, lrc.size() - 1);
            const Candidato& escolhido = lrc[idx];
            
            solucao.setPai(escolhido.vertice, escolhido.pai);
            conectados[escolhido.vertice] = true;
            numConectados++;
        }
    }
    
    solucao.calcularCusto();
    solucao.verificarViabilidade();
    
    return solucao;
}

// Executa algoritmo guloso
Solucao Algoritmos::executarGuloso(ResultadoExecucao& resultado) {
    Cronometro crono;
    crono.iniciar();
    
    Solucao melhorSolucao = construirGuloso();
    
    crono.parar();
    
    // Preencher resultado
    resultado.dataHora = Utils::getDataHoraAtual();
    resultado.instancia = grafo->getNomeInstancia();
    resultado.numVertices = grafo->getNumVertices();
    resultado.capacidade = grafo->getCapacidade();
    resultado.algoritmo = "guloso";
    resultado.alpha = -1;
    resultado.iteracoes = -1;
    resultado.tamanhoBloco = -1;
    resultado.semente = Randomizador::getSemente();
    resultado.tempoSegundos = crono.getTempoSegundos();
    resultado.melhorSolucao = melhorSolucao.getCustoTotal();
    resultado.mediaSolucoes = -1;
    resultado.melhorAlpha = -1;
    resultado.solucaoOtima = grafo->getSolucaoOtima();
    
    return melhorSolucao;
}

// Executa algoritmo guloso randomizado
Solucao Algoritmos::executarGulosoRandomizado(double alpha, int numIteracoes,
                                               ResultadoExecucao& resultado) {
    Cronometro crono;
    crono.iniciar();
    
    Solucao melhorSolucao(grafo);
    double melhorCusto = INFINITO;
    double somaCustos = 0;
    
    for (int iter = 0; iter < numIteracoes; iter++) {
        Solucao solucaoAtual = construirGulosoRandomizado(alpha);
        double custoAtual = solucaoAtual.getCustoTotal();
        somaCustos += custoAtual;
        
        if (custoAtual < melhorCusto && solucaoAtual.isValida()) {
            melhorCusto = custoAtual;
            melhorSolucao = solucaoAtual;
        }
    }
    
    crono.parar();
    
    // Preencher resultado
    resultado.dataHora = Utils::getDataHoraAtual();
    resultado.instancia = grafo->getNomeInstancia();
    resultado.numVertices = grafo->getNumVertices();
    resultado.capacidade = grafo->getCapacidade();
    resultado.algoritmo = "randomizado";
    resultado.alpha = alpha;
    resultado.iteracoes = numIteracoes;
    resultado.tamanhoBloco = -1;
    resultado.semente = Randomizador::getSemente();
    resultado.tempoSegundos = crono.getTempoSegundos();
    resultado.melhorSolucao = melhorCusto;
    resultado.mediaSolucoes = somaCustos / numIteracoes;
    resultado.melhorAlpha = -1;
    resultado.solucaoOtima = grafo->getSolucaoOtima();
    
    return melhorSolucao;
}

// Executa algoritmo guloso randomizado reativo
Solucao Algoritmos::executarGulosoReativo(const std::vector<double>& alphas,
                                           int numIteracoes, int tamanhoBloco,
                                           ResultadoExecucao& resultado) {
    Cronometro crono;
    crono.iniciar();
    
    int numAlphas = alphas.size();
    if (numAlphas == 0) {
        resultado.melhorSolucao = INFINITO;
        return Solucao(grafo);
    }
    
    // Inicializar probabilidades uniformes
    std::vector<double> probabilidades(numAlphas, 1.0 / numAlphas);
    
    // Estatísticas por alpha
    std::vector<double> somaQualidade(numAlphas, 0);
    std::vector<int> contadorUso(numAlphas, 0);
    std::vector<double> melhorPorAlpha(numAlphas, INFINITO);
    
    Solucao melhorSolucaoGlobal(grafo);
    double melhorCustoGlobal = INFINITO;
    int melhorAlphaIdx = 0;
    double somaCustos = 0;
    
    for (int iter = 0; iter < numIteracoes; iter++) {
        // Selecionar alpha baseado nas probabilidades
        double r = Randomizador::randDouble();
        double acumulado = 0;
        int alphaIdx = 0;
        
        for (int i = 0; i < numAlphas; i++) {
            acumulado += probabilidades[i];
            if (r <= acumulado) {
                alphaIdx = i;
                break;
            }
        }
        
        // Construir solução com o alpha selecionado
        Solucao solucaoAtual = construirGulosoRandomizado(alphas[alphaIdx]);
        double custoAtual = solucaoAtual.getCustoTotal();
        somaCustos += custoAtual;
        
        // Atualizar estatísticas
        contadorUso[alphaIdx]++;
        
        if (custoAtual < melhorPorAlpha[alphaIdx]) {
            melhorPorAlpha[alphaIdx] = custoAtual;
        }
        
        if (custoAtual < melhorCustoGlobal && solucaoAtual.isValida()) {
            melhorCustoGlobal = custoAtual;
            melhorSolucaoGlobal = solucaoAtual;
            melhorAlphaIdx = alphaIdx;
        }
        
        // Atualizar probabilidades a cada bloco
        if ((iter + 1) % tamanhoBloco == 0 && iter > 0) {
            // Calcular qualidade de cada alpha
            std::vector<double> qualidade(numAlphas, 0);
            double somaQualidades = 0;
            
            for (int i = 0; i < numAlphas; i++) {
                if (contadorUso[i] > 0 && melhorPorAlpha[i] < INFINITO) {
                    // Qualidade = melhor global / melhor do alpha
                    // Quanto menor o custo, maior a qualidade
                    qualidade[i] = melhorCustoGlobal / melhorPorAlpha[i];
                    qualidade[i] = std::pow(qualidade[i], 2);  // Elevar ao quadrado para amplificar
                } else {
                    qualidade[i] = 0.1;  // Valor mínimo para alphas não usados
                }
                somaQualidades += qualidade[i];
            }
            
            // Normalizar para obter probabilidades
            if (somaQualidades > 0) {
                for (int i = 0; i < numAlphas; i++) {
                    probabilidades[i] = qualidade[i] / somaQualidades;
                    
                    // Garantir probabilidade mínima
                    if (probabilidades[i] < 0.05) {
                        probabilidades[i] = 0.05;
                    }
                }
                
                // Renormalizar
                double soma = 0;
                for (double p : probabilidades) soma += p;
                for (double& p : probabilidades) p /= soma;
            }
        }
    }
    
    crono.parar();
    
    // Preencher resultado
    resultado.dataHora = Utils::getDataHoraAtual();
    resultado.instancia = grafo->getNomeInstancia();
    resultado.numVertices = grafo->getNumVertices();
    resultado.capacidade = grafo->getCapacidade();
    resultado.algoritmo = "reativo";
    resultado.alpha = -1;  // Vários alphas
    resultado.iteracoes = numIteracoes;
    resultado.tamanhoBloco = tamanhoBloco;
    resultado.semente = Randomizador::getSemente();
    resultado.tempoSegundos = crono.getTempoSegundos();
    resultado.melhorSolucao = melhorCustoGlobal;
    resultado.mediaSolucoes = somaCustos / numIteracoes;
    resultado.melhorAlpha = alphas[melhorAlphaIdx];
    resultado.solucaoOtima = grafo->getSolucaoOtima();
    
    return melhorSolucaoGlobal;
}
