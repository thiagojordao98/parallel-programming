#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Malha 3D de 100x100x100 (1.000.000 de células de fluido)
// Isso demanda bastante poder de fogo computacional, justificando o HPC
#define N 100
#define ITERS 500

int main() {
    double nu = 0.1;       // Viscosidade cinemática
    double dt = 1.0;       // Passo de tempo
    double dx = 1.0;       // Espaçamento da grade espacial

    // Otimização de Memória (Cache) para 3D:
    // Alocamos o cubo 3D como um vetor 1D contíguo.
    // Indexação matemática: array[i * N * N + j * N + k]
    double *u = (double*)malloc(N * N * N * sizeof(double));
    double *u_new = (double*)malloc(N * N * N * sizeof(double));

    if (u == NULL || u_new == NULL) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        return 1;
    }

    // 1. Inicializa o fluido parado em 3D
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                u[i * N * N + j * N + k] = 0.0;
                u_new[i * N * N + j * N + k] = 0.0;
            }
        }
    }

    // 2. Cria a perturbação 3D (uma esfera esférica no centro do cubo)
    int center = N / 2;
    int radius = N / 10;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                // Equação da esfera: (x-x0)^2 + (y-y0)^2 + (z-z0)^2 < r^2
                if ((i - center) * (i - center) + 
                    (j - center) * (j - center) + 
                    (k - center) * (k - center) < radius * radius) {
                    
                    u[i * N * N + j * N + k] = 100.0;
                    u_new[i * N * N + j * N + k] = 100.0;
                }
            }
        }
    }

    printf("Iniciando simulacao HPC 3D (Difusão)...\n");
    printf("Tamanho da malha: %d x %d x %d (%.2f milhões de células)\n", N, N, N, (N*N*N)/1000000.0);
    printf("Passos no tempo (Iterações): %d\n", ITERS);
    printf("Velocidade inicial no centro: %f\n\n", u[center * N * N + center * N + center]);

    double start_time = omp_get_wtime();

    // 3. Evolução no Tempo em 3D
    for (int t = 0; t < ITERS; t++) {
        
        // Com collapse(3), aplanamos o espaço tridimensional i, j, k 
        // em um único laço massivo (ideal para GPUs ou CPUs com muitos cores)
        #pragma omp parallel for collapse(3) schedule(runtime)
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                for (int k = 1; k < N - 1; k++) {
                    
                    int idx = i * N * N + j * N + k;
                    
                    // Aproximação do Laplaciano 3D (Derivadas parciais em X, Y e Z)
                    double laplacian = 
                        (u[(i + 1) * N * N + j * N + k] - 2.0 * u[idx] + u[(i - 1) * N * N + j * N + k]) / (dx * dx) +
                        (u[i * N * N + (j + 1) * N + k] - 2.0 * u[idx] + u[i * N * N + (j - 1) * N + k]) / (dx * dx) +
                        (u[i * N * N + j * N + (k + 1)] - 2.0 * u[idx] + u[i * N * N + j * N + (k - 1)]) / (dx * dx);
                    
                    // Atualiza a malha temporal no próximo passo
                    u_new[idx] = u[idx] + dt * nu * laplacian;
                }
            }
        }

        // Troca de ponteiros (sincronização O(1))
        double *temp = u;
        u = u_new;
        u_new = temp;
    }

    double end_time = omp_get_wtime();

    printf("Simulacao 3D concluída.\n");
    printf("Velocidade final no centro da esfera: %f\n", u[center * N * N + center * N + center]);
    printf("Tempo total de execucao: %f segundos\n", end_time - start_time);

    free(u);
    free(u_new);

    return 0;
}
