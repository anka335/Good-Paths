#include <bits/stdc++.h>
using namespace std;


struct CompNode{
    int first_nonbranch, last_nonbranch, size, id;
    vector<int> path;
};

void get_input(int &N, int &M, vector<vector<int>> &graph){
    cin >> N >> M;
    graph.assign(N, vector<int>());
    for(int i = 0; i < M; ++i){
        int u, v;
        cin >> u >> v;
        graph[u].emplace_back(v);
        graph[v].emplace_back(u);
    }
}

void DFS_iterative(int start, int v, vector<bool> &visited, vector<int> &which_component, vector<vector<int>> &graph){
    stack<int> s;
    s.push(v);
    visited[v] = true;
    which_component[v] = start;
    while(!s.empty()){
        int u = s.top();
        s.pop();
        for(auto neighbor : graph[u]){
            if(!visited[neighbor]){
                visited[neighbor] = true;
                which_component[neighbor] = start;
                s.push(neighbor);
            }
        }
    }
}

void get_components(int &N, int &M, vector<vector<int>> &graph, vector<int> &which_component, int& total_components){
    vector<bool> visited(N, false);
    which_component.assign(N, -1);
    int start=0;
    for(int i = 0; i < N; ++i){
        if(!visited[i]){
            DFS_iterative(start, i, visited, which_component, graph);
            start++;
        }
    }
    total_components = start;
    //cout << "Total Components: " << total_components << "\n";
}

void print_bridges(vector<pair<int, int>> &bridges){
    cout << "Bridges in the graph:\n";
    for(const auto &bridge : bridges){
        cout << bridge.first << " " << bridge.second << "\n";
    }
}

void find_bridges(int N, vector<vector<int>> &graph, vector<pair<int, int>> &bridges) {
    vector<int> tin(N, -1), low(N, -1);
    vector<bool> visited(N, false);
    int timer = 0;

    for (int i = 0; i < N; ++i) {
        if (!visited[i]) {
            stack<tuple<int, int, vector<int>::iterator>> s;
            s.emplace(i, -1, graph[i].begin());
            visited[i] = true;
            tin[i] = low[i] = timer++;

            while (!s.empty()) {
                auto& [v, p, it] = s.top();

                if (it == graph[v].end()) {
                    s.pop();
                    if (p != -1) {
                        low[p] = min(low[p], low[v]);
                        if (low[v] > tin[p]) {
                            //cout << p << " " << v << " is a bridge\n";
                            bridges.emplace_back(p, v);
                            bridges.emplace_back(v, p);
                        }
                    }
                    continue;
                }

                int to = *it;
                ++it;

                if (to == p) continue;

                if (visited[to]) {
                    low[v] = min(low[v], tin[to]);
                } else {
                    visited[to] = true;
                    tin[to] = low[to] = timer++;
                    s.emplace(to, v, graph[to].begin());
                }
            }
        }
    }
    sort(bridges.begin(), bridges.end());
    //print_bridges(bridges);
}

void find_branches(int N, vector<vector<int>> &graph, vector<bool> &branches) {
    branches.assign(N, false);
    for(int i = 0; i < N; ++i){
        if(graph[i].size() <= 2) branches[i] = true;
    }
}

int get_type(int N, int M, vector<vector<int>> &graph, vector<int> &which_component, int total_components, vector<pair<int, int>> &bridges){
    if(total_components > 1){
        return 1;
    }
    double avg_degree = (2.0 * M ) / N;
    avg_degree += 0.5;
    //cout << "Average Degree: " << avg_degree << "\n";
    if(avg_degree > 20){
        return 6;
    }
    if(avg_degree > 11){
        return 7;
    }
    double percentage_bridges = (double)bridges.size()/(double)(M*2) * 100.0;
    if(percentage_bridges > 50){
        return 5;
    }
    if(avg_degree < 3){
        return 4;
    }
        return 3;
}

void print_path(vector<int> &path){
    cout << path.size() << "\n";
    for(auto node : path){
        cout << node << " ";
    }
}

void BFS(int start, int N, vector<vector<int>> &graph, vector<int> &distance, vector<int> &parent){
    distance.assign(N, -1);
    parent.assign(N, -1);
    queue<int> q;
    q.push(start);
    distance[start] = 0;
    int path_end_node = -1;

    while(!q.empty()){
        int u = q.front();
        q.pop();

        for(auto neighbor : graph[u]){
            if(distance[neighbor] == -1){
                distance[neighbor] = distance[u] + 1;
                parent[neighbor] = u;
                q.push(neighbor);
            }
        }
    }
}

void get_path(int last_node, vector<int> &parent, vector<int> &path){
    path.clear();
    int current = last_node;
    while(current != -1){
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());
    //print_path(path);
}

void get_furthest(int N, int M, vector<vector<int>> &graph, vector<int> &path, int &leaf_before, int& next_leaf, bool last = false){
    vector<int> distance, parent;
    BFS(leaf_before, N, graph, distance, parent);
    int max_dist = 0;
    for(int i = 0; i < N; ++i){
        if(distance[i] > max_dist){
            max_dist = distance[i];
            next_leaf = i;
        }
    }
    if(last){
        get_path(next_leaf, parent, path);
    }
}

void get_diameter(int N, int M, vector<vector<int>> &graph, vector<int> &path){ //path should be empty by default
    int first_leaf = -1, second_leaf = -1, third_leaf = -1;
    for(int i = 0; i < N; ++i){
        if(graph[i].size() == 1){ //leaf
            first_leaf = i;
            break;
        }
    }
    get_furthest(N, M, graph, path, first_leaf, second_leaf);
    get_furthest(N, M, graph, path, second_leaf, third_leaf, 1);
}

void get_comp_nodes(int v, vector<bool> &visited, vector<vector<int>> &graph, vector<bool> &branches, int &N_new, vector<CompNode> &comp_nodes, vector<int> &which_comp){
    int count_ends = 0;
    for(auto n : graph[v]){
        if(visited[n]) continue;
        int last_node = -1;
        vector<int> path;
        stack<int> s;
        s.push(n);
        visited[n] = true;
        visited[v] = true;
        while(!s.empty()){
            int u = s.top();
            s.pop();
            if(!branches[u]){
                last_node = u;
                break;
            }
            path.emplace_back(u);
            for(auto neighbor : graph[u]){
                if(!visited[neighbor]){
                    visited[neighbor] = true;
                    s.push(neighbor);
                }
            }
            if(s.empty() && graph[u].size() > 1){
                if(!branches[graph[u][0]] && graph[u][0] != v){
                    last_node = graph[u][0];
                }
                else if(!branches[graph[u][1]] && graph[u][1] != v){
                    last_node = graph[u][1];
                }
                else {
                    last_node = v;
                }
            }
        }
        /*cout << "PATH:\n";
        for(auto p : path){
            cout << p << " ";
        }
        cout << "with first node: " << v << " and with last node: " << last_node << '\n';
        if(last_node == -1) count_ends++;*/
        if(path.size() > 0){
            N_new++;
            CompNode comp_node;
            comp_node.first_nonbranch = v;
            comp_node.last_nonbranch = last_node;
            comp_node.path = path;
            comp_node.size = path.size();
            comp_node.id = N_new;
            comp_nodes.emplace_back(comp_node);
        }
        for(auto p : path){
            which_comp[p] = N_new;
        }
    }
}

void get_weights(int N_new, vector<CompNode> &comp_nodes, vector<int> &weights){
    weights.assign(N_new+1, 1);
    for(auto c : comp_nodes)
        weights[c.id] = c.size;
}

void print_comp_graph(int N_new, vector<vector<int>> &comp_graph, vector<int> &weights){
    for(int i = 0; i <= N_new; ++i){
        if(comp_graph[i].size() == 0) continue;
        cout << i << ": ";
        for(auto n : comp_graph[i]){
            cout << n << " ";
        }
        cout << "  with weight " << weights[i];
        cout << '\n';
    }
}

void get_comp_graph(int N, int N_new, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &which_comp){
    comp_graph.assign(N_new+1, vector<int>());
    for(int i = 0; i < N; ++i){
        int id_v = which_comp[i];
        for(auto neighbor : graph[i]){
            int id_u = which_comp[neighbor];
            if(id_v == -1 && id_u == -1)
                comp_graph[i].push_back(neighbor);
            else if(id_v == -1)
                comp_graph[i].push_back(id_u);
            else if(id_u == -1)
                comp_graph[id_v].push_back(neighbor);
        }
    }
}

void compress_branches(int N, int M, vector<vector<int>> &graph, vector<bool> &branches, vector<vector<int>> &comp_graph, vector<int> &weights, vector<CompNode> &comp_nodes, vector<int> &which_comp){
    int N_new = N;
    vector<bool> visited(N, false);
    which_comp.assign(N, -1);
    for(int i = 0; i < N; ++i){
        if(!branches[i]) get_comp_nodes(i, visited, graph, branches, N_new, comp_nodes, which_comp);
    }

    get_weights(N_new, comp_nodes, weights);
    get_comp_graph(N, N_new, graph, comp_graph, which_comp);  
    
    print_comp_graph(N_new, comp_graph, weights);
}

void no_compress_change(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights){
    comp_graph = graph;
    weights.assign(N, 1);
}

void change_graph(int N, int M, vector<vector<int>> &graph, vector<bool> &branches, vector<int> &which_comp, vector<CompNode> &comp_nodes, vector<int> &weights, vector<vector<int>> &comp_graph, int type){ //compress or change graph based on type

    if(type == 1){
        no_compress_change(N, graph, comp_graph, weights);
    }
    if(type == 2){
        no_compress_change(N, graph, comp_graph, weights);
    }
    if(type == 3){
        no_compress_change(N, graph, comp_graph, weights);
    }
    if(type == 4){
        compress_branches(N, M, graph, branches, comp_graph, weights, comp_nodes, which_comp);
    }
    if(type == 5){
        compress_branches(N, M, graph, branches, comp_graph, weights, comp_nodes, which_comp);
    }
    if(type == 6){
        no_compress_change(N, graph, comp_graph, weights);
    }
    if(type == 7){
        no_compress_change(N, graph, comp_graph, weights);
    }
}

void print_final_path(vector<int> &final_path){
    cout << final_path.size() << "\n";
    for(auto node : final_path){
        cout << node << " ";
    }
    cout << '\n';
}

void decompress_branches(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &comp_path, vector<int> &final_path, vector<int> &which_comp, vector<CompNode> &comp_nodes){
    for(int i = 0; i < comp_path.size(); ++i){
        int v = comp_path[i];
        if(v < N && which_comp[v] == -1){
            final_path.emplace_back(v);
            continue;
        }
        int id = v-N-1;
        //cout << "id: " << id <<  " comp nodes size: " << comp_nodes.size() <<'\n';
        CompNode comp_node = comp_nodes[id];
        vector<int> path = comp_node.path;
        if(i == 0 && comp_node.last_nonbranch != comp_path[i+1]) reverse(path.begin(), path.end());
        else if(comp_node.first_nonbranch != comp_path[i-1]) reverse(path.begin(), path.end());
        for(auto p : path){
            final_path.emplace_back(p);
        }
    }
}

void find_solution(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &final_path, vector<int> &which_comp, vector<CompNode> &comp_nodes){ //solve graph based on type
    vector<int> comp_path; //do uzupelnienia przez local search
    decompress_branches(N, graph, comp_graph, weights, comp_path, final_path, which_comp, comp_nodes);
}

int main()
{
    int N, M, total_components, type;
    vector<vector<int>> graph, comp_graph;
    vector<int> which_component, which_comp, weights, final_path;
    vector<pair<int, int>> bridges;
    vector<bool> branches; //branch - a node that has at most 2 neighbours
    vector<CompNode> comp_nodes;

    get_input(N, M, graph);
    get_components(N, M, graph, which_component, total_components);
    find_bridges(N, graph, bridges);
    find_branches(N, graph, branches);
    type = get_type(N, M, graph, which_component, total_components, bridges);
    change_graph(N, M, graph, branches, which_comp, comp_nodes, weights, comp_graph, type);
    find_solution(N, graph, comp_graph, weights, final_path, which_comp, comp_nodes);
    print_final_path(final_path);
}