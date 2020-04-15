#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

bool bfs(int V, int rGraph[V][V], int s, int t, int parent[]);
int fordFulkerson(int V, int graph[V][V], int s, int t);

// struct for router
typedef struct Router {
	int id;
	int downspeed;
	int maxload;	// reserved for future load simulation (currently unused)
	int load;	// reserved for future load simulation (currently unused)
} Router;

// struct to hold in and out table for each router
typedef struct inout {
	int *in;	// in[j]  = 1 if there is a link j -> this router
	int *out;	// out[j] = 1 if there is a link this router -> j
} inout;

// used for static implementation of queue for bfs
int head = 0;
int tail = 0;

int main() {

	int N;
	printf("give number of routers: ");
	if (scanf("%d", &N) != 1 || N < 2) {
		printf("please give a number of routers >= 2\n");
		return 1;
	}
	printf("\n\n");

	Router routers[N]; // table of all routers
	int i, j;
	srand(time(0));
	// initialize each router with random values
	for (i = 0; i < N; i++) {
		routers[i].id = i + 1;
		routers[i].downspeed = 1 + rand() % 100;
		routers[i].maxload = 1 + rand() % 100;
		routers[i].load = 0;
	}

	int routingtable[N][N];

	// initialize table with 0
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			routingtable[i][j] = 0;
		}
	}

	// make random router connections
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			// if i need undirected graph remove comments here
			//if (j>i){
				if (i != j)
					routingtable[i][j] = rand() % 2;
				//routingtable[j][i] = routingtable[i][j];
			//}
		}
	}

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			printf("%d ", routingtable[i][j]);
		}
		printf("\n");
	}

	inout inout_table[N];
	// allocate memory for in and out tables of each router
	for (i = 0; i < N; i++) {
		inout_table[i].in = (int*)malloc(N * sizeof(int));
		inout_table[i].out = (int*)malloc(N * sizeof(int));
		if (inout_table[i].in == NULL || inout_table[i].out == NULL) {
			printf("memory allocation failed\n");
			return 1;
		}
	}

	// out table is a row and in table is a column of the routing table
	// (routingtable[i][j] means a link from router i to router j)
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			inout_table[i].out[j] = routingtable[i][j];
			inout_table[i].in[j] = routingtable[j][i];
		}
	}

	printf("\n\n");
	for (i = 0; i < N; i++) {
		printf("router %d in table: ", i + 1);
		for (j = 0; j < N; j++) {
			printf("%d ", inout_table[i].in[j]);
		}
		printf("\n");
	}

	printf("\n\n");

	for (i = 0; i < N; i++) {
		printf("router %d out table: ", i + 1);
		for (j = 0; j < N; j++) {
			printf("%d ", inout_table[i].out[j]);
		}
		printf("\n");
	}

	// include weights in the routing graph:
	// a link's capacity is the downspeed of the slower of its two routers
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (routingtable[i][j] != 0) {
				if (routers[i].downspeed < routers[j].downspeed) {
					routingtable[i][j] = routers[i].downspeed;
				}
				else {
					routingtable[i][j] = routers[j].downspeed;
				}
			}
		}
	}

	printf("\n routing table after weights are included:\n");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			printf("%d ", routingtable[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for (i = 0; i < N; i++) {
		printf("router %d downspeed: %d\n", i + 1, routers[i].downspeed);
	}

	int maxflow;
	// fordFulkerson algorithm to calculate maximum flow in the routing graph
	maxflow = fordFulkerson(N, routingtable, 0, N - 1);
	if (maxflow == 0) {
		printf("\n no path exist between the 2 routers\n");
	}
	else printf("\nmax flow: %d\n", maxflow);

	// free allocated memory
	for (i = 0; i < N; i++) {
		free(inout_table[i].in);
		free(inout_table[i].out);
	}

	return 0;
}

void enqueue(int q[], int x) {
	q[tail] = x;
	tail++;
}

int dequeue(int q[]) {
	int x = q[head];
	head++;
	return x;
}

int min(int x, int y) {
	return x < y ? x : y;  // returns minimum of x and y
}

/*
 * The bfs() and fordFulkerson() functions below implement the
 * Edmonds-Karp max-flow algorithm, adapted from the GeeksforGeeks
 * article "Ford-Fulkerson Algorithm for Maximum Flow Problem".
 */

bool bfs(int V, int rGraph[V][V], int s, int t, int parent[])
{
	// Reset the queue: head/tail are global and must start from 0
	// on every call, otherwise they keep growing and overflow q[]
	head = 0;
	tail = 0;

	// Create a visited array and mark all vertices as not visited
	bool visited[V];
	int i;
	for (i = 0; i < V; i++) {
		visited[i] = false;
	}

	// Create a queue, enqueue source vertex and mark source vertex
	// as visited. Each vertex is enqueued at most once, so V slots suffice.
	int q[V];
	enqueue(q, s);
	visited[s] = true;
	parent[s] = -1;

	// Standard BFS Loop
	while (head != tail)
	{
		int u = dequeue(q);

		int v;
		for (v = 0; v < V; v++)
		{
			if (visited[v] == false && rGraph[u][v] > 0)
			{
				enqueue(q, v);
				parent[v] = u;
				visited[v] = true;
			}
		}
	}

	// If we reached sink in BFS starting from source, then return
	// true, else false
	return (visited[t] == true);
}

// Returns the maximum flow from s to t in the given graph
int fordFulkerson(int V, int graph[V][V], int s, int t)
{
	int u, v;

	// Create a residual graph and fill the residual graph with
	// given capacities in the original graph as residual capacities
	// in residual graph
	int rGraph[V][V]; // Residual graph where rGraph[i][j] indicates
	                  // residual capacity of edge from i to j (if there
	                  // is an edge. If rGraph[i][j] is 0, then there is not)
	for (u = 0; u < V; u++)
		for (v = 0; v < V; v++)
			rGraph[u][v] = graph[u][v];

	int parent[V];  // This array is filled by BFS and to store path

	int max_flow = 0;  // There is no flow initially

	// Augment the flow while there is path from source to sink
	while (bfs(V, rGraph, s, t, parent))
	{
		// Find minimum residual capacity of the edges along the
		// path filled by BFS. Or we can say find the maximum flow
		// through the path found.
		int path_flow = INT_MAX;
		for (v = t; v != s; v = parent[v])
		{
			u = parent[v];
			path_flow = min(path_flow, rGraph[u][v]);
		}

		// update residual capacities of the edges and reverse edges
		// along the path
		for (v = t; v != s; v = parent[v])
		{
			u = parent[v];
			rGraph[u][v] -= path_flow;
			rGraph[v][u] += path_flow;
		}

		// Add path flow to overall flow
		max_flow += path_flow;
	}

	// Return the overall flow
	return max_flow;
}
