#include <omp.h> 
#include <stdio.h>  

int main(){ 
    const int SIZE = 100;
    int n = 50; 
    float a[SIZE], b[SIZE], soma[SIZE], sub[SIZE];

    for (int i=0; i<SIZE; i++){ 
        a[i] = i*3.14159;
        b[i] = i*2.71828;
    } 

    #pragma omp parallel num_threads(2)
    { 
        // Thread 0: Soma
        if (omp_get_thread_num() == 0) {
            for (int i=0; i<SIZE; i++){
                soma[i] = a[i] + b[i];
            }
        }
        // Thread 1: Subtracao
        else {
            for (int i=0; i<SIZE; i++){
                sub[i] = a[i] - b[i];
            }
        }
    }

    printf("Vetor apos adicao:\n");
    for (int i=0; i<SIZE; i++){
        printf("%f; ", soma[i]);
    }
    printf("\n\n");

    printf("Vetor apos subtracao:\n");
    for (int i=0; i<SIZE; i++){
        printf("%f; ", sub[i]);
    }
    printf("\n");
}
