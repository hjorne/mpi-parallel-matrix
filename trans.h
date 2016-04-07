#ifndef _TRANS_H_
#define _TRANS_H_

void * handleCopy(void * arg);
double** CalculateTranspose(int num_rows, int row_length, int num_threads, double** orig_matrix);

#endif