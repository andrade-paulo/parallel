#include <omp.h> 
#include <stdio.h>  

int main(){ 
    const int SIZE = 10;
    int n = 50; 
    float a[SIZE];

    for (int i=0; i<SIZE; i++){ 
        a[i] = i*3.14;
    } 

    #pragma omp parallel
    { 
        int id = omp_get_thread_num(); 
        int nthrds = omp_get_num_threads(); 

        for (int i=id; i<SIZE; i+=nthrds){ 
            a[i] = a[i] * n; 
        } 
    }

    printf("Vetor apos multiplicacao:\n");
    for (int i=0; i<SIZE; i++){
        printf("%f; ", a[i]);
    }
    printf("\n");
}
