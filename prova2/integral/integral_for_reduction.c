#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/**
 * @brief Funcao alvo: f(x) = 4 / (1 + x^2)
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * @brief Versao Sequencial
 */
double trapezio_sequencial(double a, double b, long long N) {
    double h = (b - a) / (double)N;
    double integral = (f(a) + f(b)) / 2.0;

    for (long long i = 1; i < N; i++) {
        double x = a + (double)i * h;
        integral += f(x);
    }
    integral *= h;
    return integral;
}

/**
 * @brief Versao paralela com reduction
 * @param num_threads Numero maximo de threads
 */
double trapezio_paralelo_reduction(double a, double b, long long N, int num_threads) {
    double h = (b - a) / (double)N;
    double integral = (f(a) + f(b)) / 2.0;

    // Obs.: Por questoes de disponibilidade do sistema, 
    // o numero de threads requisitado pode nao ser atendido completamente
    #pragma omp parallel for reduction(+:integral) num_threads(num_threads)
    for (long long i = 1; i < N; i++) {
        double x = a + (double)i * h;
        integral += f(x);
    }
    integral *= h;
    return integral;
}

/**
 * @brief Versao paralela manual usando atomic
 * @param num_threads O número de threads a ser usado.
 */
double trapezio_paralelo_manual(double a, double b, long long N, int num_threads) {
    double h = (b - a) / (double)N;
    double integral = (f(a) + f(b)) / 2.0;
    
    #pragma omp parallel num_threads(num_threads)
    {
        double soma_local = 0.0;
        double x;
        long long i;

        #pragma omp for
        for (i = 1; i < N; i++) {
            x = a + (double)i * h;
            soma_local += f(x);
        }

        // Obs.: O atomic e mais recomendado em detrimento do critical quando
        // a operacao e simples, pois reduz overhead.
        #pragma omp atomic
        integral += soma_local;

    }
    
    integral *= h;
    return integral;
}


int main() {
    long long N;
    double a = 0.0;
    double b = 1.0;
    double start_time, end_time;
    double pi_seq, time_seq;
    
    int threads_a_testar[] = {1, 2, 4, 8, 10, 12};
    int num_casos_teste = sizeof(threads_a_testar) / sizeof(threads_a_testar[0]);

    printf("Digite o numero de subdivisoes (N) (ex: 100000000): ");
    if (scanf("%lld", &N) != 1 || N <= 0) {
        printf("Entrada invalida. N deve ser um inteiro positivo.\n");
        return 1;
    }

    printf("\nCalculando integral com N = %lld...\n", N);
    printf("Valor de referencia (M_PI): %.15f\n", M_PI);
    
    printf("\n-=-=- Versao Sequencial -=-=-\n");
    start_time = omp_get_wtime();
    pi_seq = trapezio_sequencial(a, b, N);
    end_time = omp_get_wtime();
    time_seq = end_time - start_time;
    
    printf("   Valor PI: %.15f (Erro: %.2e)\n", pi_seq, fabs(pi_seq - M_PI));
    printf("   Tempo:    %f segundos\n", time_seq);

    printf("\n-=-=- Versoes Paralelas -=-=-\n");
    printf("+-----------+-----------------+--------------+---------------+---------------+\n");
    printf("| N Threads | Metodo          | Tempo (s)    | Speedup       | Eficiencia    |\n");
    printf("+-----------+-----------------+--------------+---------------+---------------+\n");

    for (int i = 0; i < num_casos_teste; i++) {
        int n_threads = threads_a_testar[i];
        double pi_par, time_par;
        double speedup;
        double eficiencia;

        // Teste do reduction
        start_time = omp_get_wtime();
        pi_par = trapezio_paralelo_reduction(a, b, N, n_threads);
        end_time = omp_get_wtime();
        time_par = end_time - start_time;
        speedup = (time_par > 0) ? (time_seq / time_par) : 0.0;
        eficiencia = speedup / n_threads;

        printf("| %-9d | Reduction       | %-12f | %-12.2fx | %-13.2f |\n", n_threads, time_par, speedup, eficiencia);

        // Teste do manual
        start_time = omp_get_wtime();
        pi_par = trapezio_paralelo_manual(a, b, N, n_threads);
        end_time = omp_get_wtime();
        time_par = end_time - start_time;
        speedup = (time_par > 0) ? (time_seq / time_par) : 0.0;
        eficiencia = speedup / n_threads;

        printf("| %-9d | Manual          | %-12f | %-12.2fx | %-13.2f |\n", n_threads, time_par, speedup, eficiencia);

        if (i < num_casos_teste - 1) {
             printf("|-----------+-----------------+--------------+---------------+---------------|\n");
        }
    }
    printf("+-----------+-----------------+--------------+---------------+---------------+\n");

    return 0;
}