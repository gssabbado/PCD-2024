#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_THREADS 16
#define N 2000
#define T 1000
#define D 0.1
#define DELTA_T 0.01
#define DELTA_X 1.0

void diff_eq_parallel(double **C, double **C_new) {
  for (int t = 0; t < T; t++) {

    #pragma omp parallel for collapse(2)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            C_new[i][j] = C[i][j] + D * DELTA_T * ((C[i + 1][j] + C[i - 1][j] + C[i][j + 1] + C[i][j - 1] - 4 * C[i][j]) / (DELTA_X * DELTA_X));
        }
    }

    double difmedio = 0.;
    #pragma omp parallel for collapse(2) reduction(+:difmedio)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
          difmedio += fabs(C_new[i][j] - C[i][j]);
          C[i][j] = C_new[i][j];
        }
    }

    if ((t % 100) == 0)
      printf("interacao %d - diferenca=%g\n", t, difmedio / ((N - 2) * (N - 2)));
  }
}

int main() {
    double **C = (double **)malloc(N * sizeof(double *));
    if (C == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    for (int i = 0; i < N; i++) {
        C[i] = (double *)malloc(N * sizeof(double));
        if (C[i] == NULL) {
          fprintf(stderr, "Memory allocation failed\n");
          return 1;
        }
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
          C[i][j] = 0.;
        }
    }
    
    double **C_new = (double **)malloc(N * sizeof(double *));
    if (C_new == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    for (int i = 0; i < N; i++) {
        C_new[i] = (double *)malloc(N * sizeof(double));
    
        if (C_new[i] == NULL) {
          fprintf(stderr, "Memory allocation failed\n");
          return 1;
        }
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
          C_new[i][j] = 0.;
        }
    }

    C[N / 2][N / 2] = 1.0;
  
    struct timeval inicio, fim;
    long tmili;

    gettimeofday(&inicio, NULL);

    omp_set_num_threads(MAX_THREADS);

    diff_eq_parallel(C, C_new);
  
    gettimeofday(&fim, NULL);

    // Calcula e exibe o tempo decorrido
    tmili = (int)(1000 * (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000);
    printf("Tempo decorrido: %ld ms\n", tmili);


    printf("Concentração final no centro: %f\n", C[N / 2][N / 2]);
    return 0;
}