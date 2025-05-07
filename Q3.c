#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_NODES 1000

typedef struct {
    int* neighbors;
    int degree;
} Node;

void bfs_parallel(Node* graph, int num_nodes, int start) {
    int visited[MAX_NODES] = {0};
    int frontier[MAX_NODES];
    int next_frontier[MAX_NODES];
    int frontier_size = 0, next_frontier_size = 0;

    frontier[frontier_size++] = start;
    visited[start] = 1;

    printf("BFS traversal: ");

    while (frontier_size > 0) {
        next_frontier_size = 0;

        #pragma omp parallel for shared(visited, graph, frontier, next_frontier) default(none) firstprivate(frontier_size)
        for (int i = 0; i < frontier_size; i++) {
            int u = frontier[i];
            printf("%d ", u);

            for (int j = 0; j < graph[u].degree; j++) {
                int v = graph[u].neighbors[j];

                // Atomic check and mark to avoid race condition
                if (!visited[v]) {
                    int was_set = 0;
                    #pragma omp critical
                    {
                        if (!visited[v]) {
                            visited[v] = 1;
                            next_frontier[next_frontier_size++] = v;
                            was_set = 1;
                        }
                    }
                }
            }
        }

        // Move to next frontier
        for (int i = 0; i < next_frontier_size; i++) {
            frontier[i] = next_frontier[i];
        }
        frontier_size = next_frontier_size;
    }

    printf("\n");
}

int main() {
    int num_nodes = 6;

    Node graph[MAX_NODES];

    // Initialize graph (undirected)
    for (int i = 0; i < num_nodes; i++) {
        graph[i].neighbors = malloc(num_nodes * sizeof(int));
        graph[i].degree = 0;
    }

    // Add edges (example graph)
    int edges[][2] = {
        {0, 1}, {0, 2}, {1, 3},
        {1, 4}, {2, 4}, {3, 5}, {4, 5}
    };
    int num_edges = sizeof(edges) / sizeof(edges[0]);

    for (int i = 0; i < num_edges; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        graph[u].neighbors[graph[u].degree++] = v;
        graph[v].neighbors[graph[v].degree++] = u;
    }

    // Run BFS from node 0
    bfs_parallel(graph, num_nodes, 0);

    // Cleanup
    for (int i = 0; i < num_nodes; i++)
        free(graph[i].neighbors);

    return 0;
}
