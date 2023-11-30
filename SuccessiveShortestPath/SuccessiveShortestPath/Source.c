#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#define NUM_NODES 30
#define MAX_NODES 1000
#define MAX_CAPACITY 1
#define MAX_cost 10
#define MAX_B 7

int x = 0;
int E,V = NUM_NODES;// nodes and edges
int S, T; // source and sink

int cost[MAX_NODES][MAX_NODES];
int graph[NUM_NODES][NUM_NODES];
int capacity[NUM_NODES][NUM_NODES];
int flow[NUM_NODES][NUM_NODES];
int b[NUM_NODES];

int pie[MAX_NODES]; // node potential
int ns_E[MAX_NODES], ns_D[MAX_NODES]; // sets of nodes
int e[MAX_NODES]; // pseudo flow
int  R_cost[MAX_NODES][MAX_NODES];//reduced cost
int residual_capacity[MAX_NODES][MAX_NODES], residual_cost[MAX_NODES][MAX_NODES];// for residual network

//for dijkstra
int dist[MAX_NODES];//d
int previous[MAX_NODES];
bool visited[MAX_NODES];


//set graph of node [with b(i)] and edges [with cost and capacity]
void setGraph() {
    srand(time(0));
    int e_cnt = 0;
    // Initialize all edges to 0
    for (int i = 0; i < V; i++) {
        b[i] = 0;
        for (int j = 0; j < V; j++) {
            graph[i][j] = 0;
            capacity[i][j] = 0;
            flow[i][j] = 0;
        }
    }

    // Add random edges
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (rand() % 2 == 0) {
                e_cnt++;
                graph[i][j] = 1;
                capacity[i][j] = rand() % (MAX_CAPACITY+1); // capacity is between 0 and 10
                cost[i][j] = rand() % (MAX_cost+1); //cost 0-20
            }
        }
    }
    E = e_cnt;

    int total_demand = 0;
    for (int i = 0; i < V; i++) {
        b[i] = (rand() %
            (MAX_B - (-MAX_B + 1)) + (-MAX_B));
        total_demand += b[i];
    }

    // adjust demand to make total demand zero
    int i = 0;
    if (total_demand > 0) {
        while (total_demand != 0) {
            b[i]--;
            total_demand--;
            if (i = V - 1)
                i = 0;
        }
   }
    else if(total_demand<0) {
        while (total_demand != 0) {
            b[i]++;
            total_demand++;
            if (i = V - 1)
                i = 0;
        }
    }
   
    //check sum b(i)
    int sum = 0;
    for (int i = 0; i < NUM_NODES; i++) {
        sum += b[i];
    }
    printf("Total sum of b(i) is %d\nThere is a seasible solution for input Graph!\n", sum);

}

//returns min value
int Min(int a, int b) {
    return (a < b) ? a : b;
}

//if b(i) > 0 add edge (S,i) with capicity b(i) and cost 0
//if b(i) < 0 add edge (i,T) with capicity -b(i) and cost 0
void addSourceSink() {
    for (int i = 0; i < V; i++) {
        if (b[i] > 0) {
            capacity[S][i] = b[i];
            cost[S][i] = 0;
        }
        else if (b[i] < 0) {
            capacity[i][T] = -b[i];
            cost[i][T] = 0;
        }
    }
}

//set node potential of given node to given potential
void setNodePotential(int node, int potential) {
    pie[node] = potential;
}

//check if E set is empty
bool checkE() {
    for (int i = 0; i < V; i++) {
        if (ns_E[i] == 1) {
            return false;
        }
    }
    return true;
}

//initialize node sets E and D
void initializeED() {
    for (int i = 0; i < V; i++) {
        if (e[i] > 0) {
            ns_E[i] = 1;
            ns_D[i] = 0;
        }
        if (e[i] < 0) {
            ns_D[i] = 1;
            ns_E[i] = 0;
        }
        
        if (e[i] == 0) {
            ns_D[i] = 0;
            ns_E[i] = 0;
        }
        
    }
}

//set reduced cost of given edge
void setReduceCost(int i, int j) {
    R_cost[i][j] = cost[i][j] - pie[i] - pie[j];
}

//build residual network with reduced costs
void residualGraph() {
    for (int u = 0; u < V; u++) {
        for (int v = 0; v < V; v++) {
            if (capacity[u][v] > 0) {
                {
                    if (capacity[u][v] - flow[u][v] > 0) {
                        residual_capacity[u][v] = capacity[u][v] - flow[u][v];
                        residual_cost[u][v] = R_cost[u][v];
                        residual_capacity[v][u] = flow[u][v];
                        residual_cost[v][u] = -R_cost[u][v];
                    }
                    else {
                        residual_capacity[u][v] = 0;
                        residual_cost[u][v] = 0;
                        residual_capacity[v][u] = 0;
                        residual_cost[v][u] = 0;
                    }
                }

            }
        }
    }
    /*
    // Add edges to ensure flow conservation
    for (int v = 0; v < V; v++) {
        if (v != S && v != T) {
            residual_capacity[v][v] = 10000000;
            residual_cost[v][v] = 0;
        }
    }
    */
}

//deijkstra algorithm from node k
void dijkstra(int k) {
    // Initialize distances and visited array
    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        visited[i] = false;
    }

    // Set the distance of the starting node to 0
    dist[k] = 0;
    // Find the shortest path for all vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to start in first iteration.
        int min = INT_MAX, min_index;
        for (int v = 0; v < V; v++)
            if (!visited[v] && dist[v] <= min)
                min = dist[v], min_index = v;

        // Mark the picked vertex as processed
        int u = min_index;
        visited[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < V; v++) {
            // Update dist[v] only if is not in visited, there is an edge from 
            // u to v, and total weight of path from start to v through u is 
            // smaller than current value of dist[v]
            if (!visited[v] && residual_capacity[u][v] > 0 && dist[u] != INT_MAX && dist[u] + residual_cost[u][v] < dist[v]) {
                dist[v] = dist[u] + residual_cost[u][v];
                previous[v] = u;
            }
        }
    }
}

//get min residual capacity from path P
int getMinrs(int k, int I) {
    int v = I;
    int min = INT_MAX;
    while (v != k) {
        int u = previous[v];
        if (residual_capacity[u][v] < min)
            min = residual_capacity[u][v];
        v = u;
    }
    return min;
}

//add flow to path P
void addFlowP(int k, int I, int flowToAdd) {
    int v = I;
    while (v != k) {
        int u = previous[v];
        flow[u][v] += flowToAdd;
        flow[v][u] -= flowToAdd;
        v = u;
    }
}

//calculate flow diff on current node
int flow_diff(int i) {
    int in_flow = 0, out_flow = 0;
    for (int j = 0; j < V; j++) {
        in_flow += flow[j][i];
        out_flow += flow[i][j];
    }
    return in_flow - out_flow; 
}

//print minimum cost
void TotalCost() {
    int tcost = 0;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (flow[i][j] > 0) {
                tcost += cost[i][j]*flow[i][j] ;
            }
        }
    }
    printf("The MINIMUM cost for this input will be: %d\n", tcost);
}

int main() {
    
    clock_t begin = clock();//start measuring time

    setGraph();
 
    //set node potential to 0
    for (int i = 0; i < V; i++) {
        setNodePotential(i, 0);
    }
   
    //set reduced costs
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (capacity[i][j] > 0) {
                setReduceCost(i, j);
            }
        }
    }

    //initialize e(i) = b(i);
    for (int i = 0; i < V; i++) {
        e[i] = b[i];
    }

    //initialize node sets E and D
    initializeED();

    //now add source and sink nodes and add edges (define flow network)
    S = V;
    T = V + 1;
    V = V + 2;
    addSourceSink();
    
    V = V + 2;
    //build residual graph
    residualGraph();
    //V = V - 2;

    //now, for that flow network we need to solve min cost flow problem
    int k;
    while (checkE() == false)
    {
        //get node k from set E
        for (int i = 0; i < V; i++) {
            if (ns_E[i] == 1) {
                k = i;
                break;
            }
        }
        
        //dijkstra from k
        dijkstra(k);
        
        //find node I from set D that there is a path from k to I
        int I = V + 1;
        for (int i = 0; i < V; i++) {
            if (ns_D[i] == 1 && dist[i] != INT_MAX) {
                I = i;
                break;
            }
        }
        if (I == V + 1)// node I not found
            break;

        //update pie[i] = pie[i] - d[i]
        for (int i = 0; i < V; i++) {
            if(dist[i] != INT_MAX)
                setNodePotential(i, pie[i] - dist[i]);
        }
        
        //get min residual capacity of all edges on the path from k to i
        int minRS = getMinrs(k, I);
        
        //calculate flow to add 
        int flowToadd = Min(e[k], -e[I]);
        flowToadd = Min(flowToadd, minRS);

        //add this amount to the path from k to I
        addFlowP(k, I, flowToadd);
        
        //update reduced costs 
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (capacity[i][j] > 0) {
                    setReduceCost(i, j);
                }
            }
        }

        //update G(X)
        residualGraph();

        //set new e(i)
        for (int i = 0; i < V; i++) {
            e[i] = b[i] + flow_diff(i);
        }
        
        //update E,D
        initializeED();
        
    }
    
    TotalCost();

    clock_t end = clock();
    printf("\nTime: %.7f\n", (double)(end - begin) / CLOCKS_PER_SEC);

    return 0;
}
