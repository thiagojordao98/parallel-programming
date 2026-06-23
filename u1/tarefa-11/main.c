#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Parâmetros físicos e de simulação
// Aumentamos o tamanho da malha e o número de iterações em relação ao Python
// para que o overhead de criação de threads do OpenMP seja superado pelo
// ganho de desempenho computacional na paralelização.
#define N 500
#define ITERS 1000

int main() {
    double nu = 0.1;       // Viscosidade cinemática
    double dt = 1.0;       // Passo de tempo
    double dx = 1.0;       // Espaçamento da grade espacial

    // Otimização de Memória (Cache):
    // Alocamos a malha 2D (N x N) como um vetor 1D contíguo.
    // Isso garante localidade espacial contígua em C (Row-Major), minimizando
    // cache misses quando percorremos os elementos em u[i * N + j].
    double *u = (double*)malloc(N * N * sizeof(double));
    double *u_new = (double*)malloc(N * N * sizeof(double));

    if (u == NULL || u_new == NULL) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        return 1;
    }

    // 1. Inicializa o fluido parado (velocidade 0)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            u[i * N + j] = 0.0;
            u_new[i * N + j] = 0.0;
        }
    }

    // 2. Cria a perturbação no centro do domínio
    int center = N / 2;
    int radius = N / 10;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Verifica se a célula está dentro de um raio a partir do centro
            if ((i - center) * (i - center) + (j - center) * (j - center) < radius * radius) {
                u[i * N + j] = 100.0;
                u_new[i * N + j] = 100.0; // Garante mesma inicialização
            }
        }
    }

    printf("Iniciando simulacao HPC (Navier-Stokes simplificado - Difusão)...\n");
    printf("Tamanho da malha: %d x %d\n", N, N);
    printf("Passos no tempo (Iterações): %d\n", ITERS);
    printf("Velocidade inicial no centro: %f\n\n", u[center * N + center]);

    // Medição de tempo (HPC)
    double start_time = omp_get_wtime();

    // 3. Evolução no Tempo (Diferenças Finitas - Esquema FTCS)
    for (int t = 0; t < ITERS; t++) {
        
        // Paralelização OpenMP com otimização de granularidade e particionamento:
        // - collapse(2): Achata os dois for aninhados, criando um único espaço de iteração maior,
        //   reduzindo o overhead por thread e maximizando paralelismo na distribuição.
        // - schedule(runtime): Permite alterar o escalonador via variável de ambiente 
        //   (e.g., OMP_SCHEDULE="static", OMP_SCHEDULE="dynamic, 100") em runtime, 
        //   sem necessidade de recompilar.
        #pragma omp parallel for collapse(2) schedule(runtime)
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                
                // Cálculo do Laplaciano espacial com array 1D
                double laplacian = (u[(i + 1) * N + j] - 2.0 * u[i * N + j] + u[(i - 1) * N + j]) / (dx * dx) +
                                   (u[i * N + j + 1] - 2.0 * u[i * N + j] + u[i * N + j - 1]) / (dx * dx);
                
                // Atualiza a malha temporal no próximo passo da difusão de velocidade
                u_new[i * N + j] = u[i * N + j] + dt * nu * laplacian;
            }
        }

        // Sincroniza a matriz para o próximo passo temporal.
        // Em vez de fazer uma cópia O(N^2) (como np.copy no Python),
        // trocamos apenas os ponteiros. Operação O(1), poupando I/O pesado de memória.
        double *temp = u;
        u = u_new;
        u_new = temp;
    }

    double end_time = omp_get_wtime();

    // 4. Saída de dados validando a difusão sem gargalar iteração no tempo
    printf("Simulacao concluída.\n");
    printf("Velocidade final no centro (deve ser menor, dissipou energia): %f\n", u[center * N + center]);
    printf("Tempo total de execucao: %f segundos\n", end_time - start_time);

    free(u);
    free(u_new);

    return 0;
}
