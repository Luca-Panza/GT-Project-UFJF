#include "../includes/algoritmos.h"
#include <algorithm>
#include <cmath>
#include <map>

// Construtor
Algoritmos::Algoritmos(const Grafo* g) : grafo(g), proximoSubId(1), cacheValido(false) {
    int n = g->getNumVertices();
    demandaSubarvore.resize(n + 1, 0);  // Reserve space for subtrees (1-indexed)
    subarvoreVertice.resize(n, -1);
}

// Destrutor
Algoritmos::~Algoritmos() {}

// Inicializa o cache
void Algoritmos::inicializarCache(const Solucao& solucao) const {
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    // Reset cache
    std::fill(demandaSubarvore.begin(), demandaSubarvore.end(), 0);
    std::fill(subarvoreVertice.begin(), subarvoreVertice.end(), -1);
    filhoRaizParaSubId.clear();
    proximoSubId = 1;
    
    // Raiz pertence à subárvore 0
    subarvoreVertice[raiz] = 0;
    
    // Inicializar subárvores existentes (se houver vértices já conectados)
    for (int i = 0; i < n; i++) {
        if (i != raiz && solucao.getPai(i) == raiz) {
            // Este é um filho direto da raiz - nova subárvore
            int subId = proximoSubId++;
            filhoRaizParaSubId[i] = subId;
            subarvoreVertice[i] = subId;
            demandaSubarvore[subId] = grafo->getDemanda(i);
            
            // Adicionar todos os descendentes usando BFS
            std::vector<int> fila;
            fila.push_back(i);
            size_t idx = 0;
            
            while (idx < fila.size()) {
                int v = fila[idx++];
                for (int j = 0; j < n; j++) {
                    if (j != raiz && solucao.getPai(j) == v) {
                        subarvoreVertice[j] = subId;
                        demandaSubarvore[subId] += grafo->getDemanda(j);
                        fila.push_back(j);
                    }
                }
            }
        }
    }
    
    cacheValido = true;
}

// Atualiza o cache incrementalmente
void Algoritmos::atualizarCacheDemanda(Solucao& solucao, int verticeAdicionado, int paiVertice) const {
    int raiz = grafo->getRaiz();
    int demandaVertice = grafo->getDemanda(verticeAdicionado);
    
    if (paiVertice == raiz) {
        // Nova subárvore
        int subId = proximoSubId++;
        filhoRaizParaSubId[verticeAdicionado] = subId;
        subarvoreVertice[verticeAdicionado] = subId;
        if (subId >= (int)demandaSubarvore.size()) {
            demandaSubarvore.resize(subId + 1, 0);
        }
        demandaSubarvore[subId] = demandaVertice;
    } else {
        // Adicionar a subárvore existente
        // O pai deve estar no cache porque foi adicionado anteriormente
        int subId = -1;
        if (cacheValido && paiVertice >= 0 && paiVertice < (int)subarvoreVertice.size()) {
            subId = subarvoreVertice[paiVertice];
        }
        
        // Se não encontrou no cache, usar encontrarSubarvore como fallback
        if (subId <= 0) {
            subId = encontrarSubarvore(solucao, paiVertice);
        }
        
        if (subId > 0) {
            subarvoreVertice[verticeAdicionado] = subId;
            if (subId >= (int)demandaSubarvore.size()) {
                demandaSubarvore.resize(subId + 1, 0);
            }
            demandaSubarvore[subId] += demandaVertice;
        }
    }
}

// Invalida o cache
void Algoritmos::invalidarCache() const {
    cacheValido = false;
    std::fill(demandaSubarvore.begin(), demandaSubarvore.end(), 0);
    std::fill(subarvoreVertice.begin(), subarvoreVertice.end(), -1);
    filhoRaizParaSubId.clear();
    proximoSubId = 1;
}

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
    
    // Se cache está válido e temos informação, usar cache
    if (cacheValido && vertice >= 0 && vertice < (int)subarvoreVertice.size()) {
        if (subarvoreVertice[vertice] >= 0) {
            return subarvoreVertice[vertice];
        }
    }
    
    // Fallback: subir até encontrar o filho direto da raiz
    int atual = vertice;
    while (atual != -1 && atual != raiz) {
        int pai = solucao.getPai(atual);
        if (pai == raiz) {
            // Encontramos o filho direto da raiz
            // Usar mapa para lookup O(1)
            auto it = filhoRaizParaSubId.find(atual);
            if (it != filhoRaizParaSubId.end()) {
                return it->second;
            }
            // Se não está no mapa, calcular (deve ser raro)
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
    
    // Usar cache para obter demanda da subárvore (O(1) em vez de O(n))
    int subId = encontrarSubarvore(solucao, paiVertice);
    if (subId <= 0) {
        return true;  // Não conectado ou raiz
    }
    
    // Obter demanda do cache
    int demandaAtual = 0;
    if (cacheValido && subId < (int)demandaSubarvore.size()) {
        demandaAtual = demandaSubarvore[subId];
    } else {
        // Fallback: calcular demanda (não deveria acontecer se cache está válido)
        demandaAtual = calcularDemandaSubarvore(solucao, subId);
    }
    
    return (demandaAtual + demandaVertice) <= capacidade;
}

// Gera lista de candidatos viáveis
std::vector<Candidato> Algoritmos::gerarCandidatos(const Solucao& solucao, const std::vector<bool>& conectados) const {
    int n = grafo->getNumVertices();
    int raiz = grafo->getRaiz();
    
    // Pre-filtrar vértices para reduzir iterações
    std::vector<int> naoConectados;
    std::vector<int> conectadosList;
    naoConectados.reserve(n);
    conectadosList.reserve(n);
    
    for (int i = 0; i < n; i++) {
        if (conectados[i]) {
            conectadosList.push_back(i);
        } else {
            naoConectados.push_back(i);
        }
    }
    
    // Pre-alocar candidatos com estimativa
    std::vector<Candidato> candidatos;
    candidatos.reserve(naoConectados.size() * conectadosList.size() / 4);  // Estimativa conservadora
    
    // Iterar apenas sobre listas filtradas
    for (int v : naoConectados) {
        for (int p : conectadosList) {
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
    
    // Inicializar cache
    inicializarCache(solucao);
    
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
                        atualizarCacheDemanda(solucao, v, raiz);
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
            atualizarCacheDemanda(solucao, melhor.vertice, melhor.pai);
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
    
    // Inicializar cache
    inicializarCache(solucao);
    
    std::vector<bool> conectados(n, false);
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
                        atualizarCacheDemanda(solucao, v, raiz);
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
            // Construir Lista Restrita de Candidatos (LRC) baseada em cardinalidade
            int tamanhoLRC = std::max(1, (int)std::floor(alpha * candidatos.size()));
            
            // Garantir que não ultrapasse o tamanho da lista
            tamanhoLRC = std::min(tamanhoLRC, (int)candidatos.size());
            
            // Escolher aleatoriamente entre os primeiros tamanhoLRC candidatos
            int idx = Randomizador::randInt(0, tamanhoLRC - 1);
            const Candidato& escolhido = candidatos[idx];
            
            solucao.setPai(escolhido.vertice, escolhido.pai);
            conectados[escolhido.vertice] = true;
            numConectados++;
            atualizarCacheDemanda(solucao, escolhido.vertice, escolhido.pai);
        }
    }
    
    solucao.calcularCusto();
    solucao.verificarViabilidade();
    
    return solucao;
}

// Executa algoritmo guloso
Solucao Algoritmos::executarGuloso(ResultadoExecucao& resultado, bool verbose) {
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
Solucao Algoritmos::executarGulosoRandomizado(double alpha, int numIteracoes, ResultadoExecucao& resultado, bool verbose) {
    Cronometro crono;
    crono.iniciar();
    
    Solucao melhorSolucao(grafo);
    double melhorCusto = INFINITO;

    // Solucao melhorSolucao = construirGuloso();
    // double melhorCusto = melhorSolucao.getCustoTotal();

    double somaCustos = 0;
    
    for (int iter = 0; iter < numIteracoes; iter++) {
        Solucao solucaoAtual = construirGulosoRandomizado(alpha);
        double custoAtual = solucaoAtual.getCustoTotal();
        
        if (verbose) {
            std::cout << "\nIter: " << iter << std::endl;
            std::cout << "Custo total: " << custoAtual << std::endl;
            std::cout << "Melhor custo: " << melhorCusto << std::endl;
            std::cout << "Viabilidade: " << solucaoAtual.isValida() << std::endl;   
        }

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
Solucao Algoritmos::executarGulosoReativo(const std::vector<double>& alphas, int numIteracoes, int tamanhoBloco, ResultadoExecucao& resultado, bool verbose) {
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
    double FATOR_AMPLIFICACAO = 3;
    
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

            if (verbose) {
                std::cout << "\nIter: " << iter << std::endl;

                for(int i = 0; i < numAlphas; i++) {
                    std::cout << "Alpha: " << alphas[i] << " - Probabilidade: " << probabilidades[i] << std::endl;
                }
            }

            // Calcular qualidade de cada alpha
            std::vector<double> qualidade(numAlphas, 0);
            double somaQualidades = 0;
            
            for (int i = 0; i < numAlphas; i++) {
                if (contadorUso[i] > 0 && melhorPorAlpha[i] < INFINITO) {
                    // Qualidade = melhor global / melhor do alpha
                    // Quanto menor o custo, maior a qualidade
                    qualidade[i] = melhorCustoGlobal / melhorPorAlpha[i];
                    qualidade[i] = std::pow(qualidade[i], FATOR_AMPLIFICACAO);  // Elevar ao quadrado para amplificar
                } else {
                    qualidade[i] = 0.1;  // Valor mínimo para alphas não usados
                }
                somaQualidades += qualidade[i];
            }

            if (verbose) {
                for(int i = 0; i < numAlphas; i++) {
                    std::cout << "Alpha: " << alphas[i] << " - Qualidade: " << qualidade[i] << std::endl;
                }

                for(int i = 0; i < numAlphas; i++) {
                    std::cout << "Alpha: " << alphas[i] << " - Melhor por Alpha: " << melhorPorAlpha[i] << std::endl;
                }

                std::cout << "Soma Qualidades: " << somaQualidades << std::endl;
            }
            
            // Normalizar para obter probabilidades
            if (somaQualidades > 0) {
                for (int i = 0; i < numAlphas; i++) {
                    probabilidades[i] = qualidade[i] / somaQualidades;
                }
            }

            if (verbose) {
                for(int i = 0; i < numAlphas; i++) {
                    std::cout << "Alpha: " << alphas[i] << " - Nova probabilidade: " << probabilidades[i] << std::endl;
                }
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
