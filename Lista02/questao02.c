#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define TAMANHO_VETOR 200000000 // 200 milhões de elementos
#define NUM_THREADS 8

volatile int encontrado = 0;
int indice_encontrado = -1;
int thread_que_encontrou = -1;

int main() {
    int* vetor = (int*)malloc(TAMANHO_VETOR * sizeof(int));
    
    if (vetor == NULL) {
        printf("Falha ao alocar memória para o vetor.\n");
        return 1;
    }

    srand(time(NULL));
    printf("Preenchendo o vetor com %d elementos...\n\n", TAMANHO_VETOR);
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        vetor[i] = rand() % 1000; // Preenche com números aleatórios de 0 a 999
    }

    // Garante que o elemento a ser buscado exista
    int posicao_alvo = rand() % TAMANHO_VETOR; // Uma posição arbitrária
    int alvo = 1001;
    vetor[posicao_alvo] = alvo;

    printf("Busca paralela iniciada com %d threads.\n", NUM_THREADS);
    printf("Procurando pelo elemento: %d\n\n", alvo);

    double t_inicio = omp_get_wtime();

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        // Verifica se outra thread já encontrou o elemento
        if (encontrado) {
            continue;
        }

        if (vetor[i] == alvo) {
            printf("[Thread %d] -> Encontrou o elemento %d na posição %d.\n",
                   omp_get_thread_num(), alvo, i);

            // Apenas uma thread pode executar este bloco por vez
            #pragma omp critical
            {
                if (!encontrado) {
                    encontrado = 1; // Sinaliza que o elemento foi achado
                    indice_encontrado = i;
                    thread_que_encontrou = omp_get_thread_num();
                }
            }
        }
        /*
        else {
             printf("[Thread %d] -> Não encontrou na posição %d.\n", omp_get_thread_num(), i);
        }
        */
    }

    double t_fim = omp_get_wtime();

    if (!encontrado) {
        printf("O elemento %d NÃO FOI ENCONTRADO no vetor.\n", alvo);
    }

    printf("Tempo total da busca: %f segundos.\n", t_fim - t_inicio);

    free(vetor);

    return 0;
}