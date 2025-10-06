#include <stdio.h>
#include <omp.h>

int main(int argc, char* argv[]) {
  int nt, tid;

  #pragma omp parallel num_threads(5)
  { 
    tid = omp_get_thread_num();
    printf("Eu sou a Thread %d \n", tid);

    if (tid == 0) {
       nt = omp_get_num_threads();
       printf("Numero total de threads: %d\n", nt);
    }
  }

  return 0;
}
