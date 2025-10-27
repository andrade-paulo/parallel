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
    // Validacao dos argumentos
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <N> <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }
    
    int N = atoi(argv[1]);
    const char* INPUT_FILE = argv[2];
    const char* OUTPUT_FILE = argv[3];
    
    if (N <= 0) {
        fprintf(stderr, "N deve ser um inteiro positivo.\n");
        return 1;
    }
    
    // Alocacao das matrizes
    int** image = alocarMatriz(N);
    int** blurred_image = alocarMatriz(N);
    
    if (image == NULL || blurred_image == NULL) {
        perror("Erro ao alocar memoria");
        liberarMatriz(image, N);
        liberarMatriz(blurred_image, N);
        return 1;
    }

    // Leitura da imagem do arquivo
    FILE *fp_in = fopen(INPUT_FILE, "r");
    if (fp_in == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        liberarMatriz(image, N);
        liberarMatriz(blurred_image, N);
        return 1;
    }
    
    printf("Lendo %s (esperando %dx%d)...\n", INPUT_FILE, N, N);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Se a leitura falhar, o arquivo e menor que o esperado
            if (fscanf(fp_in, "%d", &image[i][j]) != 1) {
                fprintf(stderr, "Erro: O arquivo de entrada nao contem %dx%d elementos.\n", N, N);
                fclose(fp_in);
                liberarMatriz(image, N);
                liberarMatriz(blurred_image, N);
                return 1;
            }
        }
    }
    fclose(fp_in);
    printf("Imagem lida com sucesso.\n");
    
    printf("Aplicando filtro de suavização...\n");
    
    double START_TIME = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Tratamento de borda: pixels na borda são copiados diretamente
            if (i == 0 || i == N - 1 || j == 0 || j == N - 1) {
                blurred_image[i][j] = image[i][j];
            } else {
                // Pixel central: calcula a média da vizinhança 3x3
                long long sum = 0; // Usar long long para evitar overflow na soma
                
                for (int ki = -1; ki <= 1; ki++) {
                    for (int kj = -1; kj <= 1; kj++) {
                        sum += image[i + ki][j + kj];
                    }
                }
                blurred_image[i][j] = (int)(sum / 9);
            }
        }
    }
    double END_TIME = omp_get_wtime();

    // Arquivo de saida
    FILE *fp_out = fopen(OUTPUT_FILE, "w");
    if (fp_out == NULL) {
        perror("Erro ao criar o arquivo de saída");
        liberarMatriz(image, N);
        liberarMatriz(blurred_image, N);
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

    
    printf("Imagem suavizada salva com sucesso em %s.\n", OUTPUT_FILE);
    printf("Tempo (s): %f\n", END_TIME - START_TIME);
    return 0;
}