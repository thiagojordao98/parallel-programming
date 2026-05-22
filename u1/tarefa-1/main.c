#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Versão 1: Acesso por linhas (laço interno variando coluna j)
void mxv_row(double *A, double *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        y[i] = 0.0;
        for (int j = 0; j < n; j++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
}

// Versão 2: Acesso por colunas (laço interno variando linha i)
void mxv_col(double *A, double *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        y[i] = 0.0;
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
}

int main() {
    int sizes[] = {100, 500, 1000, 2000, 4000, 8000, 10000, 15000, 20000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%-10s %-15s %-15s\n", "Tamanho N", "Tempo Linha (s)", "Tempo Coluna (s)");
    printf("---------------------------------------------\n");

    for (int k = 0; k < num_sizes; k++) {
        int n = sizes[k];
        
        // Alocando como array 1D contínuo para a matriz NxN
        double *A = (double*) malloc((size_t)n * n * sizeof(double));
        double *x = (double*) malloc((size_t)n * sizeof(double));
        double *y = (double*) malloc((size_t)n * sizeof(double));

        if (!A || !x || !y) {
            printf("Erro de alocação de memória para N = %d\n", n);
            if (A) free(A);
            if (x) free(x);
            if (y) free(y);
            break;
        }

        // Inicialização
        for (int i = 0; i < n; i++) {
            x[i] = 1.0;
            for (int j = 0; j < n; j++) {
                A[i * n + j] = 1.0;
            }
        }

        struct timespec start, end;
        double time_row, time_col;

        // Medição acesso por linhas
        clock_gettime(CLOCK_MONOTONIC, &start);
        mxv_row(A, x, y, n);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_row = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

        // Medição acesso por colunas
        clock_gettime(CLOCK_MONOTONIC, &start);
        mxv_col(A, x, y, n);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_col = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

        printf("%-10d %-15.6f %-15.6f\n", n, time_row, time_col);

        free(A);
        free(x);
        free(y);
    }

    return 0;
}
