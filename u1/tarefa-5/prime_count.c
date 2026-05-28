#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

static int is_prime(long n) {
    if (n < 2) {
        return 0;
    }

    for (long i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }

    return 1;
}

static long count_primes_sequential(long n) {
    long count = 0;

    for (long i = 2; i <= n; i++) {
        if (is_prime(i)) {
            count++;
        }
    }

    return count;
}

static long count_primes_parallel(long n) {
    long count = 0;

    #pragma omp parallel for reduction(+:count)
    for (long i = 2; i <= n; i++) {
        if (is_prime(i)) {
            count++;
        }
    }

    return count;
}

int main(int argc, char **argv) {
    long n = 500000;
    if (argc > 1) {
        n = strtol(argv[1], NULL, 10);
    }

    if (n < 2) {
        fprintf(stderr, "Uso: %s [n>=2]\n", argv[0]);
        return 1;
    }

    double start_seq = omp_get_wtime();
    long seq_count = count_primes_sequential(n);
    double seq_time = omp_get_wtime() - start_seq;

    double start_par = omp_get_wtime();
    long par_count = count_primes_parallel(n);
    double par_time = omp_get_wtime() - start_par;

    printf("Contagem de primos de 2 até %ld\n", n);
    printf("SEQUENCIAL -> primos: %ld | tempo(s): %.6f\n", seq_count, seq_time);
    printf("PARALELA   -> primos: %ld | tempo(s): %.6f | threads: %d\n",
           par_count, par_time, omp_get_max_threads());

    if (seq_count == par_count) {
        printf("Resultado: OK (valores iguais)\n");
    } else {
        printf("Resultado: ERRO (valores diferentes)\n");
    }

    if (par_time > 0.0) {
        printf("Speedup (seq/par): %.2fx\n", seq_time / par_time);
    }

    return 0;
}
