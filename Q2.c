#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_VALUE 100  // max possible value in input
#define NUM_THREADS 4

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    int input[] = {5, 2, 7, 2, 9, 1, 5, 3, 2, 7, 0, 9, 2, 1, 5, 8};
    int n = sizeof(input) / sizeof(input[0]);

    int global_hist[MAX_VALUE + 1] = {0}; // Global histogram
    int thread_hist[NUM_THREADS][MAX_VALUE + 1]; // Local histograms for threads

    // Initialize thread histograms
    for (int t = 0; t < NUM_THREADS; t++)
        for (int i = 0; i <= MAX_VALUE; i++)
            thread_hist[t][i] = 0;

    omp_set_num_threads(NUM_THREADS);

    // Step 1: Parallel histogram generation
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int chunk_size = (n + NUM_THREADS - 1) / NUM_THREADS;
        int start = tid * chunk_size;
        int end = (start + chunk_size > n) ? n : start + chunk_size;

        for (int i = start; i < end; i++) {
            int val = input[i];
            thread_hist[tid][val]++;
        }
    }

    // Step 2: Combine thread histograms into global histogram
    for (int t = 0; t < NUM_THREADS; t++) {
        for (int i = 0; i <= MAX_VALUE; i++) {
            global_hist[i] += thread_hist[t][i];
        }
    }

    // Step 3: Reconstruct the sorted array from histogram
    int sorted_index = 0;
    for (int i = 0; i <= MAX_VALUE; i++) {
        for (int count = 0; count < global_hist[i]; count++) {
            input[sorted_index++] = i;
        }
    }

    // Output sorted array
    printf("Sorted array:\n");
    print_array(input, n);

    return 0;
}
