# CMST - Capacitated Minimum Spanning Tree

Implementação de algoritmos heurísticos para o problema da Árvore Geradora Mínima com Restrição de Capacidade (CMST).

## Requisitos

- G++ (C++17)
- Make

```bash
sudo apt install g++ make
```

## Compilação

```bash
make          # Compila o projeto
make clean    # Remove arquivos compilados
make rebuild  # Limpa e recompila
```

## Uso

```bash
./cmst <instancia> <algoritmo> [opções]
```

### Algoritmos Disponíveis

| Algoritmo | Descrição |
|-----------|-----------|
| `guloso` | Construção gulosa determinística |
| `randomizado` | Guloso randomizado com parâmetro α |
| `reativo` | Guloso randomizado reativo com múltiplos α's |

## Exemplos

### Algoritmo Guloso
```bash
./cmst capmstnew/TC4001.DAT guloso
```

### Algoritmo Randomizado
```bash
# Com parâmetros padrão (α=0.3, 30 iterações)
./cmst capmstnew/TC4001.DAT randomizado

# Com parâmetros customizados
./cmst capmstnew/TC4001.DAT randomizado --alpha 0.1 --iter 50

# Com semente específica (para reproduzir resultados)
./cmst capmstnew/TC4001.DAT randomizado --alpha 0.3 --iter 30 --seed 12345
```

### Algoritmo Reativo
```bash
# Com parâmetros padrão (α's=0.1,0.3,0.5, 300 iterações, bloco=30)
./cmst capmstnew/TC4001.DAT reativo

# Com parâmetros customizados
./cmst capmstnew/TC4001.DAT reativo --alphas 0.1,0.2,0.3,0.5 --iter 500 --bloco 50
```

## Parâmetros

| Parâmetro | Algoritmo | Descrição | Padrão |
|-----------|-----------|-----------|--------|
| `--alpha` | randomizado | Fator de aleatoriedade (0.0 a 1.0) | 0.3 |
| `--alphas` | reativo | Lista de α's separados por vírgula | 0.1,0.3,0.5 |
| `--iter` | randomizado/reativo | Número de iterações | 30 / 300 |
| `--bloco` | reativo | Tamanho do bloco para atualização | 30 |
| `--seed` | todos | Semente de randomização | auto |
| `--csv` | todos | Arquivo CSV de saída | resultados.csv |
| `--verbose` | todos | Modo detalhado | desativado |
| `--help` | - | Mostra ajuda | - |

## Saída

### Terminal
O programa exibe:
- Custo da melhor solução encontrada
- Tempo de execução
- Desvio percentual em relação ao ótimo (se conhecido)
- Solução no formato CS Academy (para visualização)

### Arquivo CSV
Cada execução adiciona uma linha ao `resultados.csv` com:
- Data/hora, instância, algoritmo, parâmetros
- Semente utilizada, tempo de execução
- Melhor solução, média das soluções
- Solução ótima conhecida

## Estrutura do Projeto

```
GT-Project-UFJF/
├── includes/          # Headers (.h)
├── src/               # Código fonte (.cpp)
├── capmstnew/         # Instâncias de teste
├── Makefile           # Compilação
└── README.md
```

## Instâncias

As instâncias estão na pasta `capmstnew/` no formato OR-Library:
- `TC*.DAT` - Instâncias com custos em grafo completo
- `TE*.DAT` - Instâncias com custos euclidianos

## Visualização da Solução

A solução é impressa no formato compatível com [CS Academy Graph Editor](https://csacademy.com/app/graph_editor/):
```
0 1
0 3
1 5
...
```
