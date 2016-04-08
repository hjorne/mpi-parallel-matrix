#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "clcg4.h"
#include "init.h"
#include "trans.h"
#include "data.h"
#include "add.h"
#include "collective.h"

void printMatrix(double** matrix, int num_rows, int row_length){
    int i,j;
    for(i = 0; i < num_rows; i++){
        for(j = 0; j < row_length; j++){
            printf("%f ",matrix[i][j]);
        }
        printf("\n");
    }
}

//Usage mpirun -np numranks ./execname <matrix size> <number of threads per rank> <padding bytes>
int main(int argc, char** argv)
{
    if(argc != 4){
        printf("Invalid number of args!\n");
        printf("Usage: mpirun -np numranks ./execname <matrix size> <number of threads per rank> <padding bytes>\n");
        exit(1);
    }

    int myrank, numranks, seed, MATRIX_SIZE, NUM_THREADS, PADDING_BYTES;

    MATRIX_SIZE = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);
    FILE_BLOCK_BYTES = atoi(argv[3]);

    MPI_Init( &argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
    InitDefault();  // clcg4 initialization

    double** mymat = InitMyMatrix(myrank, numranks, MATRIX_SIZE);
    double** transpose = CalculateTranspose(MATRIX_SIZE/numranks, MATRIX_SIZE, NUM_THREADS, mymat);
    //double** new_transpose = transferData(myrank, numranks, MATRIX_SIZE, mymat, transpose);
    //double** added = addMatrix(numranks, MATRIX_SIZE, NUM_THREADS, mymat, new_transpose);

    collectiveFileWrite("collectiveOut",MATRIX_SIZE/numranks, MATRIX_SIZE, mymat, FILE_BLOCK_BYTES, myrank);

    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
    return 0;
}
