#ifndef _COLLECTIVE_H_
#define _COLLECTIVE_H_

void collectiveFileWrite(char* file, int num_rows, int row_length, double** matrix, int paddingBytes, int rank);

#endif
