// ASA 20/21 - 1º Projeto
// Grupo 090
// Sara Marques (93342)

#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <cstdio>
#define SORT 0
#define FINDK 1
using namespace std;

int n, m;

vector<vector<int>> graph;
list<int> topologicalSort;
vector<int> color;
enum color {white, gray, black, yellow};

void readInput() {

    if(!scanf("%d %d", &n, &m))
        exit(1);

    graph = vector<vector<int>>(n, vector<int>());

    int i, u, v;

    for(i = 0; i < m; i++) {
        if (!scanf("%d %d", &u, &v)) 
            exit(1);
        graph[u-1].push_back(v-1);
    }
}

void dfsVisit(int src, int flag) {

    stack<int> stack;
    vector<bool> onStack(n, false);

    color[src] = gray;
    stack.push(src);
    onStack[src] = true;

    while(!stack.empty()) {

        int top = stack.top();

        if(flag)
            stack.pop();

        bool hasChildren = false;

        vector<int>::iterator child;

        for(child = graph[top].begin(); child != graph[top].end(); child++) {
            if(color[*child] == white || (onStack[*child] && !flag)) {
                hasChildren = true;
                color[*child] = gray;
                stack.push(*child);
                onStack[*child] = true;
            }
        }
        
        color[top] = black;

        if(!flag) {
            if(!hasChildren) {                    
                while(color[top] == black) {

                    topologicalSort.push_front(top);
                    color[top] = yellow;    
                    onStack[top] = false;

                    stack.pop();

                    if(stack.empty()) 
                        break;

                    top = stack.top();

                    if(color[top] == yellow) {
                        stack.pop();

                        if(stack.empty()) 
                            break;

                        top = stack.top();                        
                    }
                }
            }
        }    
    }
}

void dfs() {
    
    topologicalSort = list<int>();
    color = vector<int>(n, white);
    int i;

    for(i = 0; i < n; i++) {
        if(color[i] == white) 
            dfsVisit(i, SORT);
    }
}

int getNumberOfInterventions() {

    color = vector<int>(n, white);
    int interventions = 0;
    list<int>::iterator i;

    for(i = topologicalSort.begin(); i != topologicalSort.end(); i++) { //ou ++i
        if(color[*i] == white) {
            dfsVisit(*i, FINDK);
            interventions++;
        }
    }   
    return interventions; 
}

int getLongestSequence() {

    int aux = 0, sequence = 0;
    vector<int> max(n, 1);
    list<int>::reverse_iterator i;

    for(i = topologicalSort.rbegin(); i != topologicalSort.rend(); i++) { //ou ++i

        vector<int>::iterator j;
        for(j = graph[*i].begin(); j != graph[*i].end(); j++) {

            if(aux < max[*j]) {
                aux = max[*j];
            }
        }
        max[*i] += aux;
        aux = 0;   

        if(sequence < max[*i]) 
            sequence = max[*i];        
    } 
    return sequence;
}

int main() {

    int k, l;

    readInput();
    dfs();
    k = getNumberOfInterventions();
    l = getLongestSequence();

    printf("%d %d\n", k, l);

    return 0;
}