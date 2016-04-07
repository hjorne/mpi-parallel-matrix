#include <time.h>
#include <stdlib.h>

#include "init.h"

int RandSeed(int myrank)
{
    srand( time(NULL) + myrank );
    return rand() % MAX_SEED;
}

double** InitMyMatrix(int myrank, int numranks, int matrix_size)
{
    int i, j;
    int seed = RandSeed(myrank);
    int numrows = matrix_size / numranks;
    double bound = 100.0;

    // Initialize rows (I like calloc okay?)
    double** mymat = (double**) calloc(numrows, sizeof(double*));

    for (i = 0; i < numrows; ++i) {
        // Initialize cols
        mymat[i] = (double*) calloc(matrix_size, sizeof(double));

        for (j = 0; j < matrix_size; ++j) {
            // Initialize matrix values to be between -bound and +bound
            // This means summing a huge matrix should average to ~0
            // Easy sanity check
            mymat[i][j] = (GenVal(seed) - 0.5) * 2.0 * bound;
        }
    }

    return mymat;
}
