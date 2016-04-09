#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <stdio.h>

#include "groupio.h"

void groupFileWrite(char* file, int num_rows, int row_length, int rank, int ranksPerFile, int file_block_bytes, double** matrix){
    long long int chunkSize = (num_rows * row_length * sizeof(double));
    int end_block_padding = 0;
    if(file_block_bytes > 0)
        end_block_padding = chunkSize % file_block_bytes;

    int myrankblock = rank / ranksPerFile;
    int newrank = rank % ranksPerFile;

    char* filename = malloc(1024*sizeof(char));
    sprintf(filename, "%s_%d",file, myrankblock);

    MPI_Comm filegroup;
    MPI_Comm_split(MPI_COMM_WORLD, myrankblock, newrank, &filegroup);
    
    MPI_File fh;
    MPI_File_open(filegroup, filename, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    MPI_File_set_view( fh, 0, MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL ) ;

    int i;
    MPI_Status * s = malloc(sizeof(MPI_Status) * num_rows);
    for(i = 0; i < num_rows; i++){
        //Change offset calculation
        int offset = (newrank * chunkSize) + (i * row_length * sizeof(double)) + end_block_padding*rank;
        MPI_File_write_at(fh, offset, matrix[i], row_length, MPI_DOUBLE, &s[i]);
    }
}
