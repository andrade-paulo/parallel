#include <omp.h> 
#include <stdio.h>  

int main(){ 
    const int SIZE = 100;
    int n = 50; 
    float a[SIZE], b[SIZE], result[SIZE];

    for (int i=0; i<SIZE; i++){ 
        a[i] = i*3.14159;
        b[i] = i*2.71828;
    } 

    #pragma omp parallel
    { 
        int id = omp_get_thread_num(); 
        int nthrds = omp_get_num_threads(); 

        for (int i=id; i<SIZE; i+=nthrds){ 
            result[i] = a[i] + b[i];
        } 
    }

    printf("Vetor apos adicao:\n");
    for (int i=0; i<SIZE; i++){
        printf("%f; ", result[i]);
    }
    printf("\n");
}
