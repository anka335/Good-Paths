#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <cstring> // For memset

// --- 1. Data Structures ---

struct Graph {
    int num_nodes;
    std::vector<std::vector<int>> adj;
    std::vector<int> degrees;
};

struct Solution {
    std::vector<int> path;
    
    // Using raw pointers or vectors. Vectors are safer, but we use fill/memset for speed.
    std::vector<int> in_path;      
    std::vector<int> adj_count;     

    void init(int n) {
        path.reserve(n);
        in_path.resize(n, 0);
        adj_count.resize(n, 0);
    }

    // OPTIMIZED CLEAR: 
    // Instead of undoing edges (slow for dense graphs), we just wipe the memory.
    // This is valid because N=1000 is small.
    void clear(int n) {
        path.clear();
        
        //std::fill is cleaner C++, usually compiles to memset for integers
        std::fill(in_path.begin(), in_path.end(), 0);
        std::fill(adj_count.begin(), adj_count.end(), 0);
    }
};

// --- 2. Helper Functions ---

// Add node and update neighbors
void push_node(Solution& s, int node, const Graph& g) {
    s.path.push_back(node);
    s.in_path[node] = 1; // Mark as true
    for (int neighbor : g.adj[node]) {
        s.adj_count[neighbor]++;
    }
}

// Remove node (Backtracking only)
void pop_node(Solution& s, const Graph& g) {
    if (s.path.empty()) return;
    int node = s.path.back();
    s.path.pop_back();
    s.in_path[node] = 0;
    for (int neighbor : g.adj[node]) {
        s.adj_count[neighbor]--;
    }
}

// --- 3. Mutation Logic (Greedy Low-Degree) ---

bool greedy_grow(Solution& s, const Graph& g) {
    int tail = s.path.back();
    
    int best_node = -1;
    int min_degree = 10000000;

    // Fast Scan
    for (int neighbor : g.adj[tail]) {
        // Valid Check: Not in path AND only connected to tail
        if (s.in_path[neighbor] == 0 && s.adj_count[neighbor] == 1) {
            
            // HEURISTIC: Low Degree is better
            int score = g.degrees[neighbor]; 
            
            // Instant take optimization
            if (score <= 2) {
                push_node(s, neighbor, g);
                return true;
            }

            if (score < min_degree) {
                min_degree = score;
                best_node = neighbor;
            } 
            else if (score == min_degree) {
                // Tie-break randomization
                if (rand() % 2 == 0) best_node = neighbor;
            }
        }
    }

    if (best_node != -1) {
        push_node(s, best_node, g);
        return true; 
    }
    return false; // Stuck
}

// --- 4. Main Program ---

int main() {
    // 1. Fast I/O
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    srand(time(NULL));

    // 2. Input
    int N, M;
    if (!(std::cin >> N >> M)) return 0;

    Graph g;
    g.num_nodes = N;
    g.adj.resize(N);
    g.degrees.resize(N);

    for(int i = 0; i < M; ++i){
        int u, v;
        std::cin >> u >> v;
        u--; v--; // 1-based -> 0-based
        if (u >= 0 && u < N && v >= 0 && v < N) {
            g.adj[u].push_back(v);
            g.adj[v].push_back(u);
        }
    }

    // Pre-calc degrees
    for(int i=0; i<N; ++i) g.degrees[i] = g.adj[i].size();

    // 3. Setup Candidates (Low degree bias)
    std::vector<int> start_candidates(N);
    for(int i=0; i<N; ++i) start_candidates[i] = i;
    
    // Sort so lowest degree nodes are first
    std::sort(start_candidates.begin(), start_candidates.end(), 
        [&](int a, int b){ return g.degrees[a] < g.degrees[b]; });

    Solution sol;
    sol.init(N);

    int global_max_len = 0;
    
    // 4. Timer Setup
    auto start_time = std::chrono::steady_clock::now();
    
    // Stop exactly just before 20s
    double time_limit = 19.8; 
    
    long long restarts = 0;

    // --- RESTART LOOP ---
    while (true) {
        restarts++;

        // Check time every 50 restarts (reduced frequency for speed)
        // Since clear() is now fast, restarts are very frequent.
        if (restarts % 50 == 0) {
            auto current_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = current_time - start_time;
            if (elapsed.count() > time_limit) break;
        }

        // RESET (Fast O(N) wipe)
        sol.clear(N);

        // Pick Start Node
        // 90% chance: pick from top 10% best candidates
        int idx;
        if ((rand() % 100) < 90) {
             int range = std::max(1, N / 10); 
             idx = rand() % range;
        } else {
             idx = rand() % N; 
        }
        
        push_node(sol, start_candidates[idx], g);

        // Greedy Growth
        while(true) {
            bool grew = greedy_grow(sol, g);
            
            if (!grew) {
                // If stuck, just break and restart. 
                // In 1000-node dense graphs, backtracking is less efficient 
                // than just trying a new random start.
                break;
            }
        }

        // Update Best
        if ((int)sol.path.size() > global_max_len) {
            global_max_len = sol.path.size();
        }
    }

    // Output Result
    std::cout << global_max_len << "\n";

    return 0;
}