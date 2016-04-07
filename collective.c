#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <mpi.h>

#include "collective.h"

void collectiveFileWrite(char* file, int num_rows, int row_length, double** matrix, int paddingBytes, int rank){
    long long int chunkSize = (num_rows * row_length * sizeof(double)) + paddingBytes;

    MPI_File fh;
    int rc = MPI_File_open(MPI_COMM_WORLD, file, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    MPI_File_set_view( fh, 0, MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL ) ;

    //int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void  *buf, int count, MPI_Datatype datatype, MPI_Status *status)
    int i;
    MPI_Status * s = malloc(sizeof(MPI_Status) * num_rows);
    for(i = 0; i < num_rows; i++){
        int offset = rank * chunkSize + i * row_length * sizeof(double) + paddingBytes;
        MPI_File_write_at_all(fh, offset, matrix[i], row_length, MPI_DOUBLE, &s[i]);
    }
}
