#include <stdlib.h>
#include "dealloc.h"

void deallocMatrix(double** mat, int rows, int cols)
{
    int i;
    for(i = 0; i < rows; i++) {
        free(mat[i]);
    }
    free(mat);
}
