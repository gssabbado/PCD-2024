#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h> // Para gettimeofday

#include <cuda_runtime.h>

#define N 2000
#define T 1000
#define D 0.1
#define DELTA_T 0.01
#define DELTA_X 1.0

__global__ void diff_eq_kernel(double *C, double *C_new, int Num)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i > 0 && i < Num - 1 && j > 0 && j < Num - 1) {
        int idx = i * Num + j;
        C_new[idx] = C[idx] + D * DELTA_T *
                    ((C[(i+1)*Num+j] + C[(i-1)*Num+j] + C[i*Num+(j+1)] + C[i*Num+(j-1)] - 4 * C[idx]) /
                     (DELTA_X * DELTA_X));
    }
}

int main()
{
    double *h_C;
    h_C = (double *)malloc(N * N * sizeof(double));

    if (h_C == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < N * N; i++) {
        h_C[i] = 0.0;
    }
    h_C[(N/2) * N + (N/2)] = 1.0;

    double *d_C, *d_C_new;
    cudaMalloc((void**)&d_C, N * N * sizeof(double));
    cudaMalloc((void**)&d_C_new, N * N * sizeof(double));

    cudaMemcpy(d_C, h_C, N * N * sizeof(double), cudaMemcpyHostToDevice);

    dim3 dimBlock(4, 2); // Exemplo de dimensões
    dim3 dimGrid((N + dimBlock.x - 1) / dimBlock.x, (N + dimBlock.y - 1) / dimBlock.y);

    // Medição de tempo
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int t = 0; t < T; t++) {
        diff_eq_kernel<<<dimGrid, dimBlock>>>(d_C, d_C_new, N);

        // Sincronizar para garantir que todos os threads terminaram
        cudaDeviceSynchronize();

        // Trocar ponteiros para a próxima iteração
        double* temp = d_C;
        d_C = d_C_new;
        d_C_new = temp;
    }

    gettimeofday(&end, NULL);

    cudaMemcpy(h_C, d_C, N * N * sizeof(double), cudaMemcpyDeviceToHost);

    printf("Concentração final no centro: %f\n", h_C[(N/2) * N + (N/2)]);

    // Calcular o tempo decorrido
    double elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0; // Segundos para milissegundos
    elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0; // Microssegundos para milissegundos
    printf("Tempo total de execução: %.2f ms\n", elapsedTime);

    cudaFree(d_C);
    cudaFree(d_C_new);
    free(h_C);

    return 0;
}
