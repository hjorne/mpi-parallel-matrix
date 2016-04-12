#include <stdlib.h>
#include "dealloc.h"

void deallocMatrix(double** mat, int rows, int cols)
{
    int i,j;
    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++){
            free(mat[i][j]);
        }
        free(mat[i]);
    }
    free(mat);
}
