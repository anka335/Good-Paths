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

int main(){
    int N, M;
    vector<vector<int>> graph;

    get_input(N, M, graph);
}