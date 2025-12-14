#include <bits/stdc++.h>
using namespace std;
constexpr int INF = 2e9;
mt19937 rng(2138);

struct CompNode{
    int first_nonbranch, last_nonbranch, weight, id;
    list<int> path;
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
int largest_component(vector<int>  &which_component)
{
    map<int, int> count_component;
    int best_component = 0;
    for(auto component_id : which_component)
    {
        if(++count_component[component_id] > count_component[best_component])
            best_component = component_id;
    }
    // cerr << count_component[best_component] << '\n';
    // for(int v = 0; v < which_component.size(); ++v)
    //     if(which_component[v] == best_component)
    //         cerr << v << ' ';
    // cerr << '\n';
    return best_component;
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

void dijkstra(int start, vector<vector<int>> &comp_graph, vector<int> &distance, vector<int> &weights, vector<int> &parent){
    //notice that dijkstra works the same as BFS for unweighted graph
    distance.assign(comp_graph.size(), INF);
    parent.assign(comp_graph.size(), -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    //cerr << start << ' ' << weights.size() << '\n';
    pq.push({weights[start], start});
    distance[start] = weights[start];

    while(!pq.empty()){
        auto [dist, u] = pq.top();
        pq.pop();
        if(dist > distance[u])
            continue;
        for(auto neighbor : comp_graph[u]){
            if(distance[u] + weights[neighbor] < distance[neighbor]){
                distance[neighbor] = distance[u] + weights[u];
                parent[neighbor] = u;
                pq.push({distance[neighbor], neighbor});
            }
        }
    }
}

void get_path(int last_node, vector<int> &parent, list<int> &path){
    path.clear();
    int current = last_node;
    while(current != -1){
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());
    //print_path(path);
}

int get_furthest(vector<vector<int>> &comp_graph, list<int> &path, vector<int> &weights, int &leaf_before, int& next_leaf, bool last = false){
    vector<int> distance, parent;
    dijkstra(leaf_before, comp_graph, distance, weights, parent);
    int max_dist = 0;
    for(int i = 0; i < comp_graph.size(); ++i){
        //cerr << i << ": " << distance[i] << '\n';
        if(distance[i] != INF && distance[i] > max_dist){
            max_dist = distance[i];
            next_leaf = i;
        }
    }
    if(last){
        get_path(next_leaf, parent, path);
    }
    return max_dist;
}

int get_diameter(vector<vector<int>> &comp_graph, list<int> &path, vector<int> &weights){ //path should be empty by default
    int first_leaf = -1, second_leaf = -1, third_leaf = -1;
    for(int i = 0; i < comp_graph.size(); ++i){
        if(!comp_graph[i].empty())
            first_leaf = i;
        if(comp_graph[i].size() == 1){ //leaf
            first_leaf = i;
            break;
        }
    }
    //cerr << first_leaf << ' ';
    get_furthest(comp_graph, path, weights, first_leaf, second_leaf);
    //cerr << second_leaf << ' ';
    return get_furthest(comp_graph, path, weights, second_leaf, third_leaf, 1);
    //cerr << third_leaf << '\n';
}

void get_comp_nodes(int v, vector<bool> &visited, vector<vector<int>> &graph, vector<bool> &branches, int &N_new, vector<CompNode> &comp_nodes, vector<int> &which_comp){
    int count_ends = 0;
    for(auto n : graph[v]){
        if(visited[n]) continue;
        int last_node = -1;
        list<int> path;
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
        if(path.size() > 3){
            CompNode comp_node;
            comp_node.first_nonbranch = path.front();
            comp_node.last_nonbranch = path.back();
            path.pop_back();
            path.pop_front();
            comp_node.path = path;
            comp_node.weight = path.size();
            comp_node.id = N_new;
            comp_nodes.emplace_back(comp_node);
            for(auto p : path){
                which_comp[p] = N_new;
            } 
            N_new++;
        }
    }
}

void get_weights(int N_new, vector<CompNode> &comp_nodes, vector<int> &weights){
    weights.assign(N_new, 1);
    for(auto c : comp_nodes)
        weights[c.id] = c.weight;
}

void get_comp_stats(vector<int> &weights){
    int sum = 0;
    for(auto w : weights) if(w != 1) sum+=w;
    cout << "SUM OF ALL WEIGHTS DIFFERENT FROM 1 (sum of compressed nodes) IS EQUAL " << sum << '\n';
}

void print_comp_graph(int N_new, vector<vector<int>> &comp_graph, vector<int> &weights){
    for(int i = 0; i < N_new; ++i){
        if(comp_graph[i].size() == 0) continue;
        cout << i << ": ";
        for(auto n : comp_graph[i]){
            cout << n << " ";
        }
        cout << "  with weight " << weights[i];
        cout << '\n';
    }
    get_comp_stats(weights);
}

void get_comp_graph(int N, int N_new, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &which_comp, vector<int> &which_component, int best_component){
    comp_graph.assign(N_new, vector<int>());
    for(int i = 0; i < N; ++i)
        if(which_component[i] == best_component)
        {
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

void compress_branches(int N, vector<vector<int>> &graph, vector<bool> &branches, vector<vector<int>> &comp_graph, vector<int> &weights, vector<CompNode> &comp_nodes, vector<int> &which_comp, vector<int> &which_component, int best_component){
    int N_new = N+1;
    vector<bool> visited(N, false);
    which_comp.assign(N, -1);
    for(int i = 0; i < N; ++i){
        if(!branches[i]) get_comp_nodes(i, visited, graph, branches, N_new, comp_nodes, which_comp);
    }

    get_weights(N_new, comp_nodes, weights);
    get_comp_graph(N, N_new, graph, comp_graph, which_comp, which_component, best_component);  
    
    print_comp_graph(N_new, comp_graph, weights);
}

void no_compress_change(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &which_component, int best_component, vector<int> &weights){
    comp_graph.assign(graph.size(), vector<int>());
    for(int v = 0; v < graph.size(); ++v)
        if(which_component[v] == best_component)
        {
            comp_graph[v] = graph[v];
        }
    weights.assign(N, 1);
    //print_comp_graph(N, comp_graph, weights);
}

void change_graph(int N, int M, vector<vector<int>> &graph, vector<bool> &branches, vector<int> &which_comp, vector<CompNode> &comp_nodes, vector<int> &weights, vector<vector<int>> &comp_graph, vector<int> &which_component, int best_component, int type){ //compress or change graph based on type

    if(type == 1){
        no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
    }
    if(type == 2){
        no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
    }
    if(type == 3){
        no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
    }
    if(type == 4){
        //no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
        compress_branches(N, graph, branches, comp_graph, weights, comp_nodes, which_comp, which_component, best_component);
    }
    if(type == 5){
        //no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
        compress_branches(N, graph, branches, comp_graph, weights, comp_nodes, which_comp, which_component, best_component);
    }
    if(type == 6){
        no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
    }
    if(type == 7){
        no_compress_change(N, graph, comp_graph, which_component, best_component, weights);
    }
}

void print_final_path(list<int> &final_path){
    cout << final_path.size() << "\n";
    for(auto node : final_path){
        cout << node << " ";
    }
    cout << '\n';
}

void decompress_branches(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights, list<int> &comp_path, list<int> &final_path, vector<int> &which_comp, vector<CompNode> &comp_nodes){
    int neigh_before = -1, neigh_after = -1;
    //print_final_path(comp_path);
    while(!comp_path.empty()){
        int v = comp_path.front();
        comp_path.pop_front();

        if(!comp_path.empty()) neigh_after = comp_path.front();
        else neigh_after = -1;
        
        if(v < N && which_comp[v] == -1){
            final_path.emplace_back(v);
            neigh_before = v;
            continue;
        }
        int id = v-N-1;
        //cout << "id: " << id <<  " comp nodes size: " << comp_nodes.size() <<'\n';
        CompNode comp_node = comp_nodes[id];
        list<int> path = comp_node.path;
        //cout << "v: " << v << " neigh_before: " << neigh_before << " neigh_before: " << neigh_before << " neigh_after: " << neigh_after << " comp_node.last_nonbranch: " << comp_node.last_nonbranch << " comp_node.first_nonbranch: " << comp_node.first_nonbranch << '\n';
        //cout << "path: ";
        /*for(auto p : path){
            cout << p << ' ';
        }
        cout << '\n';*/
        
        if(neigh_after != -1 && comp_node.last_nonbranch != neigh_after) reverse(path.begin(), path.end());
        else if(neigh_before != -1 && comp_node.first_nonbranch != neigh_before) reverse(path.begin(), path.end());

        for(auto p : path){
            final_path.emplace_back(p);
        }
        neigh_before = v;
    }
    /*for(auto p : final_path){
            cout << p << " ";
        }
        cout << '\n';*/
}

bool check_time(float time_limit, clock_t &time_start){
    if(((float)(clock()-time_start)) / CLOCKS_PER_SEC < time_limit) return true;
    else return false;
}

int unseen_count(int v, vector<vector<int>> &comp_graph, vector<int> &seen)
{
    int res = 0;
    for(int u : comp_graph[v])
        if(!seen[u])
            res++;
    return res;
}

int best_heur_neigh(int v, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis)
{
    vector<int> best_u;
    int best_unseen_count = INF;
    for(int u : comp_graph[v])
        if(!vis[u] && seen[u] == 1)
        {
            int u_unseen_count = unseen_count(u, comp_graph, seen);
            if(u_unseen_count == 0)
                continue;
            if(best_unseen_count > u_unseen_count)
            {
                best_u = {u};
                best_unseen_count = u_unseen_count;
            }
            else if(best_unseen_count > u_unseen_count)
                best_u.emplace_back(u);
        }
    if(best_u.empty())
        return -1;
    return best_u[rng() % best_u.size()];
}

int best_neigh(int v, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis)
{
    vector<int> best_u;
    int best_weight = 0;
    for(int u : comp_graph[v])
        if(!vis[u] && seen[u] == 1)
        {
            if(weights[u] > best_weight)
            {
                best_u = {u};
                best_weight = weights[u];
            }
            else if(weights[u] == best_weight)
                best_u.emplace_back(u);
        }
    if(best_u.empty())
        return -1;
    return best_u[rng() % best_u.size()];
}

int test(vector<vector<int>> &graph, list<int> &path)
{
    vector<int> seen(graph.size(), 0);
    vector<bool> vis(graph.size(), false);
    if(path.front() >= graph.size())
    {
        cerr << "node >= N";
        for(int a : path)
            cout << a << ' ';
        cout << '\n';
        return 1;
    }
    vis[path.front()] = true;
    for(int u : graph[path.front()])
        seen[u]++;
    int prev = path.front();
    list<int>::iterator it = next(path.begin());
    do
    {
        int v = *it;
        if(v >= graph.size())
        {
            cerr << "node >= N";
            // for(int a : path)
            //     cout << a << ' ';
            // cout << '\n';
            return 1;
        }
        if(find(graph[prev].begin(), graph[prev].end(), v) == graph[prev].end())
        {
            cerr << prev << " nie ma sasiada " << v << '\n';
            // for(int a : path)
            //     cout << a << ' ';
            // cout << '\n';
            return 2;
        }
        if(vis[v])
        {
            cerr << v << " odwiedzony drugi raz\n";
            // for(int a : path)
            //     cout << a << ' ';
            // cout << '\n';
            return 3;
        }
        if(seen[v] > 1)
        {
            cerr << v << " widziany wielokrotnie\n";
            // for(int a : path)
            //     cout << a << ' ';
            // cout << '\n';
            return 4;
        }
        vis[v] = true;
        for(int u : graph[v])
            seen[u]++;
        prev = *it;
        it++;
    } while (it != path.end());
    cerr << "ZAJEBISCIE\n";
    return 0;
}



void sort_graph(vector<vector<int>> &graph)
{
    for(int i = 0; i < graph.size(); ++i)
        sort(graph[i].begin(), graph[i].end());
}

bool is_edge(int v, int u, vector<vector<int>> &graph)
{
    auto it = lower_bound(graph[v].begin(), graph[v].end(), u);
    if(it == graph[v].end() || *it != u)
        return false;
    return true;
}

bool see_node(int v, vector<vector<int>> &graph, vector<int> &seen, vector<bool> &vis)
{
    // if(vis[v])
    //     cerr << "dupsonik\n";
    vis[v] = true;
    for(int u : graph[v])
        seen[u]++;
    return true;
}

void unsee_node(int v, vector<vector<int>> &graph, vector<int> &seen, vector<bool> &vis)
{
    // if(!vis[v])
    //     cerr << "dupson\n";
    vis[v] = false;
    for(int u : graph[v])
    {
        if(--seen[u] < 0)
            cerr << "ujemne seen\n";
    }
}

bool see_path(list<int> &path, vector<vector<int>> &graph, vector<int> &seen, vector<bool> &vis)
{
    for(int v : path)
        if(!see_node(v, graph, seen, vis)) return false;
    return true;
}

bool see_path(list<int>::iterator begin, list<int>::iterator end, vector<vector<int>> &graph, vector<int> &seen, vector<bool> &vis)
{
    for(auto v_it = begin; v_it != end; ++v_it)
        if(!see_node(*v_it, graph, seen, vis)) return false;
    return true;
}

void unsee_path(list<int> &path, vector<vector<int>> &graph, vector<int> &seen, vector<bool> &vis)
{
    for(int v : path)
        unsee_node(v, graph, seen, vis);
}

void unsee_path(list<int>::iterator begin, list<int>::iterator end, vector<vector<int>> &graph, vector<int> &seen, vector<bool> &vis)
{
    for(auto v_it = begin; v_it != end; ++v_it)
        unsee_node(*v_it, graph, seen, vis);
}
bool extend_back(list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis, bool heur=true)
{
    //return true if it attached a new node
    int v;
    if(heur)
        v = best_heur_neigh(comp_path.back(), comp_graph, weights, seen, vis);
    else
        v = best_neigh(comp_path.back(), comp_graph, weights, seen, vis);
    if(v == -1)
        return false;
    comp_path.emplace_back(v);
    see_node(v, comp_graph, seen, vis);
    return true;
}

bool extend_front(list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis, bool heur=true)
{
    //return true if it attached a new node
    int v;
    if(heur)
        v = best_heur_neigh(comp_path.front(), comp_graph, weights, seen, vis);
    else
        v = best_neigh(comp_path.front(), comp_graph, weights, seen, vis);
    if(v == -1)
        return false;
    comp_path.emplace_front(v);
    see_node(v, comp_graph, seen, vis);
    return true;
}

void extend_all(list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis){
    while(extend_front(comp_path, comp_graph, weights, seen, vis)){}
    while(extend_back(comp_path, comp_graph, weights, seen, vis)){}
    while(extend_front(comp_path, comp_graph, weights, seen, vis, false)){}
    while(extend_back(comp_path, comp_graph, weights, seen, vis, false)){}
}
int local_search(list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis)
{
    //return total weight
    int start;
    do
    {
        start = rng() % comp_graph.size();
    }while(comp_graph[start].empty());
    comp_path.clear();
    comp_path.emplace_back(start);
    vis[start] = true;
    for(int u : comp_graph[start])
        seen[u]++;
    //building back
    while(extend_back(comp_path, comp_graph, weights, seen, vis) || extend_back(comp_path, comp_graph, weights, seen, vis, false)){}
    //building front
    while(extend_front(comp_path, comp_graph, weights, seen, vis) || extend_front(comp_path, comp_graph, weights, seen, vis, false)){}
    
    int result = 0;
    for(int v : comp_path)
        result += weights[v];
    //cerr << result << '\n';
    return result;
}

int call_start_search(int N, list<int> &comp_path, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &which_comp, vector<CompNode> &comp_nodes, int type, clock_t &time_start)
{
    //comp_path puste
    vector<int> seen(comp_graph.size(), 0);
    vector<bool> vis(comp_graph.size(), false);
    list<int> decomp_path;
    int best_result = 0;
    while(check_time(6.5f, time_start))
    {
        list<int> curr_path;
        int curr_result = local_search(curr_path, comp_graph, weights, seen, vis);
        if(curr_result > best_result)
        {
            best_result = curr_result;
            comp_path = curr_path;
        }
        seen.assign(comp_graph.size(), 0);
        vis.assign(comp_graph.size(), false);
    }
    //print_final_path(comp_path);
    //cerr << best_result << '\n';
    return best_result;
}
//DO ZMIANY
void find_alternative_tail(list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis, clock_t &time_start, float &time_limit){
    int pref_value = 0;
    int curr_value = 0;
    for(int v : comp_path)
        curr_value += weights[v];
    for(auto tail_start = comp_path.begin(); tail_start != comp_path.end() && check_time(time_limit, time_start); ++tail_start)
    {   
        list<int> tail = {*tail_start};

        while(extend_front(tail, comp_graph, weights, seen, vis)) {}
        while(extend_front(tail, comp_graph, weights, seen, vis, false)) {}

        tail.pop_back();
        int tail_value = 0;

        for(int v : tail) 
            tail_value += weights[v];

        if(tail_value >= pref_value)
        {
            unsee_path(comp_path.begin(), tail_start, comp_graph, seen, vis);
            comp_path.erase(comp_path.begin(), tail_start);
            comp_path.splice(comp_path.begin(), tail);
            pref_value = tail_value;
        }
        else if(tail_value >= curr_value - pref_value - weights[*tail_start])
        {
            int value = 0;
            unsee_path(next(tail_start), comp_path.end(), comp_graph, seen, vis);
            comp_path.erase(next(tail_start), comp_path.end());
            tail.reverse();
            comp_path.splice(next(tail_start), tail);
            curr_value = pref_value + tail_value + weights[*tail_start];
        }
        else unsee_path(tail, comp_graph, seen, vis);
        
        pref_value += weights[*tail_start];
    }
}

void update_pref(list<int>::iterator start, list<int>::iterator end, vector<int> &pref_value, vector<int> &weights)
{
    int last = pref_value[*start];
    for(auto it = next(start); it != end; ++it)
    {
        pref_value[*it] = last + weights[*it];
        last = pref_value[*it];
    }
}

void find_new_detour(list<int>::iterator detour_start, list<int> &detour, list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis, vector<int> &pref_value, int max_depth = 50000)
{
    auto omit = next(detour_start);
    unsee_node(*omit, comp_graph, seen, vis);
    vis[*omit] = true;
    detour = {*detour_start};
    int detour_value = 0;

    while((extend_back(detour, comp_graph, weights, seen, vis) || extend_back(detour, comp_graph, weights, seen, vis, false)) && detour.size() < max_depth)
    {
        detour_value += weights[detour.back()];
        int detour_end = -1;
        for(int v : comp_graph[detour.back()])
            if(!vis[v] && seen[v] == 2)
            {
                int best_u = -1;
                int pref_val_u = INF;
                for(int u : comp_graph[v])
                    if(u != detour.back() && u != *omit && vis[u])
                    {
                        if(pref_value[u] > pref_value[*omit] && pref_value[u] < pref_val_u)
                        {
                            best_u = u;
                            pref_val_u = pref_value[u];
                        }
                        break;
                    }
                if(best_u != -1 && detour_value + weights[v] > pref_value[best_u] - weights[best_u] - pref_value[*detour_start])
                {
                    detour.emplace_back(v);
                    see_node(v, comp_graph, seen, vis);
                    detour_end = best_u;
                    break;
                }
            }
        if(detour_end != -1)
        {
            see_node(*omit, comp_graph, seen, vis);
            auto end_it = find(omit, comp_path.end(), detour_end);
            unsee_path(omit, end_it, comp_graph, seen, vis);
            for(auto it = omit; it != end_it; ++it)
                pref_value[*it] = 0;
            comp_path.erase(omit, end_it);
            detour.pop_front();
            comp_path.splice(end_it, detour);
            update_pref(detour_start, comp_path.end(), pref_value, weights);
            return;
        }
    }

    see_node(*omit, comp_graph, seen, vis);
    detour.pop_front();
    unsee_path(detour, comp_graph, seen, vis);
}

void find_detours(list<int> &comp_path, vector<vector<int>> &comp_graph, vector<int> &weights, vector<int> &seen, vector<bool> &vis, vector<int> &pref_value, clock_t &time_start, float &time_limit)
{
    pref_value.assign(comp_graph.size(), 0);
    pref_value[comp_path.front()] = weights[comp_path.front()];
    update_pref(comp_path.begin(), comp_path.end(), pref_value, weights);
    auto end = prev(comp_path.end());
    list<int> detour;
    for(auto detour_start = comp_path.begin(); detour_start != end && check_time(time_limit, time_start); ++detour_start)
    {
        detour.clear();
        find_new_detour(detour_start, detour, comp_path, comp_graph, weights, seen, vis, pref_value);
    }
}

void call_main_search(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights, list<int> &final_path, vector<int> &which_comp, vector<CompNode> &comp_nodes, list<int> &comp_path, clock_t &time_start, int type){
    float time_limit = 19.f;
    vector<bool> vis(comp_graph.size(), false);
    vector<int> seen(comp_graph.size(), 0);
    vector<int> pref_value(comp_graph.size(), 0);
    see_path(comp_path, comp_graph, seen, vis);
    
    while(check_time(time_limit, time_start))
    {
        if(type == 3 || type == 7){ //działa bardzo dobrze - nie jest do zmiany
            find_alternative_tail(comp_path, comp_graph, weights, seen, vis, time_start, time_limit); //budowanie alternatywnego taila dla każdego wierzchołka z ścieżki po kolei
            find_detours(comp_path, comp_graph, weights, seen, vis, pref_value, time_start, time_limit); //szukanie objazdu
            extend_all(comp_path, comp_graph, weights, seen, vis); //poszerzanie końców
        } else if(type == 5){ //działa źle - do zmiany
            //print_final_path(comp_path);
            find_alternative_tail(comp_path, comp_graph, weights, seen, vis, time_start, time_limit); //budowanie alternatywnego taila dla każdego wierzchołka z ścieżki po kolei
            find_detours(comp_path, comp_graph, weights, seen, vis, pref_value, time_start, time_limit); //szukanie objazdu
            extend_all(comp_path, comp_graph, weights, seen, vis); //poszerzanie końców
        } else { //działa średnio - do zmiany
            find_alternative_tail(comp_path, comp_graph, weights, seen, vis, time_start, time_limit); //budowanie alternatywnego taila dla każdego wierzchołka z ścieżki po kolei
            find_detours(comp_path, comp_graph, weights, seen, vis, pref_value, time_start, time_limit); //szukanie objazdu
            extend_all(comp_path, comp_graph, weights, seen, vis); //poszerzanie końców
        }
    }
}

void find_solution(int N, vector<vector<int>> &graph, vector<vector<int>> &comp_graph, vector<int> &weights, list<int> &final_path, vector<int> &which_comp, vector<CompNode> &comp_nodes, int type, clock_t &time_start)
{
    list<int> comp_path;

    if(type == 4){
        call_start_search(N, comp_path, graph, comp_graph, weights, which_comp, comp_nodes, type, time_start);
        call_main_search(N, graph, comp_graph, weights, final_path, which_comp, comp_nodes, comp_path, time_start, type);
        decompress_branches(N, graph, comp_graph, weights, comp_path, final_path, which_comp, comp_nodes);
    }
    else if(type == 5)
    {
        get_diameter(comp_graph, comp_path, weights);
        call_main_search(N, graph, comp_graph, weights, final_path, which_comp, comp_nodes, comp_path, time_start, type);
        decompress_branches(N, graph, comp_graph, weights, comp_path, final_path, which_comp, comp_nodes);
    }
    else {
        call_start_search(N, comp_path, graph, comp_graph, weights, which_comp, comp_nodes, type, time_start);
        call_main_search(N, graph, comp_graph, weights, final_path, which_comp, comp_nodes, comp_path, time_start, type);
        final_path = comp_path;
    }
}

int main()
{
    clock_t time_start = clock();
    int N, M, total_components, type, best_component;
    vector<vector<int>> graph, comp_graph;
    vector<int> which_component, which_comp, weights;
    list<int> final_path;
    vector<pair<int, int>> bridges;
    vector<bool> branches; //branch - a node that has at most 2 neighbours
    vector<CompNode> comp_nodes;

    get_input(N, M, graph);
    get_components(N, M, graph, which_component, total_components);
    find_bridges(N, graph, bridges);
    find_branches(N, graph, branches);
    type = get_type(N, M, graph, which_component, total_components, bridges);
    //cerr << "TYPE: " << type << '\n'; 
    best_component = largest_component(which_component);
    change_graph(N, M, graph, branches, which_comp, comp_nodes, weights, comp_graph, which_component, best_component, type);
    sort_graph(comp_graph);
    find_solution(N, graph, comp_graph, weights, final_path, which_comp, comp_nodes, type, time_start);
    print_final_path(final_path);
}

//branches na chains
//which_comp na which_compr/which_comp_node
//jak nazwac węzeł po angielsku