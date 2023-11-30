#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#define NUM_NODES 1000
#define MAX_NODES 10000
#define MAX_CAPACITY 10
#define MAX_cost 30
#define MAX_B 7

#define WHITE 0
#define GRAY 1
#define BLACK 2

int TotalF;
int x = 0;
int E, V = NUM_NODES; // nodes and edges
int S, T; // source and sink


int cost[MAX_NODES][MAX_NODES];// cost 
int graph[NUM_NODES][NUM_NODES];
int capacity[NUM_NODES][NUM_NODES];//capacity
int flow[NUM_NODES][NUM_NODES];//flow
int b[NUM_NODES];//demand for node
int residual_capacity[MAX_NODES][MAX_NODES], residual_cost[MAX_NODES][MAX_NODES];// for residual network

int color[MAX_NODES]; // needed for breadth-first search               
int pred[MAX_NODES];  // array to store augmenting path
int head, tail;
int q[MAX_NODES + 2];

//set graph with nodes [with b(i)] and edges [with capacity and cost] 
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
                capacity[i][j] = rand() % (MAX_CAPACITY + 1); // capacity is between 0 and 10
                cost[i][j] = rand() % (MAX_cost + 1); //cost 0-20
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
    else if (total_demand < 0) {
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
    printf("Total sum of b(i) is %d\nThere is a feasible solution for input Graph!\n", sum);

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

//build residual network
void residualGraph() {
    for (int u = 0; u < V; u++) {
        for (int v = 0; v < V; v++) {
            if (capacity[u][v] > 0) {
                residual_capacity[u][v] = capacity[u][v] - flow[u][v];
                residual_cost[u][v] = cost[u][v];
                residual_capacity[v][u] = flow[u][v];
                residual_cost[v][u] = -cost[u][v];
            }
            else {
                residual_capacity[u][v] = 0;
                residual_cost[u][v] = 0;
                residual_capacity[v][u] = 0;
                residual_cost[v][u] = 0;
            }
        }
    }

    // Add edges to ensure flow conservation
    for (int v = 0; v < V; v++) {
        if (v != S && v != T) {
            residual_capacity[v][v] = 10000000;
            residual_cost[v][v] = 0;
        }
    }
}

//bellman ford algorthm
bool bellmanFord(int s, int t, int dist[], int parent[]) {
    // Initialize the distance and parent arrays
    memset(dist, INT_MAX, sizeof(dist));
    memset(parent, -1, sizeof(parent));
    dist[s] = 0;

    // Create a queue and add the source vertex to it
    int front = 0, rear = 0;
    int queue[MAX_NODES];
    queue[rear++] = s;

    // Run the Bellman-Ford algorithm
    while (front != rear) {
        int u = queue[front++];
        for (int v = 0; v < V; v++) {
            if (residual_capacity[u][v] > flow[u][v] && dist[v] > dist[u] + residual_cost[u][v]) {
                dist[v] = dist[u] + residual_cost[u][v];
                parent[v] = u;
                if (v != s && v != t) {
                    queue[rear++] = v;
                }
            }
        }
    }
    // check for negative cycle
    for (int u = 0; u < V; u++) {
        for (int v = 0; v < V; v++) {
            if (dist[v] > dist[u] + residual_cost[u][v]) {
                return false;
            }
        }
    }
    // check if the sink is not reachable
    if (parent[t] == -1) {
        return false;
    }
    return true;
}

//cycle canceling algorithm
void Cycle_Canceling(int s, int t, int totalFlow) {
    //int totalCost = 0;
    int dist[MAX_NODES];
    int parent[MAX_NODES];

    // Run the Bellman-Ford algorithm
    while (bellmanFord(s, t, dist, parent)) {
        // Find the minimum flow along the path
        int flow1 = INT_MAX;
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            flow1 = Min(flow1, residual_capacity[u][v]);
        }

        // Update the flow and cost
        totalFlow += flow1;
        //totalCost += flow1 * dist[t];
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            flow[u][v] += flow1;
            flow[v][u] -= flow1;
        }
    }

}

//for queue
void enqueue(int x) {
    q[tail] = x;
    tail++;
    color[x] = GRAY;
}
int dequeue() {
    int x = q[head];
    head++;
    color[x] = BLACK;
    return x;
}

//BFS algorithm
int bfs(int start, int target) {
    int u, v;
    for (u = 0; u < V; u++) {
        color[u] = WHITE;
    }
    head = tail = 0;
    enqueue(start);
    pred[start] = -1;
    while (head != tail) {
        u = dequeue();
        // Search all adjacent white nodes v. If the capacity
        // from u to v in the residual network is positive,
        // enqueue v.
        for (v = 0; v < V; v++) {
            if (color[v] == WHITE && capacity[u][v] - flow[u][v] > 0) {
                enqueue(v);
                pred[v] = u;
            }
        }
    }
    // If the color of the target node is black now,
    // it means that we reached it.
    return color[target] == BLACK;
}

//edmonds-krap algorithm
int max_flow(int source, int sink) {
    int i, j, u;
    // Initialize empty flow.
    int max_flow = 0;
    for (i = 0; i < V; i++) {
        for (j = 0; j < V; j++) {
            flow[i][j] = 0;
        }
    }
    // While there exists an augmenting path,
    // increment the flow along this path.
    while (bfs(source, sink)) {
        // Determine the amount by which we can increment the flow.
        int increment = 10000;
        for (u = V - 1; pred[u] >= 0; u = pred[u]) {
            increment = Min(increment, capacity[pred[u]][u] - flow[pred[u]][u]);
        }
        // Now increment the flow.
        for (u = V - 1; pred[u] >= 0; u = pred[u]) {
            flow[pred[u]][u] += increment;
            flow[u][pred[u]] -= increment;
        }
        max_flow += increment;
    }
    // No augmenting path anymore. We are done.
    return max_flow;
}

//get total cost by calculating costs of the optimal flow of the edges
void TotalCost() {
    int tcost = 0;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (flow[i][j] > 0) {
                tcost += cost[i][j]*flow[i][j];
                TotalF += flow[i][j];
            }
        }
    }
    printf("The MINIMUM cost for this input will be : % d\n", tcost);
    printf("Total flow in the network will be: %d\n", TotalF);
}

int main() {

    clock_t begin = clock();//start measuring time

    setGraph();

    //now add source and sink nodes and add edges (define flow network)
    S = V;
    T = V + 1;
    V = V + 2;
    addSourceSink();

    //get max flow of the network as starting flow for cycle cancleng algorithm
    int mflow = max_flow(S, T);

    //set residual graph
    V = V - 2;
    residualGraph();


    //run cycle canceling algorithm for the max flow we found
    Cycle_Canceling(S, T, mflow);

    //pritn total cost - it wil be the min cost
    TotalCost();
   
    clock_t end = clock();
    printf("\nTime: %.7f\n", (double)(end - begin) / CLOCKS_PER_SEC);

    return 0;
}
