#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 1024
#define M 512

#define HEAVY_ATTR __attribute__((optimize("-fbranch-probabilities")))
// #define HEAVY_ATTR

void HEAVY_ATTR heavy_compute(float threshold) {
    float *a = aligned_alloc(32, N * sizeof(float));
    float *b = aligned_alloc(32, N * sizeof(float));
    float *c = aligned_alloc(32, N * sizeof(float));
    float *d = aligned_alloc(32, N * sizeof(float));
    float *matrix = aligned_alloc(32, N * M * sizeof(float));
    float sum = 0.0f;

    for (int i = 0; i < N; i++) {
        a[i] = i * 0.5f;
        b[i] = N - i;
        c[i] = sinf(i * 0.01f);
        d[i] = 0.0f;
    }

    // Loop nest with condition and multiple patterns
    for (int i = 0; i < N; i++) {
        if (threshold > 100.0f) {
            for (int j = 0; j < M; j++) {
                matrix[i * M + j] = a[i] * b[i] + j * 0.001f;
            }
        } else {
            for (int j = 0; j < M; j++) {
                matrix[i * M + j] = c[i] * j - b[i] * 0.002f;
            }
        }
    }

    // Vectorization target
    for (int i = 0; i < N; i++) {
        d[i] = a[i] * b[i] + c[i] * 2.0f;
    }

    // Reduction with dependency — partial vectorization
    for (int i = 0; i < N; i++) {
        sum += d[i];
    }

    // Peel/unroll candidate
    for (int i = 0; i < N - 3; i += 4) {
        d[i] += a[i+1] - c[i+2];
        d[i+1] += b[i+2];
        d[i+2] += a[i+3];
        d[i+3] += c[i];
    }

    printf("checksum: %f\n", sum);

    free(a);
    free(b);
    free(c);
    free(d);
    free(matrix);
}

int main() {
    heavy_compute(150.0f);  // Also try with 50.0f to trigger branch paths
    return 0;
}
