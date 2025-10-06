#include <omp.h> 
#include <stdio.h>  

int main(){ 
    const int SIZE = 1000000;
    float a[SIZE], soma;

    for (int i=0; i<SIZE; i++){ 
        a[i] = i*3.14159;
    } 

    #pragma omp parallel 
    {
        float local_sum = 0.0;

        // Calculo da soma parcial
        #pragma omp for
        for (int i=0; i<SIZE; i++){
            local_sum += a[i];
        }

        // Soma atomica da soma parcial ao total
        #pragma omp atomic
        soma += local_sum;
    }

    printf("Soma dos elementos: %f\n", soma);
}
