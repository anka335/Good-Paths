#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <chrono>

// Constant for max nodes. 
// Using 1024 allows std::bitset to be optimized by the compiler.
const int MAXN = 1024;

// --- Global Data ---
struct Graph {
    int N;
    std::bitset<MAXN> adj[MAXN]; // Adjacency Matrix as bitsets
    std::vector<int> degrees;
};

Graph G;
int global_max_len = 0;

// Timing variables
std::chrono::time_point<std::chrono::steady_clock> start_time;
double time_limit = 19.8; 

// --- Helper: Time Check ---
bool is_time_left() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - start_time;
    return elapsed.count() < time_limit;
}

// --- CORE DFS FUNCTION ---
// u: Current node
// forbidden: Mask of nodes we CANNOT visit next (history constraints)
// current_len: Depth of recursion
void dfs(int u, std::bitset<MAXN> forbidden, int current_len) {
    
    // 1. Update Global Best
    if (current_len > global_max_len) {
        global_max_len = current_len;
    }

    // 2. Identify Candidates
    // A candidate 'v' must be a neighbor of 'u' AND not forbidden.
    std::bitset<MAXN> candidates = G.adj[u] & ~forbidden;
    
    if (candidates.none()) return; // Dead end

    // 3. Heuristic Scoring (Lookahead)
    // We want to pick a neighbor 'v' that maximizes future possibilities.
    std::vector<std::pair<int, int>> moves;
    moves.reserve(G.N); // Avoid reallocations

    // Calculate the "Next Forbidden" mask just once.
    // If we pick ANY v, the next step's constraint will include neighbors of u.
    std::bitset<MAXN> next_base_forbidden = forbidden | G.adj[u];
    // Also mark u itself as forbidden (implicitly handled by adj[u] usually, but good to be explicit)
    next_base_forbidden[u] = 1; 

    for (int v = 0; v < G.N; ++v) {
        if (candidates[v]) {
            // LOOKAHEAD:
            // If we move to v, how many valid neighbors will v have?
            // Valid neighbors of v = adj[v] AND NOT (next_base_forbidden)
            int score = (G.adj[v] & ~next_base_forbidden).count();
            
            moves.push_back({score, v});
        }
    }

    // 4. Sort Candidates (Greedy descending)
    std::sort(moves.begin(), moves.end(), [](const std::pair<int,int>& a, const std::pair<int,int>& b){
        return a.first > b.first; 
    });

    // 5. Recursive Backtracking
    // To stay within 20s, we limit the branching factor (Beam Search style within DFS).
    // We only explore the top K best moves.
    int branching_limit = 5; 
    
    // If we are very deep, we can narrow the beam to save time
    if (current_len > 40) branching_limit = 3;

    for (int i = 0; i < std::min((int)moves.size(), branching_limit); ++i) {
        int v = moves[i].second;

        // Constraint check: Stop if we ran out of time
        // Checking every single node is slow, check periodically
        if ((i == 0 || current_len % 10 == 0) && !is_time_left()) return;

        // Recurse
        // Pass 'next_base_forbidden' which we calculated earlier: 
        // It contains Old_Forbidden + Neighbors_of_U.
        // We also need to mark 'v' itself as visited? 
        // Actually, 'v' becomes the new 'u', so it's handled in the next level logic.
        // But for strictness, ensure v cannot be revisited.
        
        std::bitset<MAXN> next_forbidden_for_v = next_base_forbidden;
        next_forbidden_for_v[v] = 1; 

        dfs(v, next_forbidden_for_v, current_len + 1);
        
        // Backtrack is implicit (variables go out of scope)
    }
}

// --- Main ---

int main() {
    // Fast I/O
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int M;
    if (!(std::cin >> G.N >> M)) return 0;

    // Input Reading & Graph Building
    for(int i = 0; i < M; ++i){
        int u, v;
        std::cin >> u >> v;
        u--; v--; // 0-based
        if(u >= 0 && u < G.N && v >= 0 && v < G.N) {
            G.adj[u][v] = 1;
            G.adj[v][u] = 1;
        }
    }
    
    // Pre-calculate degrees for smart starting
    G.degrees.resize(G.N);
    std::vector<int> start_nodes(G.N);
    for(int i=0; i<G.N; ++i) {
        G.degrees[i] = G.adj[i].count();
        start_nodes[i] = i;
    }

    // Sort start nodes: Low degree nodes are MUCH better starts in dense graphs
    std::sort(start_nodes.begin(), start_nodes.end(), [&](int a, int b){
        return G.degrees[a] < G.degrees[b];
    });

    start_time = std::chrono::steady_clock::now();

    // --- RESTART LOOP ---
    // Try starting from every node (best ones first)
    for (int u : start_nodes) {
        if (!is_time_left()) break;

        // Initial setup
        std::bitset<MAXN> forbidden;
        forbidden[u] = 1; // Cannot return to start immediately

        dfs(u, forbidden, 1);
    }

    std::cout << global_max_len << "\n";

    return 0;
}