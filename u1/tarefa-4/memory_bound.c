#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_N 100000000
#define DEFAULT_MAX_THREADS 8

static double seconds_since(double start) {
    return omp_get_wtime() - start;
}

static void fill_vectors(double *a, double *b, double *c, size_t n) {
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        a[i] = (double)(i % 100) * 0.25;
        b[i] = (double)(i % 50) * 0.5;
        c[i] = 0.0;
    }
}

static double vector_sum(double *a, double *b, double *c, size_t n) {
    double checksum = 0.0;

    #pragma omp parallel for reduction(+:checksum)
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
        checksum += c[i];
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

    double *a = (double *)malloc(n * sizeof(double));
    double *b = (double *)malloc(n * sizeof(double));
    double *c = (double *)malloc(n * sizeof(double));
    if (!a || !b || !c) {
        fprintf(stderr, "Erro: falha na alocacao de memoria.\n");
        free(a);
        free(b);
        free(c);
        return 1;
    }

    int threads = omp_get_max_threads();
    printf("Benchmark memory-bound: soma de vetores\n");
    printf("Tamanho: %llu\n", (unsigned long long)n);
    printf("Threads: %d\n", threads);

    fill_vectors(a, b, c, n);

    double start = omp_get_wtime();
    double checksum = vector_sum(a, b, c, n);
    double elapsed = seconds_since(start);

    printf("Tempo(s): %.6f  Soma: %.2f\n", elapsed, checksum);

    free(a);
    free(b);
    free(c);
    return 0;
}
