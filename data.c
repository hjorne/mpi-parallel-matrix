#include <stdlib.h>
#include <mpi.h>
#include <string.h>

#include "data.h"

double** transferData(int my_rank, int numranks, int matrix_size, double** orig, double** transpose)
{
    int numrows = matrix_size / numranks;
    double** new_transpose = (double**) calloc(numrows, sizeof(double*));
    int i;
    for(i = 0; i < numrows; i++) {
        new_transpose[i] = (double*) calloc(matrix_size, sizeof(double));
    }

    MPI_Request* send_reqs = malloc(sizeof(MPI_Request) * numranks * numrows);
    MPI_Request* recv_reqs = malloc(sizeof(MPI_Request) * numranks * numrows);
    for(i = 0; i < numranks; i++) {
        if(i != my_rank) {
            int j;
            for(j = 0; j < numrows; j++) {
                MPI_Irecv(&(new_transpose[j][i * numrows]), numrows, MPI_DOUBLE, i, j, MPI_COMM_WORLD,
                          &recv_reqs[i * numrows + j]);
                MPI_Isend(transpose[(i * numrows) + j], numrows, MPI_DOUBLE, i, j, MPI_COMM_WORLD,
                          &send_reqs[i * numrows + j]);
                  if(j % 128 == 0){
                      MPI_Waitall(128, (send_reqs + (i*numrows)));
                  }
            }
        }
        else {
            int j;
            for(j = 0; j < numrows; j++) {
                memcpy(&(new_transpose[j][i * numrows]), transpose[(i * numrows) + j], numrows * sizeof(double));
            }
        }
    }

    /*
    MPI_Status stat;

    for(i = 0; i < numranks; i++) {
        if(i != my_rank) {
            int j;
            for(j = 0; j < numrows; j++) {
                MPI_Wait(&recv_reqs[i * numrows + j], &stat);
                MPI_Wait(&send_reqs[i * numrows + j], &stat);
            }
        }
    }
    */


    return new_transpose;
}
