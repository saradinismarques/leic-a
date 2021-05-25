// ASA 20/21 - 2º Projeto
// Grupo 090
// Sara Marques (93342)

#include <iostream>
#include <vector>
#include <queue>
#include <cstdio>
using namespace std;

int n, k;
int size;

vector<vector<int>> graph;
vector<vector<int>> graphRes;


bool bfs(int source, int sink, int parent[]) {
    vector<bool> visited(size, false);
    int capacity, u;
    queue<int> queue;

    queue.push(source);
    visited[source] = true;
    parent[source] = -1;

    while(!queue.empty()) {
        u = queue.front();
        queue.pop();

        for(int v = 0; v < size; v++) {
            capacity = graphRes[u][v];
            if(visited[v] == false && capacity > 0) {
                if (v == sink) {
                    parent[v] = u;
                    return true;
                }
                queue.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return false;
}


int edmondsKarp(int source, int sink, int max_flow_value) {
    graphRes = vector<vector<int>>(size, vector<int>());
    int i, j, max_flow = 0, parent[size], path_flow, u, v, capacity;

    for(i = 0; i < size; i++) {
        vector<int> row;
        graphRes.push_back(row);
        for(j = 0; j < size; j++)
            graphRes[i].push_back(graph[i][j]);
    }
    
    while(bfs(source, sink, parent)) {
        path_flow = max_flow_value;
        v = sink;

        while(v != source) {
            u = parent[v];
            capacity = graphRes[u][v];
            path_flow = min(path_flow, capacity);
            v = u;
        }
        v = sink;
        while(v != source) {
            u = parent[v];
            graphRes[u][v] -= path_flow;
            graphRes[v][u] += path_flow;
            v = u;
        }
        max_flow += path_flow;
    }
    return max_flow;
}


void readInput() {
    if(!scanf("%d %d", &n, &k))
        exit(1);
    
    size = n + 2;
    int i, j, x, y, c;
    graph = vector<vector<int>>(size, vector<int>());

    for(i = 0; i < size; i++) {
        vector<int> row;
        graph.push_back(row);
        for(j = 0; j < size; j++)
            graph[i].push_back(0);
    }
    for(i = 1; i < size - 1; i++) {
        if (!scanf("%d %d", &x, &y)) 
            exit(1);
        graph[0][i] = x;
        graph[i][size - 1] = y;
    }
    for(x = 0; x < k; x++) {
        if (!scanf("%d %d %d", &i, &j, &c)) 
            exit(1);
        graph[i][j] = c;
        graph[j][i] = c;
    }
}


int main() {
    int cost;
    
    readInput();

    int source = 0, sink = size - 1, max_flow_value = size * size; 

    cost = edmondsKarp(source, sink, max_flow_value);
    printf("%d\n", cost);

    return 0;
}