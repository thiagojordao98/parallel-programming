#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define TOSSES 50000000

int main() {
    int thread_count = 4;
    double start, finish;
    omp_set_num_threads(thread_count);

    printf("Simulacao de PI com %d tosses e %d threads.\n\n", TOSSES, thread_count);

    // =========================================================================
    // VERSÃO 1: Contador Compartilhado + CRITICAL no laço
    // =========================================================================
    long long hits_v1 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num() + time(NULL);
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) {
                #pragma omp critical
                hits_v1++;
            }
        }
    }
    finish = omp_get_wtime();
    printf("V1 (Compartilhado + critical) | Pi: %.5f | Tempo: %f s\n", 4.0 * hits_v1 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 2: Contador Compartilhado + ATOMIC no laço
    // =========================================================================
    long long hits_v2 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num() + time(NULL);
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) {
                #pragma omp atomic
                hits_v2++;
            }
        }
    }
    finish = omp_get_wtime();
    printf("V2 (Compartilhado + atomic)   | Pi: %.5f | Tempo: %f s\n", 4.0 * hits_v2 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 3: Contador Privado + CRITICAL (Apenas no final)
    // =========================================================================
    long long hits_v3 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        long long local_hits = 0;
        unsigned int seed = omp_get_thread_num() + time(NULL);
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) local_hits++;
        }
        #pragma omp critical
        hits_v3 += local_hits;
    }
    finish = omp_get_wtime();
    printf("V3 (Privado + critical)       | Pi: %.5f | Tempo: %f s\n", 4.0 * hits_v3 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 4: Contador Privado + ATOMIC (Apenas no final)
    // =========================================================================
    long long hits_v4 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        long long local_hits = 0;
        unsigned int seed = omp_get_thread_num() + time(NULL);
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) local_hits++;
        }
        #pragma omp atomic
        hits_v4 += local_hits;
    }
    finish = omp_get_wtime();
    printf("V4 (Privado + atomic)         | Pi: %.5f | Tempo: %f s\n", 4.0 * hits_v4 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 5: Cláusula REDUCTION (Mais produtiva e performática)
    // =========================================================================
    long long hits_v5 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num() + time(NULL);
        #pragma omp for reduction(+:hits_v5)
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) hits_v5++;
        }
    }
    finish = omp_get_wtime();
    printf("V5 (Clausula reduction)       | Pi: %.5f | Tempo: %f s\n", 4.0 * hits_v5 / TOSSES, finish - start);

    return 0;
}
