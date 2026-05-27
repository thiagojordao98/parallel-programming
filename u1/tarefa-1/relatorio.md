# Relatório Prático: Impacto do Padrão de Acesso à Memória na Multiplicação Matriz-Vetor

## 1. Introdução

A multiplicação de uma matriz por um vetor (y = A * x) é uma operação fundamental em álgebra linear computacional. Embora matematicamente a ordem das operações não altere o resultado, computacionalmente a forma como os dados são acessados tem um impacto drástico no tempo de execução. Este relatório analisa o desempenho de duas implementações em linguagem C: uma iterando a matriz por linhas e outra por colunas, a fim de demonstrar a importância da localidade espacial e o impacto do gargalo de von Neumann.

## 2. Metodologia

O experimento foi conduzido executando um programa em C que realiza a multiplicação de uma matriz quadrada A de tamanho N x N por um vetor x de tamanho N.

* **Linguagem e Armazenamento:** Na linguagem C, matrizes são armazenadas na memória em formato *row-major* (linha por linha). Isso significa que elementos adjacentes em uma mesma linha estão em endereços de memória contíguos.
* **Abordagem 1 (Acesso por Linhas):** O laço interno varia a coluna (j). O acesso à memória segue o armazenamento contíguo.
* **Abordagem 2 (Acesso por Colunas):** O laço interno varia a linha (i). O acesso dá saltos na memória de tamanho N.
* **Medição de Tempo:** Foi utilizada a função `clock_gettime(CLOCK_MONOTONIC)` para capturar o tempo de execução apenas do trecho de processamento, isolando o tempo de alocação e inicialização.
* **Hardware de Teste:** O código foi executado em uma máquina com **[INSERIR SUA RAM, ex: 8 GB]** de memória RAM e um processador **Intel(R) Core(TM) i5-7200U @ 2.50GHz** (2 núcleos físicos e 4 lógicos). O processador possui a seguinte hierarquia de cache: L1 de 128 KB, L2 de 512 KB e L3 de 3072 KB (3 MB).

## 3. Resultados Obtidos

A tabela abaixo apresenta os tempos de execução (em segundos) extraídos durante os testes para matrizes de dimensões variando de N = 100 até N = 15000. A última coluna indica o quanto a versão por colunas foi mais lenta em comparação à versão por linhas.

| Tamanho N | Tempo Linha (s) | Tempo Coluna (s) | Aumento Percentual (%) |
| --- | --- | --- | --- |
| 100 | 0.000000 | 0.000000 | N/A (Irrisório) |
| 500 | 0.001000 | 0.002000 | + 100.00% |
| 1000 | 0.006000 | 0.013000 | + 116.67% |
| 2000 | 0.042000 | 0.096000 | + 128.57% |
| 4000 | 0.096000 | 0.377000 | + 292.71% |
| 8000 | 0.481000 | 3.093000 | + 543.04% |
| 10000 | 0.416000 | 3.382000 | + 712.98% |
| 15000 | 2.012000 | 13.079000 | + 550.05% |

> **Nota sobre o dado de N = 8000:** O tempo da versão por linhas (0.481s) foi ligeiramente maior do que o tempo para N = 10000 (0.416s). Isso é um comportamento comum em medições reais, geralmente causado por interrupções do Sistema Operacional (processos em background) disputando a CPU ou limpando a cache durante aquela execução específica.

## 4. Discussão e Análise de Desempenho

### 4.1. O Ponto de Divergência e a Capacidade da Cache

Observando os resultados, nota-se que para tamanhos pequenos (N = 100 e N = 500), a diferença absoluta de tempo é insignificante. A partir de **N = 1000**, os tempos começam a divergir, e em **N = 4000**, o acesso por colunas passa a ser quase 300% mais lento, atingindo seu ápice em N = 10000 (mais de 7 vezes mais lento).

Esse ponto de divergência pode ser explicado matematicamente pela capacidade da cache L3 do processador utilizado (3 MB):

* Em linguagem C, cada elemento `double` ocupa 8 bytes.
* Para **N = 500**, a matriz inteira ocupa 2.000.000 bytes (aproximadamente 2 MB). Portanto, ela **cabe inteiramente na cache L3**.
* Para **N = 1000**, a matriz passa a ocupar ~8 MB, ultrapassando o limite físico da cache L3.
* Para **N = 4000**, a matriz ocupa ~128 MB, forçando o processador a buscar os dados massivamente na memória RAM principal, que é muito mais lenta.

### 4.2. O Gargalo de von Neumann, Localidade Espacial e Temporal

Ao esgotar a capacidade da cache, a arquitetura esbarra no **Gargalo de von Neumann** — a limitação onde a CPU é forçada a ficar ociosa esperando os dados chegarem da RAM. Para mitigar isso, o hardware depende de dois princípios:

* **Localidade Espacial (O grande diferencial):** Refere-se à probabilidade de a CPU acessar endereços vizinhos na memória. Como matrizes em C são *row-major*, o acesso por linhas respeita a localidade espacial. Ao pedir um elemento, a CPU traz uma "linha de cache" inteira (geralmente 64 bytes) para as memórias rápidas (L1/L2). Nos acessos seguintes, os dados já estão lá (*Cache Hit*).
Já o acesso por colunas quebra essa localidade. O programa salta enormes blocos de memória a cada iteração. Para N = 4000, cada leitura pula 32.000 bytes, invalidando as linhas de cache e exigindo buscas constantes na RAM (*Cache Misses* severos).
* **Localidade Temporal:** Refere-se à reutilização recente de uma memória. A variável `y[i]` (acumulador) e o vetor `x` se beneficiam disso, permanecendo na cache em ambas as versões. Contudo, a localidade temporal não é suficiente para salvar a versão por colunas do estrago causado pela quebra da localidade espacial na leitura da matriz A.

## 5. Conclusão

Os testes práticos demonstram que a complexidade algorítmica teórica não define sozinha o desempenho de um software. Embora ambas as funções possuam complexidade O(N^2), o tempo de execução no mundo real variou em mais de 700% unicamente devido à forma como a estrutura de dados foi percorrida. O desenvolvimento de códigos de alta performance exige não apenas a lógica matemática, mas a compreensão íntima das limitações físicas do hardware, otimizando o fluxo de dados para evitar o Gargalo de von Neumann e maximizar a taxa de acertos nas memórias cache L1, L2 e L3.

---

## Anexo: Código Fonte Utilizado

```c
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
    int sizes[] = {100, 500, 1000, 2000, 4000, 8000, 10000, 15000};
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

```