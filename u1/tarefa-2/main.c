#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000000  // 100 milhões de elementos

// ============================================================
// Laço 1 – Inicialização independente (sem dependência)
//   Cada iteração calcula v[i] de forma independente.
//   O processador pode aplicar pipeline e executar várias
//   iterações simultaneamente (alto ILP).
// ============================================================
void laco_independente(double *v, int n) {
    for (int i = 0; i < n; i++) {
        v[i] = (double)i * 0.5 + 1.0;
    }
}

// ============================================================
// Laço 2 – Soma acumulativa (com dependência entre iterações)
//   Cada iteração depende do resultado da anterior:
//     soma = soma + v[i]
//   Isso cria uma cadeia de dependência (RAW – Read After Write)
//   que impede o pipeline de executar a próxima soma antes
//   da anterior terminar. O ILP é baixo.
// ============================================================
double laco_dependente(double *v, int n) {
    double soma = 0.0;
    for (int i = 0; i < n; i++) {
        soma += v[i];
    }
    return soma;
}

// ============================================================
// Laço 3 – Soma com múltiplos acumuladores (quebrando dependência)
//   Usa 4 variáveis independentes (soma0..soma3) para acumular
//   parcelas diferentes. Cada acumulador forma uma cadeia
//   independente, permitindo que o processador as execute em
//   paralelo nos estágios do pipeline (alto ILP).
//   No final, as parcelas são combinadas.
// ============================================================
double laco_multi_acumulador(double *v, int n) {
    double soma0 = 0.0, soma1 = 0.0, soma2 = 0.0, soma3 = 0.0;

    // Processa 4 elementos por iteração
    int limite = n - (n % 4);
    for (int i = 0; i < limite; i += 4) {
        soma0 += v[i];
        soma1 += v[i + 1];
        soma2 += v[i + 2];
        soma3 += v[i + 3];
    }

    // Processa elementos restantes
    for (int i = limite; i < n; i++) {
        soma0 += v[i];
    }

    return soma0 + soma1 + soma2 + soma3;
}

// Calcula o tempo em segundos entre dois timespec
static double tempo_decorrido(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) * 1e-9;
}

int main() {
    printf("=============================================================\n");
    printf("  Investigação de Paralelismo ao Nível de Instrução (ILP)\n");
    printf("  Tamanho do vetor: %d elementos\n", N);
    printf("=============================================================\n\n");

    double *v = (double *)malloc((size_t)N * sizeof(double));
    if (!v) {
        fprintf(stderr, "Erro: falha na alocação de memória.\n");
        return 1;
    }

    struct timespec inicio, fim;
    double t1, t2, t3;

    // ----------------------------------------------------------
    // Laço 1 – Inicialização independente
    // ----------------------------------------------------------
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    laco_independente(v, N);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    t1 = tempo_decorrido(inicio, fim);

    printf("Laço 1 – Inicialização independente\n");
    printf("  Tempo: %.6f s\n\n", t1);

    // ----------------------------------------------------------
    // Laço 2 – Soma com dependência (acumulador único)
    // ----------------------------------------------------------
    volatile double resultado_dep;  // volatile evita otimização de dead-code
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    resultado_dep = laco_dependente(v, N);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    t2 = tempo_decorrido(inicio, fim);

    printf("Laço 2 – Soma dependente (acumulador único)\n");
    printf("  Resultado: %.2f\n", resultado_dep);
    printf("  Tempo: %.6f s\n\n", t2);

    // ----------------------------------------------------------
    // Laço 3 – Soma com múltiplos acumuladores
    // ----------------------------------------------------------
    volatile double resultado_multi;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    resultado_multi = laco_multi_acumulador(v, N);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    t3 = tempo_decorrido(inicio, fim);

    printf("Laço 3 – Soma com múltiplos acumuladores\n");
    printf("  Resultado: %.2f\n", resultado_multi);
    printf("  Tempo: %.6f s\n\n", t3);

    // ----------------------------------------------------------
    // Resumo comparativo
    // ----------------------------------------------------------
    printf("=============================================================\n");
    printf("  RESUMO COMPARATIVO\n");
    printf("=============================================================\n");
    printf("  %-42s %10.6f s\n", "Laço 1 (independente – inicialização):", t1);
    printf("  %-42s %10.6f s\n", "Laço 2 (dependente – acumulador único):", t2);
    printf("  %-42s %10.6f s\n", "Laço 3 (multi-acumulador – sem dep.):", t3);
    printf("-------------------------------------------------------------\n");

    if (t3 > 0.0) {
        printf("  Speedup Laço 3 vs Laço 2: %.2fx\n", t2 / t3);
    }
    printf("=============================================================\n");

    free(v);
    return 0;
}
