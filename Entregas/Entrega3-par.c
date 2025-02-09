#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#define N 2000      // Tamanho da grade
#define T 1000      // Número de iterações no tempo
#define D 0.1       // Coeficiente de difusão
#define DELTA_T 0.01
#define DELTA_X 1.0

// Função para resolver a equação de difusão
void diff_eq(double *C, double *C_new, int local_rows, int rank, int size) {
    MPI_Request reqs[4];
    MPI_Status stats[4];
    
    for (int t = 0; t < T; t++) {
        int req_count = 0;
        
        // Comunicação entre processos
        if (size > 1) {
            // Enviar/receber da borda superior
            if (rank > 0) {
                MPI_Isend(&C[N], N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[req_count]);
                MPI_Irecv(&C[0], N, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[req_count + 1]);
                req_count += 2;
            }
            
            // Enviar/receber da borda inferior
            if (rank < size - 1) {
                MPI_Isend(&C[local_rows * N], N, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[req_count]);
                MPI_Irecv(&C[(local_rows + 1) * N], N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[req_count + 1]);
                req_count += 2;
            }
            
            // Esperar a conclusão de todas as comunicações
            MPI_Waitall(req_count, reqs, stats);
        }
        
        // Cálculo da difusão
        double difmedio = 0.0;
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 1; j < N - 1; j++) {
                // Cálculo da equação de difusão
                C_new[i * N + j] = C[i * N + j] + D * DELTA_T * (
                    (C[(i + 1) * N + j] + C[(i - 1) * N + j] +
                     C[i * N + (j + 1)] + C[i * N + (j - 1)] -
                     4.0 * C[i * N + j]) / (DELTA_X * DELTA_X)
                );
                
                difmedio += fabs(C_new[i * N + j] - C[i * N + j]);
            }
        }
        
        // Copiar os resultados de volta para C
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 1; j < N - 1; j++) {
                C[i * N + j] = C_new[i * N + j];
            }
        }
        
        // Calcular e imprimir a diferença média global
        double global_difmedio;
        MPI_Allreduce(&difmedio, &global_difmedio, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        
        if (rank == 0 && (t % 100) == 0) {
            printf("Iteração %d - Diferença Média = %g\n", t, global_difmedio / ((N - 2) * (N - 2)));
        }
    }
}

int main(int argc, char **argv) {
    int rank, size;
    struct timeval inicio, fim;
    long tmili;
    
    // Inicialização do MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Calcular a distribuição de linhas entre processos
    int rows_per_process = N / size;
    int remainder = N % size;
    int local_rows;
    int start_row;
    
    if (rank < remainder) {
        local_rows = rows_per_process + 1;
        start_row = rank * local_rows;
    } else {
        local_rows = rows_per_process;
        start_row = rank * local_rows + remainder;
    }
    
    // Alocar memória para as matrizes locais (incluindo bordas fantasmas)
    double *C = (double *)calloc((local_rows + 2) * N, sizeof(double));
    double *C_new = (double *)calloc((local_rows + 2) * N, sizeof(double));
    
    if (C == NULL || C_new == NULL) {
        fprintf(stderr, "Erro na alocação de memória no processo %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Inicializar uma concentração alta no centro da matriz
    if (rank == size / 2) {
        int mid_row = local_rows / 2 + 1;  // +1 para considerar a borda fantasma
        C[mid_row * N + N/2] = 1.0;
        C_new[mid_row * N + N/2] = 1.0;
    }
    
    // Medir tempo de execução
    gettimeofday(&inicio, NULL);
    
    // Executar a simulação
    diff_eq(C, C_new, local_rows, rank, size);
    
    gettimeofday(&fim, NULL);
    
    tmili = (long)(1000 * (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000);

    // Imprimir resultados
    if (rank == 0)
    {
        printf("Tempo total de execução: %ld ms\n", tmili);
    }
    // Verificar o valor final no centro
    if (rank == size / 2) {
        int mid_row = local_rows / 2 + 1;
        printf("Concentração final no centro: %f\n", C[mid_row * N + N/2]);
    }
    
    // Liberar memória e finalizar MPI
    free(C);
    free(C_new);
    MPI_Finalize();
    
    return 0;
}