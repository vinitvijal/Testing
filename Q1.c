#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 500  // Size of the matrices

int main() {
    double A[N][N], B[N][N], C[N][N];

    // Initialize A and B
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }

    // Parallelize only the outermost loop
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return 0;
}
