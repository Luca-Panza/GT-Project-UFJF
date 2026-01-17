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
- Stores the adjacency matrix with edge costs
- Manages vertex demands and capacity constraints
- Loads instances from OR-Library format files
- Provides access to graph properties (number of vertices, capacity, costs, etc.)

#### 2. **Solucao (Solution) Class** (`solucao.h/cpp`)

- Represents a CMST solution as a tree structure
- Uses a parent array (`pai[]`) to represent the tree: `pai[i]` = parent of vertex `i`
- Tracks subtrees: each subtree is identified by a unique ID
- Validates solutions by checking capacity constraints
- Calculates total solution cost

#### 3. **Algoritmos (Algorithms) Class** (`algoritmos.h/cpp`)

- Implements three heuristic algorithms:
  - **Greedy**: Deterministic construction
  - **Greedy Randomized**: Construction with randomness controlled by parameter `α`
  - **Reactive Greedy Randomized**: Adaptive algorithm using multiple `α` values

#### 4. **Utils (Utilities)** (`utils.h/cpp`)

- Random number generation with seed management
- Execution time measurement
- CSV result logging
- Date/time formatting

## Algorithm Details

### Solution Construction Strategy

All three algorithms follow a similar construction pattern:

1. **Initialization**: Start with only the root vertex connected
2. **Iterative Construction**: While there are unconnected vertices:
   - Generate a list of viable candidates (edges that can be added)
   - Select a candidate based on the algorithm strategy
   - Add the selected edge to the solution
3. **Validation**: Check if the solution respects all capacity constraints

### Key Functions

#### Candidate Generation (`gerarCandidatos`)

For each unconnected vertex `v`, the algorithm:

- Considers all already-connected vertices `p` as potential parents
- Checks if edge `(v, p)` exists in the graph
- Verifies if adding `v` as a child of `p` respects the capacity constraint
- Creates a candidate list sorted by edge cost (ascending)

#### Capacity Checking (`podeAdicionar`)

Before adding a vertex `v` as a child of `p`:

- If `p` is the root: Check if `demand(v) ≤ Q` (new subtree)
- If `p` is not the root:
  - Find which subtree `p` belongs to
  - Calculate current demand of that subtree
  - Verify: `current_demand + demand(v) ≤ Q`

#### Subtree Identification (`encontrarSubarvore`)

- Traverses up the tree from a vertex until reaching a direct child of the root
- Each direct child of the root starts a new subtree
- Returns the subtree ID for capacity tracking

### Algorithm 1: Greedy (Deterministic)

**Strategy**: Always select the candidate with the lowest cost.

```cpp
Solucao construirGuloso()
```

**Process**:

1. Generate all viable candidates
2. Sort candidates by cost (ascending)
3. Select the first candidate (lowest cost)
4. Add the edge to the solution
5. Repeat until all vertices are connected

**Time Complexity**: O(n² × m) where `m` is the number of edges checked per iteration

**Characteristics**:

- Fast and deterministic
- Produces the same solution for the same input
- May get trapped in local optima

### Algorithm 2: Greedy Randomized

**Strategy**: Use a **Restricted Candidate List (LRC)** based on parameter `α ∈ [0, 1]`.

```cpp
Solucao construirGulosoRandomizado(double alpha)
```

**Process**:

1. Generate and sort candidates by cost
2. Calculate threshold: `threshold = min_cost + α × (max_cost - min_cost)`
3. Build LRC: all candidates with `cost ≤ threshold`
4. Randomly select one candidate from the LRC
5. Add the selected edge
6. Repeat until all vertices are connected

**Parameter `α`**:

- `α = 0`: Pure greedy (only best candidate)
- `α = 1`: Fully random (all candidates equally likely)
- `α ∈ (0, 1)`: Balance between quality and diversity

**Execution**:

- Runs multiple iterations (default: 30)
- Returns the best solution found across all iterations

### Algorithm 3: Reactive Greedy Randomized

**Strategy**: Adaptively manage multiple `α` values using performance feedback.

```cpp
Solucao executarGulosoReativo(vector<double> alphas, int iteracoes, int tamanhoBloco)
```

**Process**:

1. Initialize uniform probabilities for each `α` value
2. For each iteration:
   - Select an `α` based on current probabilities
   - Construct a solution using that `α`
   - Track the best solution found with each `α`
3. Every `tamanhoBloco` iterations:
   - Calculate quality score for each `α`: `quality = (best_global / best_alpha)²`
   - Update probabilities proportionally to quality
   - Ensure minimum probability (5%) for exploration
   - Renormalize probabilities

**Adaptive Mechanism**:

- Alphas that produce better solutions get higher probabilities
- Maintains exploration through minimum probability guarantees
- Automatically focuses on promising `α` values

**Parameters**:

- `alphas`: List of `α` values to explore (default: [0.1, 0.3, 0.5])
- `iteracoes`: Total number of iterations (default: 300)
- `tamanhoBloco`: Block size for probability updates (default: 30)

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

- **Greedy**: O(n² × m) per solution
- **Randomized**: O(n² × m × iterations)
- **Reactive**: O(n² × m × iterations) with additional overhead for probability updates

### Space Complexity

- O(n²) for the graph adjacency matrix
- O(n) for solution representation

### Optimization Strategies

1. **Early Termination**: Stop if no viable candidates exist
2. **Caching**: Subtree demands are recalculated as needed
3. **Incremental Updates**: Solution validity is tracked incrementally

## Input/Output Format

### Input (OR-Library Format)

```
n Q
<matrix of n×n costs>
<optional: optimal solution value>
```

### Output

- **Terminal**: Solution cost, execution time, deviation from optimal (if known)
- **CSV File**: Detailed execution results with all parameters
- **CS Academy Format**: Solution edges for visualization

## Strengths and Limitations

### Strengths

- ✅ Handles large instances efficiently
- ✅ Provides multiple algorithm variants
- ✅ Adaptive algorithm (reactive) adjusts to problem characteristics
- ✅ Reproducible results through seed control
- ✅ Comprehensive result logging

### Limitations

- ⚠️ Heuristic approach (no optimality guarantee)
- ⚠️ Construction-based (no local search/improvement phase)
- ⚠️ May produce infeasible solutions if capacity constraints are too tight

## Future Improvements

Potential enhancements:

1. **Local Search**: Add improvement phase after construction
2. **Hybrid Approaches**: Combine with exact methods for small subproblems
3. **Advanced Adaptive Strategies**: More sophisticated probability update mechanisms
4. **Parallelization**: Run multiple iterations in parallel
