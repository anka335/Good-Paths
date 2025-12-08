#include <bits/stdc++.h>
using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

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

void find_branches(int N, vector<vector<int>> &graph, vector<pair<int, int>> &branches, vector<pair<int, int>> &nonbranches) {
    for(int i = 0; i < N; ++i){
        if(graph[i].size() <= 2){
            for(auto neighbor : graph[i]){
                branches.emplace_back(i, neighbor);
                branches.emplace_back(neighbor, i);
            }
        } else {
            for(auto neighbor : graph[i]){
                nonbranches.emplace_back(i, neighbor);
                nonbranches.emplace_back(neighbor, i);
            }
        }
    }
    sort(branches.begin(), branches.end());
    sort(nonbranches.begin(), nonbranches.end());
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

void compress_branches(){

}

void change_graph(int N, int M, vector<vector<int>> &graph, int type){ //compress or change graph based on type
    if(type == 4){
        compress_branches();
    }
    if(type == 5){
        compress_branches();
    }   
}

void find_solution(){ //solve graph based on type

}

int main()
{
    int N, M, total_components, type;
    vector<vector<int>> graph, tree;
    vector<int> which_component;
    vector<pair<int, int>> bridges, branches, non_branches;

    get_input(N, M, graph);
    get_components(N, M, graph, which_component, total_components);
    find_bridges(N, graph, bridges);
    find_branches(N, graph, branches, non_branches);
    type = get_type(N, M, graph, which_component, total_components, bridges);
    change_graph(N, M, graph, type);
    find_solution();
}