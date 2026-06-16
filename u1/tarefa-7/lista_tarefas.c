#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Estrutura do nó da lista encadeada
typedef struct Node {
    char filename[256];
    struct Node* next;
} Node;

// Função para adicionar um nó ao final da lista
void append(Node** head, const char* name) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->filename, name);
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

// Função para liberar a memória da lista
void free_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
}

int main() {
    Node* head = NULL;
    
    // Criação da lista fictícia
    append(&head, "documento_A.txt");
    append(&head, "imagem_B.png");
    append(&head, "planilha_C.csv");
    append(&head, "codigo_D.c");
    append(&head, "log_E.txt");

    printf("--- Execucao 1: Regiao paralela SEM sincronizacao de criacao ---\n");
    #pragma omp parallel
    {
        Node* current = head;
        while (current != NULL) {
            // Cada thread cria uma tarefa para processar o nó atual
            #pragma omp task firstprivate(current)
            {
                printf("[Incorreto] Thread %d processando arquivo: %s\n", omp_get_thread_num(), current->filename);
            }
            current = current->next;
        }
    }

    printf("\n--- Execucao 2: Regiao paralela COM #pragma omp single ---\n");
    #pragma omp parallel
    {
        // Apenas UMA thread percorre a lista e gera as tarefas
        #pragma omp single
        {
            Node* current = head;
            while (current != NULL) {
                #pragma omp task firstprivate(current)
                {
                    printf("[Correto] Thread %d processando arquivo: %s\n", omp_get_thread_num(), current->filename);
                }
                current = current->next;
            }
        } // A barreira implícita garante que as tarefas finalizem antes de sair da região
    }

    free_list(head);
    return 0;
}
