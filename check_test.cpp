#include <bits/stdc++.h>
using namespace std;


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

void DFS_iterative(int start, int v, vector<bool> &visited, vector<int> &which_component, vector<vector<int>> &graph, int count){
    stack<int> s;
    s.push(v);
    visited[v] = true;
    which_component[v] = start;
    while(!s.empty()){
        int u = s.top();
        count++;
        s.pop();
        for(auto neighbor : graph[u]){
            if(!visited[neighbor]){
                visited[neighbor] = true;
                which_component[neighbor] = start;
                s.push(neighbor);
            }
        }
    }
    //if(count > 40)
     //   cout << "Component " << start << " has " << count << " nodes.\n";
}

void get_components(int &N, int &M, vector<vector<int>> &graph, vector<int> &which_component, int& total_components){
    vector<bool> visited(N, false);
    which_component.assign(N, -1);
    int start=0;
    for(int i = 0; i < N; ++i){
        if(!visited[i]){
            DFS_iterative(start, i, visited, which_component, graph, 0);
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

bool shortest_path(int start_node, int how_long, int N, vector<vector<int>>& graph) {
    vector<int> distance(N, -1);
    vector<int> parent(N, -1);
    queue<int> q;

    distance[start_node] = 0;
    q.push(start_node);

    int path_end_node = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        path_end_node = u;

        if (distance[u] >= how_long) {
            continue;
        }

        for (int v : graph[u]) {
            if (distance[v] == -1) {
                distance[v] = distance[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }

        // Path found, reconstruct and print
        vector<int> path;
        int current = path_end_node;
        while (current != -1) {
            path.push_back(current);
            current = parent[current];
        }
        reverse(path.begin(), path.end());

        if (path.size() >= how_long) {
            cout << how_long << endl;
            for (size_t i = 0; i < how_long; ++i) {
                cout << path[i] << (i == path.size() - 1 ? "" : " ");
            }
            cout << endl;
            return true;
        }
    return false;
}

int get_type(int N, int M, vector<vector<int>> &graph, vector<int> &which_component, int total_components, vector<pair<int, int>> &bridges){
    if(total_components > 1){
        return 6;
    }
    double avg_degree = (2.0 * M ) / N;
    avg_degree += 0.5;
    cout << "Average Degree: " << avg_degree << "\n";
    if(avg_degree > 20){
        return 1;
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
    if(avg_degree >= 9){
        return 2;
    }
        return 3;
}

void check_if_cactus(int N, int M, vector<vector<int>> &graph){
    if (N > 0 && M == 0 && N > 1) {
        cout << "The graph is not a cactus graph (it's not connected)." << endl;
        return;
    }
    if (N <= 1) {
        cout << "The graph is a cactus graph." << endl;
        return;
    }
    
    vector<int> which_component;
    int total_components;
    get_components(N, M, graph, which_component, total_components);
    if (total_components > 1) {
        cout << "The graph is not a cactus graph (it's not connected)." << endl;
        return;
    }

    auto analyze_bcc = 
        [&](const vector<pair<int, int>>& bcc_edges_popped, const vector<vector<int>>& adj) -> bool {
        
        unordered_set<int> bcc_vertices;
        for(const auto& e : bcc_edges_popped) {
            bcc_vertices.insert(e.first);
            bcc_vertices.insert(e.second);
        }

        size_t num_vertices = bcc_vertices.size();
        size_t num_edges = 0;
        for (int vert_u : bcc_vertices) {
            for (int vert_v : adj[vert_u]) {
                if (bcc_vertices.count(vert_v)) {
                    num_edges++;
                }
            }
        }
        num_edges /= 2;

        if (num_edges == 0) return true;
        if (num_edges == 1) return true; // Bridge

        if (num_vertices != num_edges) return false;
        
        for(int u : bcc_vertices){
            int degree_in_bcc = 0;
            for(int v : adj[u]){
                if(bcc_vertices.count(v)) degree_in_bcc++;
            }
            if(degree_in_bcc != 2) return false;
        }
        return true;
    };

    vector<int> tin(N, -1), low(N, -1);
    stack<pair<int, int>> edge_stack;
    int timer = 0;
    bool is_cactus = true;

    for (int i = 0; i < N; ++i) {
        if (tin[i] != -1) continue;

        stack<tuple<int, int, vector<int>::iterator>> s;
        s.emplace(i, -1, graph[i].begin());
        tin[i] = low[i] = timer++;

        while (!s.empty()) {
            auto& [u, p, it] = s.top();

            if (it == graph[u].end()) {
                s.pop();
                if (!s.empty()) {
                    auto& [parent_u, parent_p, parent_it] = s.top();
                    low[parent_u] = min(low[parent_u], low[u]);
                    if (low[u] >= tin[parent_u]) {
                        vector<pair<int, int>> popped;
                        while(!edge_stack.empty()) {
                            pair<int, int> edge = edge_stack.top();
                            popped.push_back(edge);
                            edge_stack.pop();
                            if(edge.first == parent_u && edge.second == u) break;
                        }
                        if (!analyze_bcc(popped, graph)) {
                            is_cactus = false;
                        }
                    }
                }
                 if (!is_cactus) break;
                continue;
            }

            int v = *it;
            ++it;

            if (v == p) continue;

            if (tin[v] != -1) {
                if (tin[v] < tin[u]) {
                    low[u] = min(low[u], tin[v]);
                    edge_stack.push({u, v});
                }
            } else {
                tin[v] = low[v] = timer++;
                edge_stack.push({u, v});
                s.emplace(v, u, graph[v].begin());
            }
        }
        if (!is_cactus) break;
    }

    if (is_cactus && !edge_stack.empty()) {
        vector<pair<int, int>> popped;
        while(!edge_stack.empty()) { 
            popped.push_back(edge_stack.top()); 
            edge_stack.pop();
        }
        if (!analyze_bcc(popped, graph)) {
            is_cactus = false;
        }
    }

    if (is_cactus) {
        cout << "The graph is a cactus graph." << endl;
    } else {
        cout << "The graph is not a cactus graph." << endl;
    }
}

//podmianka 6 na 1 i 1 na 6

int main()
{
    int N, M, total_components;
    vector<vector<int>> graph, tree;
    vector<int> which_component;
    vector<pair<int, int>> bridges;

    get_input(N, M, graph);
    get_components(N, M, graph, which_component, total_components);
    find_bridges(N, graph, bridges);

    //BRIDGES PERCENTAGE CHECK
    /*double percentage = (double)bridges.size()/(double)(M*2) * 100.0;
    if(percentage < 0.1){
        cout << "1\n" << 0;
    } else if(percentage < 5){
        cout << "2\n";
        cout << 0 << " " << graph[0][0] << '\n';
    } else if(percentage < 50){
        int i = 1;
        while(!shortest_path(0, i, 3, N, graph)){
            i++;
        }
    } else {
        int i = 1;
        while(!shortest_path(0, i, 4, N, graph)){
            i++;
        }
    }*/

    //TOTAL COMPONENTS CHECK
    //cout << "Total Components: " << total_components << "\n";
    /*int i = 1;
    while(!shortest_path(0, i, total_components, N, graph) and i < N){
        i++;
    }
    if(i == N){
        cout << "7\n";
        while(!shortest_path(0, i, 7, N, graph) and i < N){
            i++;
        }
    }*/

    //AVERAGE DEGREE CHECK
    /*double avg_degree = (2.0 * M ) / N;
    cout << "Average Degree: " << avg_degree << "\n";
    avg_degree += 0.5; //rounding
    bool ok = false;
    
    for(int i = 0; i < N; ++i){
        if(shortest_path(i, avg_degree, N, graph)){
            ok = true;
            break;
        }
    }
    if(!ok){
        for(int i = 0; i < N; ++i){
        if(shortest_path(i, 3, N, graph)){
            ok = true;
            break;
            }
        }
    }
    if(!ok){
        cout << -1;
    }*/

    //graph type 5 - what percentage of bridges
    /*double percentage = (double)bridges.size()/(double)(M*2) * 100.0;
    if(percentage < 50){
        cout << "1\n" << 0;
    } else {
        if(percentage < 75){
            cout << "2\n";
            cout << 0 << " " << graph[0][0] << '\n';
        } else if (percentage < 85){
            int i = 1;
            while(!shortest_path(0, 3, N, graph)){
                i++;
            }
        } else if (percentage < 95){
            int i = 1;
            while(!shortest_path(0, 4, N, graph)){
                i++;
            }
        } else {
            int i = 1;
            while(!shortest_path(0, 5, N, graph)){
                i++;
            }
        }
    }*/

    //graph type 4 - ring like
    /*int degrees_over_2 = 0;
    for(int i = 0; i < N; ++i){
        if(graph[i].size() > 2){
            degrees_over_2++;
        }
    }
    cout << (double)degrees_over_2/(double)N * 100 << '\n';*/
    
    //graph type 1 - a lot of components
    /*int num_components = N/total_components;
    if(num_components < 100){
        cout << "1\n" << 0;
    } else if(num_components < 500){
        cout << "2\n";
        cout << 0 << " " << graph[0][0] << '\n';
    } else if(num_components < 1000){
        int i = 1;
        while(!shortest_path(0, 3, N, graph)){
            i++;
        }
    } else {
        int i = 1;
        while(!shortest_path(0, 4, N, graph)){
            i++;
        }
    }*/

    //graph type 2 - ???
    /*double avg_degree = (2.0 * M ) / N;
    //cout << "Average Degree: " << avg_degree << "\n";
    avg_degree += 0.5; //rounding
    bool ok = false;
    avg_degree /= double(N-1);

    if(avg_degree < 0.1){
        cout << "1\n" << 0;
    } else if(avg_degree < 0.3){
        cout << "2\n";
        cout << 0 << " " << graph[0][0] << '\n';
    } else if(avg_degree < 0.5){
        int i = 1;
        while(!shortest_path(0, 3, N, graph)){
            i++;
        }
    } else {
        int i = 1;
        while(!shortest_path(0, 4, N, graph)){
            i++;
        }
    }*/

   //GET TYPE
    int type = get_type(N, M, graph, which_component, total_components, bridges);
    //cout << type << '\n';
    //int i = 1;
    //while(!shortest_path(0, type, N, graph)){
    //    i++;
    //}

    //CHECK IF CACTUS
    check_if_cactus(N, M, graph);
}