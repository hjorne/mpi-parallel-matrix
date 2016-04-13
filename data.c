#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <stdio.h>

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
    MPI_Status* status_p = malloc(sizeof(MPI_Status) * 128);

    for(i = 0; i < numrows; i++){
        memcpy(&(new_transpose[i][my_rank * numrows]), transpose[(my_rank * numrows) + i], numrows * sizeof(double));
        free(transpose[(my_rank * numrows) + i]);
    }
    for(i = 1; i < numranks; i++) {
        //new logic: comm with offset
        int comm_mat_recv = ((my_rank - i + numranks) % numranks);
        int comm_mat_send = (my_rank + i) % numranks;
        //printf("rank %d, on i %d, send to %d recv from %d\n", my_rank, i, comm_mat_send, comm_mat_recv);
        int j;
        for(j = 0; j < numrows; j++){
            MPI_Irecv(&(new_transpose[j][comm_mat_recv * numrows]), numrows, MPI_DOUBLE, 
                      comm_mat_recv, j, MPI_COMM_WORLD, &recv_reqs[comm_mat_recv * numrows + j]);
            MPI_Isend(transpose[(comm_mat_send * numrows) + j], numrows, MPI_DOUBLE, 
                      comm_mat_send, j, MPI_COMM_WORLD, &send_reqs[comm_mat_send * numrows + j]);
            free(transpose[(comm_mat_send * numrows) + j]);
            if(j % 128 == 127 || j == numrows - 1){
                //printf("rank %d, i %d, comm_mat %d, j %d, jmod %d\n", my_rank, i, comm_mat, j, j%128);
                MPI_Waitall(j % 128, &(send_reqs[comm_mat_send * numrows + (j - (j % 128))]), status_p);
                MPI_Waitall(j % 128, &(recv_reqs[comm_mat_recv * numrows + (j - (j % 128))]), status_p);
            }
        }
    }
    return new_transpose;
}
