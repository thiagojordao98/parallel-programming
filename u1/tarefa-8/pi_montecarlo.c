#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define TOSSES 50000000

int main(int argc, char* argv[]) {
    int thread_count = 4; // Número de threads
    double start, finish;
    
    omp_set_num_threads(thread_count);
    printf("Simulacao com %d tosses e %d threads.\n\n", TOSSES, thread_count);

    // =========================================================================
    // VERSÃO 1: rand() + Variável Privada + Critical
    // =========================================================================
    long long global_hits_v1 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        long long local_hits = 0;
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) local_hits++;
        }
        #pragma omp critical
        global_hits_v1 += local_hits;
    }
    finish = omp_get_wtime();
    printf("V1 (rand   + critical)     | Pi: %.5f | Tempo: %f s\n", 
           4.0 * global_hits_v1 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 2: rand() + Vetor Compartilhado (Falso Compartilhamento)
    // =========================================================================
    long long* shared_hits_v2 = (long long*)calloc(thread_count, sizeof(long long));
    start = omp_get_wtime();
    #pragma omp parallel
    {
        int my_rank = omp_get_thread_num();
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) shared_hits_v2[my_rank]++; // Atualiza diretamente no vetor
        }
    }
    long long global_hits_v2 = 0;
    for (int i = 0; i < thread_count; i++) global_hits_v2 += shared_hits_v2[i];
    finish = omp_get_wtime();
    printf("V2 (rand   + shared array) | Pi: %.5f | Tempo: %f s\n", 
           4.0 * global_hits_v2 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 3: rand_r() + Variável Privada + Critical
    // =========================================================================
    long long global_hits_v3 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        long long local_hits = 0;
        unsigned int seed = (unsigned int)omp_get_thread_num() + time(NULL);
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) local_hits++;
        }
        #pragma omp critical
        global_hits_v3 += local_hits;
    }
    finish = omp_get_wtime();
    printf("V3 (rand_r + critical)     | Pi: %.5f | Tempo: %f s\n", 
           4.0 * global_hits_v3 / TOSSES, finish - start);

    // =========================================================================
    // VERSÃO 4: rand_r() + Vetor Compartilhado (Falso Compartilhamento isolado)
    // =========================================================================
    long long* shared_hits_v4 = (long long*)calloc(thread_count, sizeof(long long));
    start = omp_get_wtime();
    #pragma omp parallel
    {
        int my_rank = omp_get_thread_num();
        unsigned int seed = (unsigned int)my_rank + time(NULL);
        #pragma omp for
        for (long long i = 0; i < TOSSES; i++) {
            double x = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            double y = ((double)rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
            if (x*x + y*y <= 1.0) shared_hits_v4[my_rank]++; // Falso compartilhamento
        }
    }
    long long global_hits_v4 = 0;
    for (int i = 0; i < thread_count; i++) global_hits_v4 += shared_hits_v4[i];
    finish = omp_get_wtime();
    printf("V4 (rand_r + shared array) | Pi: %.5f | Tempo: %f s\n", 
           4.0 * global_hits_v4 / TOSSES, finish - start);

    free(shared_hits_v2);
    free(shared_hits_v4);
    return 0;
}
