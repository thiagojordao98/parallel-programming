#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main(int argc, char *argv[]) {
    long long num_trials = 100000000; // Número padrão de iterações
    long long points_inside = 0;

    // Permite passar o número de iterações como argumento
    if (argc > 1) {
        num_trials = atoll(argv[1]);
    }

    double start_time = omp_get_wtime();

    // Região paralela
    #pragma omp parallel
    {
        // Variável "seed" local para cada thread garantir que a geração de
        // números aleatórios seja thread-safe e não cause gargalos
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)omp_get_thread_num();
        
        // Distribuição do loop entre as threads e soma paralela na variável points_inside
        #pragma omp for
        for (long long i = 0; i < num_trials; i++) {
            // Gera coordenadas aleatórias x e y entre 0 e 1
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            // Verifica se o ponto caiu dentro do quarto de círculo (x^2 + y^2 <= 1)
            if (x * x + y * y <= 1.0) {
                points_inside++;
            }
        }
    }

    // Calcula a estimativa de Pi
    double pi_estimate = 4.0 * (double)points_inside / (double)num_trials;
    
    double end_time = omp_get_wtime();

    printf("Número de tentativas: %lld\n", num_trials);
    printf("Pontos dentro do círculo: %lld\n", points_inside);
    printf("Estimativa de Pi: %f\n", pi_estimate);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    return 0;
}
