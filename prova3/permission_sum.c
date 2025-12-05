#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 5) {
        if (rank == 0) {
            printf("Erro: Este programa exige exatamente 5 processos.\n");
        }
        MPI_Finalize();
        return 0;
    }

    int valor = rank;
    int buffer_rx = 0;  // Buffer para receber dados
    int sinal = 0;      // Variável dummy para sinalização
    
    const int TAG_DADO = 100;
    const int TAG_SINAL = 200;


    if (rank == 0) {
        printf("[P0] Enviando permissão de início para P1...\n");
        MPI_Send(&sinal, 1, MPI_INT, 1, TAG_SINAL, MPI_COMM_WORLD);
    
    } else if (rank == 1) {
        MPI_Recv(&sinal, 1, MPI_INT, 0, TAG_SINAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[P1] Permissão recebida de P0. Enviando valor %d para P3.\n", valor);
        
        MPI_Send(&valor, 1, MPI_INT, 3, TAG_DADO, MPI_COMM_WORLD);

    } else if (rank == 2) {
        printf("[P2] Enviando autorização de  impressão para P4...\n");
        MPI_Send(&sinal, 1, MPI_INT, 4, TAG_SINAL, MPI_COMM_WORLD);

    } else if (rank == 3) {
        MPI_Recv(&buffer_rx, 1, MPI_INT, 1, TAG_DADO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int resultado = buffer_rx + rank; // Dado recebido (1) + Rank atual (3)
        printf("[P3] Recebeu %d. Somando meu rank (3). Enviando %d para P4.\n", buffer_rx, resultado);
        
        MPI_Send(&resultado, 1, MPI_INT, 4, TAG_DADO, MPI_COMM_WORLD);

    } else if (rank == 4) {
        MPI_Recv(&buffer_rx, 1, MPI_INT, 3, TAG_DADO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[P4] Dado recebido (%d). Aguardando autorização de P2...\n", buffer_rx);

        MPI_Recv(&sinal, 1, MPI_INT, 2, TAG_SINAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("\n------------------------------------------------\n");
        printf("[P4] Autorização de P2 recebida. VALOR FINAL: %d\n", buffer_rx);
        printf("------------------------------------------------\n");
    }

    MPI_Finalize();
    return 0;
}
