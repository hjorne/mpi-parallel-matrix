#ifndef _GROUPIO_H_
#define _GROUPIO_H_

void groupFileWrite(char* file, int num_rows, int row_length, int rank, int ranksPerFile, int file_block_bytes, double* matrix);

#endif
