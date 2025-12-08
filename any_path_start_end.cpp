#include <bits/stdc++.h>
using namespace std;
vector<int> best_path;
void dfs(int v, int end, vector<int> &curr_path, vector<int> &seen_count, vector<vector<int>>& graph)
{
    curr_path.emplace_back(v);
    for(int u : graph[v])
        seen_count[u]++;
    if(seen_count[end] != 0)
    {
        best_path = curr_path;
        return;
    }
    else
    {
        for(int u : graph[v])
        {
            if(!best_path.empty())
                return;
            if(seen_count[u] == 1)
            {
                dfs(u, end, curr_path, seen_count, graph);
            }
        }
    }

    for(int u : graph[v])
        seen_count[u]--;
    curr_path.pop_back();
}
int main()
{
    int n, m, start = 0, end = 5;
    cin >> n >> m;
    vector<vector<int>> graph(n);
    for(int i = 0, a, b; i < m; ++i)
    {
        cin >> a >> b;
        graph[a].emplace_back(b);
        graph[b].emplace_back(a);
    }
    vector<int> curr_path;
    vector<int> seen_count(n, 0);
    seen_count[start] = 1;
    dfs(start, end, curr_path, seen_count, graph);
    best_path.emplace_back(end);
    cout << best_path.size() << '\n';
    for(auto v : best_path)
        cout << v << ' ';
    cout << '\n';
}