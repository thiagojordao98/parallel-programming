#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 10000000

int main() {
    int count;
    double pi;

    printf("--- 1. Race Condition ---\n");
    count = 0;
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        unsigned int seed = i;
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            count++; // Race condition
        }
    }
    pi = 4.0 * count / N;
    printf("Pi (Race Condition): %f (Count: %d)\n\n", pi, count);

    printf("--- 2. Critical Section ---\n");
    count = 0;
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        unsigned int seed = i;
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            #pragma omp critical
            {
                count++;
            }
        }
    }
    pi = 4.0 * count / N;
    printf("Pi (Critical): %f (Count: %d)\n\n", pi, count);

    printf("--- 3. Clauses (private, firstprivate, shared) ---\n");
    count = 0;
    int shared_var = 10;
    int first_priv_var = 20;
    int last_priv_var = 0;
    
    #pragma omp parallel default(none) shared(count, shared_var, last_priv_var) firstprivate(first_priv_var)
    {
        int local_count = 0; // private implicitly by declaration
        unsigned int seed = omp_get_thread_num();
        
        #pragma omp for lastprivate(last_priv_var)
        for (int i = 0; i < N; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0) {
                local_count++;
            }
            if (i == N - 1) {
                last_priv_var = omp_get_thread_num(); // record thread num that does last iteration
            }
        }
        
        #pragma omp critical
        {
            count += local_count;
            shared_var++; // Demonstrating shared mutation safely
            // first_priv_var can be read, it starts at 20 for all threads
        }
    }
    pi = 4.0 * count / N;
    printf("Pi (Clauses): %f (Count: %d)\n", pi, count);
    printf("shared_var mutated to: %d\n", shared_var);
    printf("first_priv_var original value untouched in outer scope: %d\n", first_priv_var);
    printf("last_priv_var from last iteration thread: %d\n\n", last_priv_var);

    return 0;
}
