# PCD-2024

Para compilar os códigos use:
```bash
gcc -o ./outputs/arquivo ./files/arquivo.c

```
Execução do código:

```bash
./outputs/arquivo

```
Compilar arquivo com OpenMP:
```bash
gcc -o ./outputs/arquivo -fopenmp ./files/arquivo.c

```
Execução do código:
```bash
./outputs/arquivo

```
Compilar arquivo com CUDA:
```bash
nvcc ./files/Entrega2-par.cu -o ./outputs/Entrega2-par
```

Compilar arquivo com MPI
```bash
mpicc -o  ./outputs/Entrega3-par ./Entregas/Entrega3-par.c
```

Execução do código:
```bash
mpirun -np 1 ./outputs/Entrega3-par
```
