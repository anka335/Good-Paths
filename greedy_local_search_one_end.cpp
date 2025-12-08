#include <bits/stdc++.h>
using namespace std;
constexpr int INF = 2e9;
mt19937 rng(2137);
void shuffle_graph(vector<vector<int>> graph)
{
    for(int i = 0; i < graph.size(); ++i)
        shuffle(graph[i].begin(), graph[i].end(), rng);
}
int unseen_count(int v, vector<vector<int>> &graph, vector<int> &seen)
{
    int res = 0;
    for(int u : graph[v])
        if(!seen[u])
            res++;
    return res;
}
pair<int, int> best_neigh(int v, vector<vector<int>> &graph, vector<int> &seen)
{
    int best_unseen_count = INF, best_u = INF, any_u = INF, any_unseen_count = INF;
    for(int u : graph[v])
        if(seen[u] == 1)
        {
            any_u = u;
            int u_unseen_count = unseen_count(u, graph, seen);
            any_unseen_count = u_unseen_count;
            if(best_unseen_count > u_unseen_count && u_unseen_count != 0)
            {
                best_unseen_count = u_unseen_count;
                best_u = u;
            }
        }
    if(best_u != INF)
        return {best_unseen_count, best_u};
    if(any_u != INF)
        return {any_unseen_count, any_u};
    return {-1, -1};
    
}
void greedy_local_search(int start, list<int> &path, vector<vector<int>> &graph, vector<int> &seen)
{
    int n = graph.size();
    seen.assign(n, 0);
    bool change;
    path = {start};
    seen[start] = 1;
    for(int u : graph[start])
        seen[u]++;
    int best_u, temp;
    do
    {
        tie(temp, best_u) = best_neigh(path.back(), graph, seen);
        if(temp != -1)
        {
            path.emplace_back(best_u);
            for(int k : graph[best_u])
                seen[k]++;
        }
    } while(temp != -1);
}
int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    clock_t time_start = clock();
    int n, m;
    cin >> n >> m;
    vector<vector<int>> graph(n);
    for(int i = 0, a, b; i < m; ++i)
    {
        cin >> a >> b;
        graph[a].emplace_back(b);
        graph[b].emplace_back(a);
    }
    vector<int> seen(n, 0);
    list<int> best_path;
    list<int> path;
    vector<int> starts(n);
    for(int i = 0; i < n; ++i)
        starts[i] = i;
    shuffle(starts.begin(), starts.end(), rng);
    while(((float)(clock()-time_start)) / CLOCKS_PER_SEC < 19.0f && !starts.empty())
    {
        greedy_local_search(starts.back(), path, graph, seen);
        starts.pop_back();
        if(best_path.size() < path.size())
            best_path = path;
    }
    cout << best_path.size() << '\n';
    for(int a : best_path)
        cout << a << ' ';
    cout << '\n';
}