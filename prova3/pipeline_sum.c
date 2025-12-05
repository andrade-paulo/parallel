#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            printf("Erro: Execute com pelo menos 2 processos\n");
        }
        MPI_Finalize();
        return 0;
    }

    // O "buffer único" do enunciado
    int token_soma = 0;

    const int TAG = 0; 

    if (rank == 0) {
        token_soma = rank; // Valor inicial é o próprio rank (0)
        
        printf("[P%d] Iniciando o pipeline. Enviando valor %d para P%d.\n", rank, token_soma, rank + 1);
        
        // Envio bloqueante para o próximo
        MPI_Send(&token_soma, 1, MPI_INT, rank + 1, TAG, MPI_COMM_WORLD);

    } else {
        // O receiver já é bloqueante 
	MPI_Recv(&token_soma, 1, MPI_INT, rank - 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        printf("[P%d] Recebeu %d de P%d. ", rank, token_soma, rank - 1);

        token_soma = token_soma + rank;
        printf("Novo total: %d.\n", token_soma);

        if (rank < size - 1) {
            // Se não sou o ultimo, envio para o proximo
            MPI_Send(&token_soma, 1, MPI_INT, rank + 1, TAG, MPI_COMM_WORLD);
        } else {
            // O últime imprime o resultado
            printf("----------------------------------------\n");
            printf("[P%d] Fim do Pipeline. SOMA TOTAL: %d\n", rank, token_soma);
            printf("----------------------------------------\n");
        }
    }

    MPI_Finalize();
    return 0;
}
