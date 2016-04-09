#include <stdlib.h>
#include <mpi.h>

#include "groupio.h"

void groupFileWrite(char* file, int num_rows, int row_length, int rank, int ranksPerFile, int file_block_bytes, double** matrix){
    int myrankblock = rank/ranksPerFile;
    
    char* filename = malloc(1024*sizeof(char));
    sprintf(filename, "%s_%d",file, myrankblock);

    //do something to add myrankblock to file name

    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, file, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    MPI_File_set_view( fh, 0, MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL ) ;

    int i;
    MPI_Status * s = malloc(sizeof(MPI_Status) * num_rows);
    for(i = 0; i < num_rows; i++){
        //Change offset calculation
        int offset = (rank * chunkSize) + (i * row_length * sizeof(double)) + end_block_padding*rank;
        MPI_File_write_at(fh, offset, matrix[i], row_length, MPI_DOUBLE, &s[i]);
    }
}
