#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define TAMANHO_VETOR 40
#define NUM_THREADS 4

int main() {
    int vetor[TAMANHO_VETOR];

    // Preenche o vetor
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        vetor[i] = 100 + (i * 10);
    }

    printf("Iniciando varredura ordenada do vetor com %d threads.\n", NUM_THREADS);
    printf("O vetor tem %d elementos.\n", TAMANHO_VETOR);
    printf("---------------------------------------------------\n");

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for ordered
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        #pragma omp ordered
        {
            printf("[Thread %d] imprimindo Vetor[%02d] = %d\n",
                   omp_get_thread_num(), i, vetor[i]);
        }
    }

    printf("---------------------------------------------------\n");
    printf("Varredura ordenada concluída.\n");

    return 0;
}