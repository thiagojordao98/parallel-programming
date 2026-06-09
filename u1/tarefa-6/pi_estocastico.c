#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 10000000

int main() {
    int count;
    double pi;
    double start, end;

    printf("Estimativa Estocastica de Pi (N = %d)\n\n", N);

    // 1. Race Condition
    count = 0;
    start = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        unsigned int seed = i;
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            count++; // Condicao de corrida
        }
    }
    end = omp_get_wtime();
    pi = 4.0 * count / N;
    printf("1. Com Condicao de Corrida (#pragma omp parallel for):\n");
    printf("   Pi estimado: %f (Contagem: %d)\n", pi, count);
    printf("   Tempo: %f segundos\n\n", end - start);

    // 2. Correcao com omp critical
    count = 0;
    start = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        unsigned int seed = i;
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            #pragma omp critical
            {
                count++;
            }
        }
    }
    end = omp_get_wtime();
    pi = 4.0 * count / N;
    printf("2. Correcao com #pragma omp critical (Sem otimizacao local):\n");
    printf("   Pi estimado: %f (Contagem: %d)\n", pi, count);
    printf("   Tempo: %f segundos\n\n", end - start);

    // 3. Reestruturacao com omp parallel, omp for e clausulas
    count = 0;
    int shared_var = 0;
    int first_priv_var = 100;
    int last_priv_var = -1;

    start = omp_get_wtime();
    #pragma omp parallel default(none) shared(count, shared_var, last_priv_var) firstprivate(first_priv_var)
    {
        int local_count = 0;
        double x, y;
        unsigned int seed = omp_get_thread_num();
        
        #pragma omp for lastprivate(last_priv_var) private(x, y)
        for (int i = 0; i < N; i++) {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0) {
                local_count++;
            }
            if (i == N - 1) {
                last_priv_var = omp_get_thread_num();
            }
        }
        
        #pragma omp critical
        {
            count += local_count;
            shared_var += first_priv_var;
        }
    }
    end = omp_get_wtime();
    pi = 4.0 * count / N;
    printf("3. Reestruturado com Clausulas de Escopo:\n");
    printf("   Pi estimado: %f (Contagem: %d)\n", pi, count);
    printf("   Tempo: %f segundos\n", end - start);
    printf("   Testando clausulas:\n");
    printf("   - shared_var (mutada): %d (esperado: 100 * num_threads)\n", shared_var);
    printf("   - first_priv_var (externo): %d (esperado: 100)\n", first_priv_var);
    printf("   - last_priv_var (ID da thread da ultima iteracao): %d\n\n", last_priv_var);

    return 0;
}
