#include <stdlib.h>
#include <mpi.h>

#include "collective.h"

void collectiveFileWrite(char* file, int num_rows, int row_length, double** matrix,
                         int file_block_bytes, int rank)
{
    long long int chunkSize = (num_rows * row_length * sizeof(
                                   double)); //Total size of the data written by each rank
    int end_block_padding = 0;
    if(file_block_bytes > 0)
        end_block_padding = chunkSize %
                            file_block_bytes; //Padding based on the remaining bytes left after placing the chunk into blocks

    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, file, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    MPI_File_set_view( fh, 0, MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL ) ;

    //int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void  *buf, int count, MPI_Datatype datatype, MPI_Status *status)
    int i;
    MPI_Status* s;
    //Calculate each offset based on: The base rank chunk offset and the padding for each rank
    int offset = (rank * chunkSize)  + end_block_padding * rank;
    MPI_File_write_at_all(fh, offset, matrix, row_length * num_rows, MPI_DOUBLE, &s);
}
