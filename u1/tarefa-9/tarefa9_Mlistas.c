#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

void insert(Node** head, int val) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = val;
    new_node->next = *head;
    *head = new_node;
}

// Função para contar os elementos e liberar a memória de uma lista
int count_and_free(Node** head) {
    int count = 0;
    Node* current = *head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
        count++;
    }
    *head = NULL;
    return count;
}

int main(int argc, char* argv[]) {
    int N = 1000000; // Aumentado para 1.000.000 para medição de tempo perceptível
    int M = 5; // M = 5 listas por padrão, ajustável pelo usuário

    if (argc > 1) {
        M = strtol(argv[1], NULL, 10);
    }

    // Vetores dinâmicos: um para as listas e outro para os locks individuais
    Node** lists = (Node**)calloc(M, sizeof(Node*));
    omp_lock_t* locks = (omp_lock_t*)malloc(M * sizeof(omp_lock_t));

    // Inicializa um lock independente para cada lista iterativamente
    for (int i = 0; i < M; i++) {
        omp_init_lock(&locks[i]);
    }

    printf("Iniciando inserção dinâmica em %d listas (%d inserções)...\n", M, N);

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < N; i++) {
                #pragma omp task firstprivate(i)
                {
                    unsigned int seed = omp_get_thread_num() + time(NULL) + i;
                    int target_list = rand_r(&seed) % M; // Sorteia o alvo dinamicamente

                    // Bloqueia APENAS a lista específica selecionada em tempo de execução
                    omp_set_lock(&locks[target_list]);
                    
                    insert(&lists[target_list], i);
                    
                    // Libera o lock daquela lista
                    omp_unset_lock(&locks[target_list]);
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    double exec_time = end_time - start_time;

    // Destruição das variáveis de lock para liberar recursos do sistema
    for (int i = 0; i < M; i++) {
        omp_destroy_lock(&locks[i]);
    }

    // Contagem e liberação das listas
    int total = 0;
    for (int i = 0; i < M; i++) {
        int count = count_and_free(&lists[i]);
        printf("Lista %d: %d elementos\n", i, count);
        total += count;
    }
    
    free(lists);
    free(locks);

    printf("Total inserido: %d de %d esperados (Status: %s)\n", 
           total, N, (total == N) ? "OK (Sem perda de dados)" : "FALHA (Condição de corrida!)");
    printf("Tempo de execução: %.4f segundos\n", exec_time);

    return 0;
}
