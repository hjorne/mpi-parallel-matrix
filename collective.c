#include <stdlib.h>
#include <pthread.h>
#include <mpi.h>

#include "collective.h"

void collectiveFileWrite(char* file, int num_rows, int row_length, double** matrix, int paddingBytes, int rank){
    long long int chunkSize = (num_rows * row_length * sizeof(double)) + paddingBytes;

    MPI_File fh;
    int rc = MPI_File_open(MPI_COMM_WORLD, file, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    //int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void  *buf, int count, MPI_Datatype datatype, MPI_Status *status)
    int i;
    MPI_Status s;
    for(i = 0; i < num_rows; i++){
        int offset = rank * chunkSize + i * row_length * sizeof(double);
        if(i == 0 && rank != 0){
            offset += paddingBytes;
        }
        MPI_File_write_at_all(fh, offset, matrix[i], row_length, MPI_DOUBLE, &s);
    }
}
