#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_N 20000000
#define DEFAULT_MAX_THREADS 8

static double heavy_math(double x) {
    double value = x;

    for (int k = 0; k < 40; k++) {
        value = sin(value) * cos(value) + sqrt(fabs(value) + 1.0);
        value = value * 0.999999 + 0.000001 * x;
    }

    return value;
}

static double compute_kernel(double *input, size_t n) {
    double checksum = 0.0;

    #pragma omp parallel for reduction(+:checksum)
    for (size_t i = 0; i < n; i++) {
        double x = input[i];
        double result = heavy_math(x);
        checksum += result;
    }

    return checksum;
}

int main(int argc, char **argv) {
    size_t n = DEFAULT_N;
    if (argc > 1) {
        n = (size_t)strtoull(argv[1], NULL, 10);
    }
    if (argc > 2) {
        int nt = atoi(argv[2]);
        if (nt > 0) {
            omp_set_num_threads(nt);
        }
    }
    if (n == 0) {
        fprintf(stderr, "Uso: %s [tamanho] [num_threads]\n", argv[0]);
        return 1;
    }

    double *input = (double *)malloc(n * sizeof(double));
    if (!input) {
        fprintf(stderr, "Erro: falha na alocacao de memoria.\n");
        return 1;
    }

    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        input[i] = (double)(i % 1000) * 0.001 + 0.5;
    }

    int threads = omp_get_max_threads();
    printf("Benchmark compute-bound: calculo matematico intensivo\n");
    printf("Tamanho: %llu\n", (unsigned long long)n);
    printf("Threads: %d\n", threads);

    double start = omp_get_wtime();
    double checksum = compute_kernel(input, n);
    double elapsed = omp_get_wtime() - start;

    printf("Tempo(s): %.6f  Soma: %.2f\n", elapsed, checksum);

    free(input);
    return 0;
}
