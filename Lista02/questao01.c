#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 8

struct timeval tempo_inicio_global;

int main() {
    printf("%d threads serão criadas para a corrida.\n", NUM_THREADS);

    omp_set_num_threads(NUM_THREADS);

    // Inicia a região paralela. O código dentro deste bloco será
    // executado por todas as NUM_THREADS.
    #pragma omp parallel
    {
        int id = omp_get_thread_num();

        // Sinal de largada dado pela thread mestre (ID 0)
        #pragma omp master
        {
            printf("\n[Thread Mestre %d]: Pressione ENTER para iniciar a corrida...\n", id);
            getchar(); // Espera por um Enter

            gettimeofday(&tempo_inicio_global, NULL);

            printf("----------------- SINAL DE LARGADA -----------------\n");
        }

        // Barreira de sincronização
        #pragma omp barrier

        // Formatação do tempo para exibição
        char buffer_tempo[32];
        time_t segundos = tempo_inicio_global.tv_sec;
        strftime(buffer_tempo, 32, "%H:%M:%S", localtime(&segundos));

        printf("[Thread %d]: LARGADA! Início: %s.%06ld\n",
               id,
               buffer_tempo,
               (long)tempo_inicio_global.tv_usec);

        // Simula algum trabalho
        usleep(100000 + (id * 50000));

        printf("[Thread %d]: Tarefa concluída.\n", id);
    }

    printf("----------------- FIM DA CORRIDA -----------------\n");
    printf("Todas as tarefas foram concluídas.\n");

    return 0;
}