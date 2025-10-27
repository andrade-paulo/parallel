#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>


int** alocarMatriz(int N) {
    int** matriz;
    
    // Linhas de ponteiros
    matriz = (int**)malloc(N * sizeof(int*));
    if (matriz == NULL) return NULL;

    // Aloca os blocos de dados de cada linha
    int* data = (int*)malloc(N * N * sizeof(int));
    if (data == NULL) {
        free(matriz);
        return NULL;
    }

    // Aponta cada ponteiro de linha para o local correto no bloco de dados
    for (int i = 0; i < N; i++) {
        matriz[i] = &(data[i * N]);
    }
    
    return matriz;
}

void liberarMatriz(int** matriz, int N) {
    if (matriz != NULL) {
        free(matriz[0]); // Libera o bloco de dados
        free(matriz);   // Libera os ponteiros das linhas
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Uso: %s <N> <arquivo_entrada> <arquivo_saida> <threads_1> [threads_2] ...\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    const char* INPUT_FILE = argv[2];
    const char* OUTPUT_FILE = argv[3];

    // Block size para o processamento em blocos da matriz
    // Pode receber um ajuste fino para maximizar o uso da cache, mas depende do processador
    const int BLOCK_SIZE = 64;

    // Determina quantos testes de thread serao executados
    int num_testes = argc - 4;
    int* thread_counts = (int*)malloc(num_testes * sizeof(int));
    if (thread_counts == NULL) {
        perror("Erro ao alocar memoria para contagem de threads");
        return 1;
    }

    printf("Testando com as seguintes contagens de threads: ");
    for (int i = 0; i < num_testes; i++) {
        thread_counts[i] = atoi(argv[i + 4]);
        if (thread_counts[i] <= 0) {
            fprintf(stderr, "Erro: Numero de threads deve ser positivo (recebeu %s)\n", argv[i + 4]);
            free(thread_counts);
            return 1;
        }
        printf("%d ", thread_counts[i]);
    }
    printf("\n");


    if (N <= 0) {
        fprintf(stderr, "N deve ser um inteiro positivo.\n");
        free(thread_counts);
        return 1;
    }

    // Alocacao das matrizes

    int** image = alocarMatriz(N);
    int** blurred_image = alocarMatriz(N);

    if (image == NULL || blurred_image == NULL) {
        perror("Erro ao alocar memoria");
        liberarMatriz(image, N);
        liberarMatriz(blurred_image, N);
        free(thread_counts);
        return 1;
    }

    FILE *fp_in = fopen(INPUT_FILE, "r");
    if (fp_in == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        liberarMatriz(image, N);
        liberarMatriz(blurred_image, N);
        free(thread_counts);
        return 1;
    }

    printf("Lendo %s (esperando %dx%d)...\n", INPUT_FILE, N, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fscanf(fp_in, "%d", &image[i][j]) != 1) {
                fprintf(stderr, "Erro: O arquivo de entrada nao contem %dx%d elementos.\n", N, N);
                fclose(fp_in);
                liberarMatriz(image, N);
                liberarMatriz(blurred_image, N);
                free(thread_counts);
                return 1;
            }
        }
    }
    fclose(fp_in);
    printf("Imagem lida com sucesso.\n\n");

    printf("--- Tabela de Comparacao (N=%d, BLOCK_SIZE=%d) ---\n", N, BLOCK_SIZE);
    printf("| Threads | Tempo de Execucao (s) |\n");
    printf("|---------|-----------------------|\n");

    for (int t_idx = 0; t_idx < num_testes; t_idx++) {
        int num_threads = thread_counts[t_idx];
        
        double START_TIME = omp_get_wtime();

        // Paraleliza os loops EXTERNOS (ii, jj) que iteram sobre os BLOCOS
        // O 'collapse(2)' trata os dois loops de bloco como um unico espaco
        // de iteracao, distribuindo os blocos entre as threads
        #pragma omp parallel for num_threads(num_threads) collapse(2)
        for (int ii = 0; ii < N; ii += BLOCK_SIZE) {
            for (int jj = 0; jj < N; jj += BLOCK_SIZE) {

                // Esta parte e executada sequencialmente por cada thread,
                // maximizando o reuso dos dados que ja estao na cache.
                for (int i = ii; i < ii + BLOCK_SIZE && i < N; i++) {
                    for (int j = jj; j < jj + BLOCK_SIZE && j < N; j++) {
                        
                        // Tratamento de borda: pixels na borda são copiados diretamente
                        if (i == 0 || i == N - 1 || j == 0 || j == N - 1) {
                            blurred_image[i][j] = image[i][j];
                        } else {
                            // Pixel central: calcula a média da vizinhança 3x3
                            long long sum = 0; // risco de overflow com int
                            
                            for (int offset_i = -1; offset_i <= 1; offset_i++) {
                                for (int offset_j = -1; offset_j <= 1; offset_j++) {
                                    sum += image[i + offset_i][j + offset_j];
                                }
                            }
                            
                            blurred_image[i][j] = (int)(sum / 9);
                        }
                    }
                }
            }
        }

        double END_TIME = omp_get_wtime();

        printf("| %7d | %21.6f |\n", num_threads, END_TIME - START_TIME);
    }
    printf("-------------------------------------\n\n");


    // Escrita do resultado (se tiver varios, sera o ultimo)
    printf("Salvando o resultado do ultimo teste (%d threads) em %s...\n", thread_counts[num_testes - 1], OUTPUT_FILE);
    FILE *fp_out = fopen(OUTPUT_FILE, "w");
    if (fp_out == NULL) {
        perror("Erro ao criar o arquivo de saida");
        liberarMatriz(image, N);
        liberarMatriz(blurred_image, N);
        free(thread_counts);
        return 1;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fp_out, "%d", blurred_image[i][j]);
            if (j < N - 1) {
                fprintf(fp_out, " ");
            }
        }
        fprintf(fp_out, "\n");
    }
    fclose(fp_out);

    // Libera a memoria
    liberarMatriz(image, N);
    liberarMatriz(blurred_image, N);
    free(thread_counts);

    printf("Processo concluido.\n");
    return 0;
}