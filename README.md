# PCD-2024

Para compilar os códigos use:
gcc -o ./outputs/arquivo ./files/arquivo.c

Execução do código:
./outputs/arquivo

Compilar arquivo com OpenMP:
gcc -o ./outputs/arquivo -fopenmp ./files/arquivo.c

Execução do código:
./outputs/arquivo

Compilar arquivo com CUDA:
nvcc ./files/Entrega2-par.cu -o ./outputs/Entrega2-par
