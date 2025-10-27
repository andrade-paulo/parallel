#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

long num_steps = 100000;
double step;

int main(int argc, char *argv[]) {
   double START_TIME = omp_get_wtime();

   if (argc == 2)
      num_steps = strtod(argv[1], NULL);

   double x, pi, sum = 0.0;

   step = 1.0 / (double)num_steps;

   for (int i=0; i < num_steps; i++) {
      x = (i+0.5) * step;
      sum += 4.0 / (1.0 + x * x);
   }

   pi = step * sum;

   double END_TIME = omp_get_wtime();

   printf("PI: %f\n", pi);
   printf("Tempo: %f\n", (END_TIME - START_TIME) * 1000);
}
