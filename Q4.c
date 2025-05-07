#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#define V 6  // number of vertices
#define INF INT_MAX

int minDistance(int dist[], int visited[]) {
    int min = INF, min_index = -1;

    #pragma omp parallel
    {
        int local_min = INF;
        int local_index = -1;

        #pragma omp for nowait
        for (int v = 0; v < V; v++) {
            if (!visited[v] && dist[v] < local_min) {
                local_min = dist[v];
                local_index = v;
            }
        }

        #pragma omp critical
        {
            if (local_min < min) {
                min = local_min;
                min_index = local_index;
            }
        }
    }

    return min_index;
}

void dijkstra_parallel(int graph[V][V], int src) {
    int dist[V];     // Shortest distances
    int visited[V];  // Visited vertices

    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, visited);
        if (u == -1) break;
        visited[u] = 1;

        // Parallel relaxation step
        #pragma omp parallel for
        for (int v = 0; v < V; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                #pragma omp critical
                {
                    if (dist[u] + graph[u][v] < dist[v]) {
                        dist[v] = dist[u] + graph[u][v];
                    }
                }
            }
        }
    }

    // Output distances
    printf("Vertex \t Distance from Source\n");
    for (int i = 0; i < V; i++)
        printf("%d \t %d\n", i, dist[i]);
}

int main() {
    int graph[V][V] = {
        {0, 4, 0, 0, 0, 0},
        {4, 0, 8, 0, 0, 0},
        {0, 8, 0, 7, 0, 4},
        {0, 0, 7, 0, 9, 14},
        {0, 0, 0, 9, 0, 10},
        {0, 0, 4, 14, 10, 0}
    };

    omp_set_num_threads(4); // Set number of threads
    dijkstra_parallel(graph, 0);

    return 0;
}