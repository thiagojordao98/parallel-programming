#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Função para calcular arctan(x) usando a série de Taylor
// arctan(x) = x - x^3/3 + x^5/5 - x^7/7 + ...
double taylor_arctan(double x, long long iterations) {
    double result = 0.0;
    double x_squared = x * x;
    double power = x;
    int sign = 1;

    for (long long i = 0; i < iterations; i++) {
        result += sign * (power / (2.0 * i + 1.0));
        power *= x_squared; // Próxima potência de x para o termo seguinte
        sign = -sign;
    }
    
    return result;
}

double calculate_pi_machin(long long iterations) {
    // Fórmula de Machin: pi / 4 = 4 * arctan(1/5) - arctan(1/239)
    // pi = 16 * arctan(1/5) - 4 * arctan(1/239)
    double term1 = taylor_arctan(1.0 / 5.0, iterations);
    double term2 = taylor_arctan(1.0 / 239.0, iterations);
    
    return 16.0 * term1 - 4.0 * term2;
}

int main() {
    // A série de Machin converge absurdamente mais rápido que Leibniz.
    long long iter_counts[] = {1, 2, 3, 5, 10, 20, 100, 1000, 100000, 10000000};
    int num_tests = sizeof(iter_counts) / sizeof(iter_counts[0]);

    printf("Aproximação de Pi pela Fórmula de Machin\n");
    printf("%-15s | %-15s | %-15s | %-15s\n", "Iterações", "Tempo (s)", "Valor Estimado", "Erro Absoluto");
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < num_tests; i++) {
        long long iters = iter_counts[i];
        
        clock_t start_time = clock();
        double pi_approx = calculate_pi_machin(iters);
        clock_t end_time = clock();
        
        double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        double error = fabs(M_PI - pi_approx);
        
        printf("%-15lld | %-15.6f | %-15.10f | %-15.10f\n", iters, elapsed_time, pi_approx, error);
    }

    printf("----------------------------------------------------------------------\n");
    printf("Valor real de Pi: %.15f\n", M_PI);

    return 0;
}
