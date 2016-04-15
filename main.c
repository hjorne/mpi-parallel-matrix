#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <hwi/include/bqc/A2_inlines.h>
#include "clcg4.h"
#include "init.h"
#include "trans.h"
#include "data.h"
#include "add.h"
#include "collective.h"
#include "dealloc.h"
#include "groupio.h"

void printMatrix(double** matrix, int num_rows, int row_length)
{
    int i, j;
    printf("\n\n====\n\n");
    for(i = 0; i < num_rows; i++) {
        for(j = 0; j < row_length; j++) {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

double * allocateContiguous(double** orig, int num_rows, int row_length){
    double * contig = malloc(sizeof(double) * num_rows * row_length)
    int i;
    for(i = 0; i < num_rows; i++){
        memcpy(contig[i * num_rows * row_length], orig[i], sizeof(double) * row_length);
    }
    return contig;
}

#define FILE_BLOCK_BYTES 8388608

//Usage mpirun -np numranks ./execname <matrix size> <number of threads per rank> <block size>
int main(int argc, char** argv)
{
    if(argc != 3) {
        printf("Invalid number of args!\n");
        printf("Usage: mpirun -np numranks ./execname <matrix size> <number of threads per rank>\n");
        exit(1);
    }

    unsigned long long start_cycle_time;
    unsigned long long end_cycle_time;
    unsigned long long total_cycle_time;
    unsigned long long global_runtime = GetTimeBase();
    double time_taken;

    int myrank, numranks, MATRIX_SIZE, NUM_THREADS;

    MATRIX_SIZE = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);
    //FILE_BLOCK_BYTES = atoi(argv[3]);

    MPI_Init( &argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
    InitDefault();  // clcg4 initialization

    MPI_Barrier( MPI_COMM_WORLD );

    start_cycle_time = GetTimeBase();
    printf("Rank %i: Initialization started\n", myrank);
    double** mymat = InitMyMatrix(myrank, numranks, MATRIX_SIZE);
    printf("Rank %i: Initialization complete\n", myrank);

    printf("Rank %i: Transpose started\n", myrank);
    double** transpose = CalculateTranspose(MATRIX_SIZE / numranks, MATRIX_SIZE, NUM_THREADS, mymat);
    printf("Rank %i: Transpose complete\n", myrank);

    printf("Rank %i: Transfer started\n", myrank);
    double** new_transpose = transferData(myrank, numranks, MATRIX_SIZE, mymat, transpose);
    printf("Rank %i: Transfer complete\n", myrank);

    printf("Rank %i: Deallocation started\n", myrank);
    deallocMatrix(mymat, MATRIX_SIZE);
    printf("Rank %i: Deallocation complete\n", myrank);

    printf("Rank %i: Matrix addition started\n", myrank);
    double** added = addMatrix(numranks, MATRIX_SIZE, NUM_THREADS, mymat, new_transpose);
    printf("Rank %i: Matrix addition complete\n", myrank);

    printf("Rank %i: Contiguous allocation started\n", myrank);
    double* added_contig = allocateContiguous(added,MATRIX_SIZE/myrank,MATRIX_SIZE)
    printf("Rank %i: Contiguous allocation finished\n", myrank);

    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("Calculations took %f seconds\n", time_taken);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    collectiveFileWrite("collectiveOut", MATRIX_SIZE / numranks, MATRIX_SIZE, added_contig, 0, myrank);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("Collective with zero padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    collectiveFileWrite("collectiveOut", MATRIX_SIZE / numranks, MATRIX_SIZE, added_contig, FILE_BLOCK_BYTES,
                        myrank);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("Collective with 8mb padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    groupFileWrite("groupOut", MATRIX_SIZE / numranks, MATRIX_SIZE, myrank, 4, 0, added_contig);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("4 Group with zero padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    groupFileWrite("groupOut", MATRIX_SIZE / numranks, MATRIX_SIZE, myrank, 4, FILE_BLOCK_BYTES, added_contig);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("4 Group with 8mb padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    groupFileWrite("groupOut", MATRIX_SIZE / numranks, MATRIX_SIZE, myrank, 8, 0, added_contig);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("8 Group with zero padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    groupFileWrite("groupOut", MATRIX_SIZE / numranks, MATRIX_SIZE, myrank, 8, FILE_BLOCK_BYTES, added_contig);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("8 Group with 8mb padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    groupFileWrite("groupOut", MATRIX_SIZE / numranks, MATRIX_SIZE, myrank, 32, 0, added_contig);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("32 Group with zero padding took %f seconds\n", time_taken);
    }


    MPI_Barrier( MPI_COMM_WORLD );
    start_cycle_time = GetTimeBase();
    groupFileWrite("groupOut", MATRIX_SIZE / numranks, MATRIX_SIZE, myrank, 32, FILE_BLOCK_BYTES,
                   added_contig);
    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - start_cycle_time;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));
    if(myrank == 0) {
        printf("32 Group with 8mb padding took %f seconds\n", time_taken);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    end_cycle_time = GetTimeBase();
    total_cycle_time = end_cycle_time - global_runtime;
    time_taken = (double)total_cycle_time / (1.6 * pow(10.0, 9));

    if(myrank == 0) {
        printf("All operations took %f seconds\n", time_taken);
    }
    MPI_Finalize();
    return 0;
}
