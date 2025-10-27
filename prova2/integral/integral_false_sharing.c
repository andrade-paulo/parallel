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
      max_threads_number = strtol(argv[2], NULL, 10);
   }

   double pi, sum[max_threads_number]; 
   int n_threads; 
   
   step = 1.0/(double) num_steps; 

   omp_set_num_threads(max_threads_number); 
   #pragma omp parallel 
   { 
      double x;
      int core_id = omp_get_thread_num(); 
      if (core_id == 0) n_threads = omp_get_num_threads();

      sum[core_id] = 0.0;
      for (int i = core_id; i < num_steps; i=i+n_threads) {
         x = (i + 0.5) * step;
         sum[core_id] += 4.0 / (1.0 + x * x);
      }

      //printf("Core ID: %d\nSoma parcial: %f\n\n", core_id, step * sum[core_id]);
   } 

   pi = 0.0;
   for (int i = 0; i < n_threads; i++) {
      pi += step * sum[i];
   }

   double END_TIME = omp_get_wtime();

   printf("PI: %f\n", pi);
   printf("Tempo (ms): %f\n", (END_TIME - START_TIME) * 1000.0);
}
