#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int meu_valor = rank;
    int soma_total = 0;  // Deixa de ser um unico buffer

    MPI_Reduce(&meu_valor, &soma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("[Root] A soma total dos ranks de 0 a %d é: %d\n", size-1, soma_total);
    }

    MPI_Finalize();
    return 0;
}
