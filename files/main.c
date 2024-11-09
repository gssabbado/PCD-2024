#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

#define N 100000000
#define MAX_THREADS 16

float *array;

int main(void){
    float maxglobal;
    long i;

    struct timeval inicio, final2;
    int tmili;

    array = (float*) malloc(N * sizeof(float));

    // Initialize the array
    for(i = 0; i < N; i++) 
        array[i] = i / 1000.0;

    // Start timing
    gettimeofday(&inicio, NULL);

    // Set the number of threads for OpenMP
    omp_set_num_threads(MAX_THREADS);

    // Find the maximum value using OpenMP with a reduction
    maxglobal = array[0];
    #pragma omp parallel for reduction(max:maxglobal)
    for(i = 0; i < N; i++) {
        if(array[i] > maxglobal) maxglobal = array[i];
    }

    // End timing
    gettimeofday(&final2, NULL);
    tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
  
    printf("tempo decorrido: %d milisegundos\n", tmili);
    printf("maior valor=%f\n", maxglobal);

    free(array);

    return 0;
}