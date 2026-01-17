# Capacitated Minimum Spanning Tree (CMST) - Solution Explanation

## Problem Overview

The **Capacitated Minimum Spanning Tree (CMST)** problem is a variant of the classic Minimum Spanning Tree problem with an additional capacity constraint. Given:

- A complete graph with `n` vertices
- A root vertex (typically vertex 0)
- A capacity limit `Q` for each subtree
- Edge costs between all pairs of vertices
- Demands for each vertex (default: 1)

The goal is to find a spanning tree rooted at the root vertex such that:

1. The total cost of all edges is minimized
2. Each subtree directly connected to the root has a total demand that does not exceed the capacity `Q`

## Solution Architecture

The codebase implements **construction-based heuristic algorithms** that build solutions incrementally by connecting vertices one at a time while respecting capacity constraints.

### Core Components

#### 1. **Grafo (Graph) Class** (`grafo.h/cpp`)

- Represents the problem instance
- Stores the adjacency matrix with edge costs (`matrizAdj`)
- Manages vertex demands (`demandas[]`, default: 1 for all vertices)
- Stores capacity constraint (`capacidade`, Q)
- Loads instances from OR-Library format files with special handling for:
  - Fixed-width format (4 characters per value)
  - Concatenated values (e.g., "311000" = 31 and 1000)
  - Optional optimal solution value in last line
- Provides access to graph properties (number of vertices, capacity, costs, etc.)
- Root vertex is always vertex 0

#### 2. **Solucao (Solution) Class** (`solucao.h/cpp`)

- Represents a CMST solution as a tree structure
- Uses a parent array (`pai[]`) to represent the tree: `pai[i]` = parent of vertex `i`
- Root has `pai[root] = -1`
- Tracks subtrees using `subarvore[]` array: `subarvore[i]` = subtree ID of vertex `i`
- Identifies subtrees using BFS from root's direct children (`identificarSubarvores()`)
- Validates solutions by checking capacity constraints for each subtree
- Calculates total solution cost by summing edge costs
- Provides methods for:
  - Getting subtree vertices: `getVerticesSubarvore(idSubarvore)`
  - Getting subtree demand: `getDemandaSubarvore(idSubarvore)`
  - Formatting output: `toStringCSAcademy()` for visualization

#### 3. **Algoritmos (Algorithms) Class** (`algoritmos.h/cpp`)

- Implements three heuristic algorithms:
  - **Greedy**: Deterministic construction (`construirGuloso()`)
  - **Greedy Randomized**: Construction with randomness controlled by parameter `α` (`construirGulosoRandomizado(alpha)`)
  - **Reactive Greedy Randomized**: Adaptive algorithm using multiple `α` values (`executarGulosoReativo()`)
- Manages caching system for efficient capacity checking:
  - `inicializarCache()`: Initializes cache from existing solution
  - `atualizarCacheDemanda()`: Incrementally updates cache when vertex is added
  - `invalidarCache()`: Resets cache when needed
- Core helper functions:
  - `gerarCandidatos()`: Generates viable candidate edges
  - `podeAdicionar()`: Checks capacity constraints
  - `encontrarSubarvore()`: Finds subtree ID for a vertex
  - `calcularDemandaSubarvore()`: Calculates subtree demand (fallback if cache invalid)

#### 4. **Utils (Utilities)** (`utils.h/cpp`)

- **Randomizador**: Random number generation with seed management
  - Uses `std::mt19937` Mersenne Twister generator
  - Supports seed initialization (automatic from time or manual)
  - Provides `randInt(min, max)` and `randDouble()` methods
- **Cronometro**: Execution time measurement
  - High-resolution clock for precise timing
  - Returns time in seconds or milliseconds
- **EscritorCSV**: CSV result logging
  - Writes execution results to CSV file
  - Automatically creates header if file doesn't exist
  - Appends results to existing file
- **Utils namespace**: Date/time formatting and number formatting utilities
- **ResultadoExecucao**: Structure storing all execution metadata and results

## Algorithm Details

### Solution Construction Strategy

All three algorithms follow a similar construction pattern:

1. **Initialization**: 
   - Create empty solution with only root vertex connected
   - Initialize cache for subtree demands (`inicializarCache()`)
   - Mark root as connected in `conectados[]` array

2. **Iterative Construction**: While `numConectados < n`:
   - Generate a list of viable candidates using `gerarCandidatos()`
   - If no candidates exist: fallback to force connection to root (if capacity allows)
   - Select a candidate based on the algorithm strategy:
     - **Greedy**: First candidate (lowest cost)
     - **Randomized**: Random from LRC
     - **Reactive**: Random from LRC using selected α
   - Add the selected edge: `solucao.setPai(vertice, pai)`
   - Update cache incrementally: `atualizarCacheDemanda(solucao, vertice, pai)`
   - Mark vertex as connected

3. **Finalization**:
   - Calculate total cost: `solucao.calcularCusto()`
   - Validate solution: `solucao.verificarViabilidade()`
   - Return solution

### Key Data Structures

#### Candidato Structure

Represents a potential edge to be added to the solution:

```cpp
struct Candidato {
    int vertice;        // Vertex to be connected
    int pai;            // Parent vertex (where it will connect)
    double custo;       // Edge cost
    int subarvore;      // Subtree ID of the parent
    
    // Comparison operator for sorting (by cost, ascending)
    bool operator<(const Candidato& outro) const;
};
```

Candidates are sorted by cost (ascending) to enable efficient greedy selection.

### Key Functions

#### Candidate Generation (`gerarCandidatos`)

For each unconnected vertex `v`, the algorithm:

- Pre-filters vertices into `conectadosList` and `naoConectados` for efficiency
- For each unconnected vertex `v`:
  - Considers all already-connected vertices `p` as potential parents
  - Checks if edge `(v, p)` exists in the graph using `grafo->existeAresta(v, p)`
  - Verifies if adding `v` as a child of `p` respects the capacity constraint using `podeAdicionar()`
  - Finds the subtree ID of parent `p` using `encontrarSubarvore()`
- Creates a `Candidato` structure with: `(vertice, pai, custo, subarvore)`
- Sorts candidates by edge cost (ascending) using `std::sort`
- Returns sorted candidate list

**Optimization**: Pre-filtering reduces iterations from O(n²) to O(|conectados| × |não_conectados|)

#### Capacity Checking (`podeAdicionar`)

Before adding a vertex `v` as a child of `p`:

- If `p` is the root: Check if `demand(v) ≤ Q` (new subtree, always allowed if demand fits)
- If `p` is not the root:
  - Find which subtree `p` belongs to using `encontrarSubarvore(solucao, p)`
  - Get current demand of that subtree from cache (O(1)) or calculate if cache invalid
  - Verify: `current_demand + demand(v) ≤ Q`

**Cache Optimization**: Uses `demandaSubarvore[subId]` cache for O(1) lookup instead of O(n) traversal

#### Subtree Identification (`encontrarSubarvore`)

- If vertex is root: returns 0
- If cache is valid: uses `subarvoreVertice[vertice]` for O(1) lookup
- Otherwise: traverses up the tree from vertex until reaching a direct child of the root
- Each direct child of the root starts a new subtree (IDs: 1, 2, 3, ...)
- Uses `filhoRaizParaSubId` map for O(1) lookup of subtree ID from root child
- Returns the subtree ID for capacity tracking

**Cache System**: Maintains `subarvoreVertice[]` array and `filhoRaizParaSubId` map for efficient subtree lookups

### Algorithm 1: Greedy (Deterministic)

**Strategy**: Always select the candidate with the lowest cost.

```cpp
Solucao construirGuloso()
```

**Process**:

1. Initialize cache for subtree demands
2. Mark root as connected
3. While there are unconnected vertices:
   - Generate all viable candidates using `gerarCandidatos()`
   - If no candidates exist: try forcing connection to root (fallback)
   - Select the first candidate (lowest cost after sorting)
   - Add the edge: `solucao.setPai(vertice, pai)`
   - Update cache incrementally: `atualizarCacheDemanda()`
   - Mark vertex as connected
4. Calculate total cost: `solucao.calcularCusto()`
5. Validate solution: `solucao.verificarViabilidade()`

**Time Complexity**: O(n² × log(n)) per solution (due to sorting candidates each iteration)

**Characteristics**:

- Fast and deterministic
- Produces the same solution for the same input
- May get trapped in local optima
- Uses cache for efficient capacity checking

### Algorithm 2: Greedy Randomized

**Strategy**: Use a **Restricted Candidate List (LRC)** based on parameter `α ∈ [0, 1]` using a **cardinality-based** approach.

```cpp
Solucao construirGulosoRandomizado(double alpha)
```

**Process**:

1. Generate and sort candidates by cost (ascending)
2. Calculate LRC size: `tamanhoLRC = max(1, floor(α × candidatos.size()))`
3. Build LRC: the first `tamanhoLRC` candidates (lowest cost)
4. Randomly select one candidate from the LRC using uniform distribution
5. Add the selected edge
6. Repeat until all vertices are connected

**Parameter `α`**:

- `α = 0`: Pure greedy (only best candidate, LRC size = 1)
- `α = 1`: All candidates equally likely (LRC size = all candidates)
- `α ∈ (0, 1)`: Balance between quality and diversity (proportional LRC size)

**Implementation Details**:

- Uses cardinality-based LRC (not value-based threshold)
- LRC size is proportional to the number of candidates
- Random selection uses `Randomizador::randInt(0, tamanhoLRC - 1)`

**Execution**:

- Runs multiple iterations (default: 30)
- Tracks best solution across all iterations
- Only valid solutions (respecting capacity) are considered for best solution
- Returns the best solution found across all iterations

### Algorithm 3: Reactive Greedy Randomized

**Strategy**: Adaptively manage multiple `α` values using performance feedback with quality-based probability updates.

```cpp
Solucao executarGulosoReativo(vector<double> alphas, int iteracoes, int tamanhoBloco)
```

**Process**:

1. Initialize uniform probabilities for each `α` value: `prob[i] = 1.0 / numAlphas`
2. For each iteration:
   - Select an `α` based on current probabilities (roulette wheel selection)
   - Construct a solution using `construirGulosoRandomizado(α)`
   - Track the best solution found with each `α` (`melhorPorAlpha[i]`)
   - Track the global best solution (`melhorCustoGlobal`)
3. Every `tamanhoBloco` iterations:
   - Calculate quality score for each `α`: 
     - `qualidade[i] = (melhorCustoGlobal / melhorPorAlpha[i])^FATOR_AMPLIFICACAO`
     - Where `FATOR_AMPLIFICACAO = 3` (amplifies differences)
   - Normalize qualities to probabilities: `prob[i] = qualidade[i] / somaQualidades`
   - If an alpha hasn't been used, assign minimum quality (0.1) to maintain exploration

**Adaptive Mechanism**:

- Alphas that produce better solutions (lower costs) get higher quality scores
- Quality is amplified by power of 3 to emphasize differences
- Probabilities are proportional to quality, automatically focusing on promising `α` values
- Unused alphas get minimum quality to maintain exploration

**Parameters**:

- `alphas`: List of `α` values to explore (default: [0.1, 0.3, 0.5])
- `iteracoes`: Total number of iterations (default: 300)
- `tamanhoBloco`: Block size for probability updates (default: 30)

**Quality Calculation**:

- Higher quality = better performance (lower cost)
- Formula: `qualidade[i] = (melhorCustoGlobal / melhorPorAlpha[i])^FATOR_AMPLIFICACAO`
- Where `FATOR_AMPLIFICACAO = 3` (amplifies differences between alphas)
- If `melhorPorAlpha[i]` equals `melhorCustoGlobal`, quality = 1.0
- If `melhorPorAlpha[i]` is worse, quality < 1.0 (proportional to ratio)
- Unused alphas get minimum quality of 0.1 to maintain exploration
- Probabilities are normalized: `prob[i] = qualidade[i] / somaQualidades`

## Solution Representation

### Tree Structure

The solution is represented using a **parent array**:

- `pai[i] = j` means vertex `i` is a child of vertex `j`
- `pai[root] = -1` (root has no parent)
- This implicitly defines a tree structure

### Subtree Management

- Each direct child of the root starts a new subtree
- Subtree IDs are assigned sequentially (1, 2, 3, ...)
- The root belongs to "subtree 0"
- All descendants of a root's child belong to the same subtree

### Capacity Validation

For each subtree `s`:

```
sum(demand(v) for all v in subtree s) ≤ Q
```

## Example Execution Flow

Consider a graph with 5 vertices, capacity Q=3, and root=0:

1. **Initial State**: Only vertex 0 (root) is connected
2. **Iteration 1**:
   - Candidates: (1→0), (2→0), (3→0), (4→0)
   - Select: (1→0) with cost 5.0
   - Subtree 1 created: {1}
3. **Iteration 2**:
   - Candidates: (2→0), (2→1), (3→0), (3→1), (4→0), (4→1)
   - If demand(2)+demand(1) ≤ 3: (2→1) is viable
   - Select: (2→1) with cost 3.0
   - Subtree 1: {1, 2}
4. **Iteration 3**:
   - If subtree 1 is full (demand=3), only candidates connecting to root are viable
   - Select: (3→0) with cost 6.0
   - Subtree 2 created: {3}
5. **Iteration 4**:
   - Select: (4→3) or (4→0) depending on capacity
   - Complete solution

## Performance Considerations

### Time Complexity

- **Greedy**: O(n² × log(n)) per solution
  - Candidate generation: O(n²) worst case
  - Sorting candidates: O(n² × log(n²)) = O(n² × log(n))
  - Capacity checking: O(1) with cache, O(n) without cache
  
- **Randomized**: O(n² × log(n) × iterations)
  - Same per-iteration complexity as Greedy
  - Multiplied by number of iterations (default: 30)
  
- **Reactive**: O(n² × log(n) × iterations + |alphas| × (iterations / tamanhoBloco))
  - Same per-iteration complexity as Randomized
  - Additional O(|alphas|) overhead for probability updates every `tamanhoBloco` iterations

### Space Complexity

- O(n²) for the graph adjacency matrix (`matrizAdj`)
- O(n) for solution representation (`pai[]`, `subarvore[]`)
- O(n) for cache structures (`demandaSubarvore[]`, `subarvoreVertice[]`)
- O(n) for candidate lists (worst case: all vertices can connect to all others)
- O(|alphas|) for reactive algorithm probability tracking
- **Total**: O(n²) dominated by graph storage

### Optimization Strategies

1. **Caching System**: 
   - `demandaSubarvore[subId]`: O(1) lookup for subtree demand
   - `subarvoreVertice[v]`: O(1) lookup for vertex's subtree ID
   - `filhoRaizParaSubId`: O(1) map from root child to subtree ID
   - Cache is initialized once and updated incrementally when vertices are added
   - Reduces capacity checking from O(n) to O(1)

2. **Pre-filtering**: 
   - Separates connected and unconnected vertices into lists
   - Reduces candidate generation from O(n²) to O(|conectados| × |não_conectados|)

3. **Incremental Updates**: 
   - Cache is updated incrementally when each vertex is added
   - Avoids recalculating entire subtree demands each time

4. **Fallback Mechanism**: 
   - If no candidates exist, tries forcing connection to root
   - Prevents premature termination when solution is still possible

## Input/Output Format

### Input (OR-Library Format)

```
n Q
<matrix of n×n costs in fixed-width format (4 chars per value)>
<optional: optimal solution value on last line>
```

**Format Details**:
- First line: number of vertices `n` and capacity `Q`
- Following lines: cost matrix in fixed-width format (4 characters per value)
- Special handling: concatenated values (e.g., "311000" parsed as 31 and 1000)
- Last line (optional): known optimal solution value
- Values of 1000 on diagonal represent infinity (no self-loops)

### Output

- **Terminal**: 
  - Algorithm name, seed, execution time
  - Best solution cost, average solution cost (for randomized algorithms)
  - Best alpha (for reactive algorithm)
  - Deviation from optimal (if known)
  - Solution validity status
  - Solution edges in CS Academy format
  
- **CSV File**: Detailed execution results with columns:
  - `data_hora, instancia, n_vertices, capacidade, algoritmo, alpha, iteracoes, tamanho_bloco, semente, tempo_segundos, melhor_solucao, media_solucoes, melhor_alpha, solucao_otima`
  
- **CS Academy Format**: Solution edges as `origem destino` (one edge per line) for visualization

## Strengths and Limitations

### Strengths

- ✅ Handles large instances efficiently with caching optimizations
- ✅ Provides multiple algorithm variants (greedy, randomized, reactive)
- ✅ Adaptive algorithm (reactive) automatically adjusts to problem characteristics
- ✅ Reproducible results through seed control
- ✅ Comprehensive result logging to CSV
- ✅ Efficient capacity checking using O(1) cache lookups
- ✅ Pre-filtering reduces candidate generation overhead
- ✅ Incremental cache updates avoid expensive recalculations

### Limitations

- ⚠️ Heuristic approach (no optimality guarantee)
- ⚠️ Construction-based (no local search/improvement phase)
- ⚠️ May produce infeasible solutions if capacity constraints are too tight
- ⚠️ Cache invalidation can occur if solution is modified externally
- ⚠️ Fixed-width format parsing may fail on malformed input files

## Implementation Details

### Cache Management

The caching system is crucial for performance:

1. **Initialization**: `inicializarCache()` builds cache from existing solution using BFS
2. **Incremental Updates**: `atualizarCacheDemanda()` updates cache when vertex is added:
   - If parent is root: creates new subtree ID
   - If parent is not root: adds demand to existing subtree
3. **Cache Structure**:
   - `demandaSubarvore[subId]`: Total demand of subtree `subId`
   - `subarvoreVertice[v]`: Subtree ID of vertex `v`
   - `filhoRaizParaSubId`: Map from root's direct child to subtree ID

### Default Parameters

- **Greedy Randomized**:
  - `alpha = 0.3`
  - `iteracoes = 30`
  
- **Reactive Greedy Randomized**:
  - `alphas = [0.1, 0.3, 0.5]`
  - `iteracoes = 300`
  - `tamanhoBloco = 30`
  - `FATOR_AMPLIFICACAO = 3`

### Command-Line Interface

The program supports command-line arguments:

```bash
./cmst <instancia> <algoritmo> [opcoes]
```

**Algorithms**: `guloso`, `randomizado`, `reativo`

**Options**:
- `--alpha <valor>`: Alpha value for randomized (0.0-1.0)
- `--alphas <lista>`: Comma-separated alpha values for reactive
- `--iter <num>`: Number of iterations
- `--bloco <num>`: Block size for reactive
- `--seed <num>`: Random seed
- `--csv <arquivo>`: CSV output file
- `--verbose`: Verbose output mode

## Future Improvements

Potential enhancements:

1. **Local Search**: Add improvement phase after construction (2-opt, 3-opt, edge swaps)
2. **Hybrid Approaches**: Combine with exact methods for small subproblems
3. **Advanced Adaptive Strategies**: More sophisticated probability update mechanisms (exponential smoothing, decay)
4. **Parallelization**: Run multiple iterations in parallel
5. **Memory Optimization**: Reduce space complexity for very large instances
6. **Better Fallback**: Improve handling when no candidates exist
