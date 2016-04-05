#include <stdio.h>
#include <mpi.h>
#include "clcg4.h"
#include "init.h"
#include "trans.h"
#include "data.h"
#include "add.h"

#define MATRIX_SIZE 64
#define NUM_THREADS 16

int main(int argc, char** argv)
{
    int myrank, numranks, seed;

    MPI_Init( &argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
    InitDefault();  // clcg4 initialization

    double** mymat = InitMyMatrix(myrank, numranks, MATRIX_SIZE);
    double** transpose = CalculateTranspose(MATRIX_SIZE/numranks, MATRIX_SIZE, NUM_THREADS, mymat);
    double** new_transpose = transferData(myrank, numranks, MATRIX_SIZE, mymat, transpose);
    double** added = addMatrix(numranks, MATRIX_SIZE, NUM_THREADS, mymat, new_transpose);

    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
    return 0;
}
