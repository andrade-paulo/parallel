#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 8

void operacao_par_soma_impar_max(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype) {
    int *in = (int *)invec;
    int *inout = (int *)inoutvec;
    
    for (int i = 0; i < *len; i++) {
        if (i % 2 == 0) {
            // Par: soma
            inout[i] = inout[i] + in[i];
        } else {
            // Impar: max
            if (in[i] > inout[i]) {
                inout[i] = in[i];
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank * 100);

    int meus_dados[ARRAY_SIZE];
    int resultado[ARRAY_SIZE];

    printf("[P%d] Gerando: [ ", rank);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // Valor aleatório entre 0 e 10
        meus_dados[i] = rand() % 11; 
        
        printf("%d ", meus_dados[i]);
    }
    printf("]\n");

    // Operador MPI
    MPI_Op op_custom;
    MPI_Op_create(&operacao_par_soma_impar_max, 1, &op_custom);

    // O vetor 'resultado' resultará no root
    MPI_Reduce(meus_dados, resultado, ARRAY_SIZE, MPI_INT, op_custom, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Barreira apenas para garantir que o output dos processos
        // acima seja impresso antes do resultado final no terminal (questão estética)
        MPI_Barrier(MPI_COMM_WORLD); 

        printf("\n------------------------------------------------\n");
        printf("RESULTADO FINAL (Pares=Soma, Ímpares=Max):\n");
        printf("[ ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", resultado[i]);
        }
        printf("]\n");
        printf("------------------------------------------------\n");
    } else {
        // Os outros processos também entram na barreira
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Liberando memoria
    MPI_Op_free(&op_custom);

    MPI_Finalize();
    return 0;
}
