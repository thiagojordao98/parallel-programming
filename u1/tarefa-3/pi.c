#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double calculate_pi(long long iterations) {
    double pi_approx = 0.0;
    int sign = 1;
    for (long long i = 0; i < iterations; i++) {
        pi_approx += sign * (1.0 / (2.0 * i + 1.0));
        sign = -sign;
    }
    return pi_approx * 4.0;
}

int main() {
    long long iter_counts[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    int num_tests = sizeof(iter_counts) / sizeof(iter_counts[0]);

    printf("%-15s | %-15s | %-15s | %-15s\n", "Iterações", "Tempo (s)", "Valor Estimado", "Erro Absoluto");
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < num_tests; i++) {
        long long iters = iter_counts[i];
        
        clock_t start_time = clock();
        double pi_approx = calculate_pi(iters);
        clock_t end_time = clock();
        
        double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        double error = fabs(M_PI - pi_approx);
        
        printf("%-15lld | %-15.6f | %-15.10f | %-15.10f\n", iters, elapsed_time, pi_approx, error);
    }

    printf("----------------------------------------------------------------------\n");
    printf("Valor real de Pi: %.15f\n", M_PI);

    return 0;
}
