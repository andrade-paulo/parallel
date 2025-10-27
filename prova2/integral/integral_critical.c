#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

long num_steps = 100000;
int max_threads_number = 4;
double step;

int main(int argc, char *argv[]) {
   double START_TIME = omp_get_wtime();

   if (argc == 3) {
      num_steps = strtol(argv[1], NULL, 10);
      max_threads_number = atoi(argv[2]);
   }

   double pi = 0.0; 
   int n_threads; 
   
   step = 1.0/(double) num_steps; 

   omp_set_num_threads(max_threads_number); 
   #pragma omp parallel 
   { 
      double x, sum = 0.0;
      int core_id = omp_get_thread_num(); 
      if (core_id == 0) n_threads = omp_get_num_threads();

      for (int i = core_id; i < num_steps; i=i+n_threads) {
         x = (i + 0.5) * step;
         sum += 4.0 / (1.0 + x * x);
      }

      #pragma omp critical
      {
         pi += step * sum;
         //printf("Core ID: %d\nPI parcial: %f\n\n", core_id, pi);
      }
   } 

   double END_TIME = omp_get_wtime();

   printf("PI: %f\n", pi);
   printf("Tempo (ms): %f\n", (END_TIME - START_TIME) * 1000);
}
