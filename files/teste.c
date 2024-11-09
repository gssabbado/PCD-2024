#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#define N 1000000000

int main(int argc, char *argv[]) {
    struct timeval inicio, final2;
    double x, resultado_final;
    int i;
    int tmili;

    gettimeofday(&inicio, NULL);
    // Defina o número máximo de threads
    int max_threads = 16; // substitua com o número desejado
    omp_set_num_threads(max_threads);

    resultado_final = 1.0;
    x = 1.0 + 1.0 / N;

    #pragma omp parallel for reduction(*:resultado_final) private(i)
    for (i = 0; i < N; i++) {
        resultado_final *= x;
    }

    gettimeofday(&final2, NULL);

    // Cálculo do tempo decorrido em milissegundos
    tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);

    printf("Tempo decorrido: %d milissegundos\n", tmili);
    printf("Tempo decorrido (tv_sec): %d\n", (int) (final2.tv_sec - inicio.tv_sec));
    printf("Tempo decorrido (tv_usec): %d\n", (int) (final2.tv_usec - inicio.tv_usec));
    printf("Resultado = %lf\n", resultado_final);

    return 0;
}
