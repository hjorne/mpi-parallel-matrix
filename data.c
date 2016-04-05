#include <stdlib.h>
#include <mpi.h>

#include "data.h"

double** transferData(int my_rank, int numranks, int matrix_size, double** orig, double** transpose)
{
    int numrows = matrix_size / num_ranks;
    double** new_transpose = (double**) calloc(numrows, sizeof(double*));
    int i;
    for(i = 0; i < numrows, i++)
    {
        new_transpose[i] = (double*) calloc(matrix_size, sizeof(double));
    }

    for(i = 0; i < numranks; i++)
    {
        
    }
}
