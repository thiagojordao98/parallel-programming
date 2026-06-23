#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// Estrutura do nó da lista encadeada
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Função de inserção atômica lógica no início da lista
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

int main() {
    int N = 1000000; // Aumentado para 1.000.000 para medição de tempo perceptível
    Node* head0 = NULL;
    Node* head1 = NULL;

    printf("Iniciando inserção em 2 listas usando tasks (%d inserções)...\n", N);

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < N; i++) {
                #pragma omp task firstprivate(i)
                {
                    unsigned int seed = omp_get_thread_num() + time(NULL) + i;
                    int target_list = rand_r(&seed) % 2;

                    if (target_list == 0) {
                        #pragma omp critical(lista0)
                        {
                            insert(&head0, i);
                        }
                    } else {
                        #pragma omp critical(lista1)
                        {
                            insert(&head1, i);
                        }
                    }
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    double exec_time = end_time - start_time;

    int count0 = count_and_free(&head0);
    int count1 = count_and_free(&head1);
    int total = count0 + count1;

    printf("Inserções concluídas.\n");
    printf("Lista 0: %d elementos\n", count0);
    printf("Lista 1: %d elementos\n", count1);
    printf("Total inserido: %d de %d esperados (Status: %s)\n", 
           total, N, (total == N) ? "OK (Sem perda de dados)" : "FALHA (Condição de corrida!)");
    printf("Tempo de execução: %.4f segundos\n", exec_time);

    return 0;
}
