#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

// ----------------------------------------------------------------------
// DATA STRUCTURES
// ----------------------------------------------------------------------

// Represents an edge in the compressed graph
struct CompressedEdge {
    int target_junction_idx; // The index in the 'junctions' vector
    int original_weight;     // How many nodes are in this segment
    vector<int> path_segment; // The actual sequence of original nodes (excluding start, including end)
};

struct Result {
    long long total_length;
    vector<int> full_path;
};

// ----------------------------------------------------------------------
// CLASS: GRAPH SOLVER
// ----------------------------------------------------------------------

class RingSolver {
    int N; // Original number of nodes
    vector<vector<int>> original_adj; // Original Adjacency List
    
    // Compression Data
    vector<int> junction_map; // Maps compressed_index -> original_node_id
    map<int, int> node_to_junction_id; // Maps original_node_id -> compressed_index
    vector<vector<CompressedEdge>> compressed_adj;
    
    // Solver Data
    vector<bool> visited;
    vector<int> current_junction_path;
    vector<int> best_junction_path;
    long long max_len_found;

public:
    RingSolver(int n) : N(n), original_adj(n) {}

    void addEdge(int u, int v) {
        original_adj[u].push_back(v);
        original_adj[v].push_back(u);
    }

    // ---------------------------------------------------------
    // STEP 1: COMPRESSION LOGIC
    // ---------------------------------------------------------
    void compress() {
        vector<int> junctions;

        // 1. Identify Junctions (Degree != 2)
        for (int i = 0; i < N; ++i) {
            if (original_adj[i].size() != 2) {
                node_to_junction_id[i] = junctions.size();
                junctions.push_back(i);
            }
        }

        // Handle edge case: Pure Ring (No chords, all degree 2)
        if (junctions.empty() && N > 0) {
            node_to_junction_id[0] = 0;
            junctions.push_back(0);
        }

        junction_map = junctions;
        int J = junctions.size();
        compressed_adj.resize(J);

        // 2. Build Compressed Edges by traversing original graph
        for (int i = 0; i < J; ++i) {
            int original_u = junctions[i];

            // Explore all neighbors of this junction in the original graph
            for (int neighbor : original_adj[original_u]) {
                
                // Traversal Variables
                int curr = neighbor;
                int prev = original_u;
                vector<int> segment;
                segment.push_back(curr);
                
                // Traverse the chain until we hit another junction
                while (node_to_junction_id.find(curr) == node_to_junction_id.end()) {
                    int next_node = -1;
                    // Find the next node that isn't the one we came from
                    for (int n : original_adj[curr]) {
                        if (n != prev) {
                            next_node = n;
                            break;
                        }
                    }
                    if (next_node == -1) break; // Dead end (shouldn't happen in ring+chord)
                    
                    prev = curr;
                    curr = next_node;
                    segment.push_back(curr);
                }

                // We reached a destination junction
                int original_v = curr;
                int j_idx_v = node_to_junction_id[original_v];

                // Add to compressed adjacency list
                // Weight is segment.size() because it counts the nodes added
                compressed_adj[i].push_back({j_idx_v, (int)segment.size(), segment});
            }
        }
    }

    // ---------------------------------------------------------
    // STEP 2: DFS SOLVER ON COMPRESSED GRAPH
    // ---------------------------------------------------------
    void dfs(int u, long long current_len) {
        visited[u] = true;
        current_junction_path.push_back(u);

        if (current_len > max_len_found) {
            max_len_found = current_len;
            best_junction_path = current_junction_path;
        }

        for (auto& edge : compressed_adj[u]) {
            if (!visited[edge.target_junction_idx]) {
                dfs(edge.target_junction_idx, current_len + edge.original_weight);
            }
        }

        current_junction_path.pop_back();
        visited[u] = false;
    }

    // ---------------------------------------------------------
    // STEP 3: ORCHESTRATION & EXPANSION
    // ---------------------------------------------------------
    Result solve() {
        // Run compression
        compress();

        int J = compressed_adj.size();
        visited.assign(J, false);
        max_len_found = -1;

        // Run DFS from every junction
        for (int i = 0; i < J; ++i) {
            dfs(i, 0); // Start weight 0 (counting edges/segments) or 1 (counting nodes)? 
                       // Usually length = sum of edge weights. 
        }

        // Reconstruct full path from 'best_junction_path'
        vector<int> full_path;
        if (!best_junction_path.empty()) {
            
            // Add the starting junction
            int start_j = best_junction_path[0];
            full_path.push_back(junction_map[start_j]);

            // Loop through the junction sequence
            for (size_t k = 0; k < best_junction_path.size() - 1; ++k) {
                int u_idx = best_junction_path[k];
                int v_idx = best_junction_path[k+1];

                // Find the specific edge segment used between u and v
                // (There might be multiple edges between two junctions, we need the longest or valid one)
                // In simple compression, we just look for the neighbor match. 
                // Note: For strict correctness if multiple edges exist between same U and V, 
                // the DFS should have tracked exactly which edge index was taken. 
                // For this example, we assume we find the neighbor match.
                
                for (auto& edge : compressed_adj[u_idx]) {
                    if (edge.target_junction_idx == v_idx) {
                        // Append the segment nodes
                        for (int node : edge.path_segment) {
                            full_path.push_back(node);
                        }
                        break; 
                    }
                }
            }
        }

        return {max_len_found, full_path};
    }
};

// ----------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------
int main() {
    // Constructing a "Ring with Chords"
    // Ring: 0-1-2-3-4-5-0
    // Chord: 0-3 (cuts the ring in half)
    // Extra tail to test dead ends or non-ring parts: 2-6
    
    int N, M;
    cin >> N >> M;
    RingSolver graph(N);

    for(int i = 0; i < M; ++i){
        int u, v;
        cin >> u >> v;
        graph.addEdge(u, v);
    }

    Result res = graph.solve();

    cout << "Longest Path Length (Edges): " << res.total_length << endl;
    cout << "Path Sequence: ";
    for (int node : res.full_path) {
        cout << node << " ";
    }
    cout << endl;

    // Expected Logic:
    // Shortest path 0->3 is 1 edge.
    // Longest way around ring 0-1-2-3 is 3 edges.
    // Longest way 0-5-4-3 is 3 edges.
    // But we want Longest Simple Path in the whole graph.
    // Path: 4-5-0-1-2-3 (Length 5)
    
    return 0;
}