#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>

#include "add.h"

struct addThreadArgs
{
    double** orig;
    double** transpose;
    double** added;
    int row_start;
    int num_rows;
    int row_length;
};

void * handleAdd(void * arg)
{
    struct addThreadArgs *args = arg;
    int i, j;
    for(i = args->row_start; i < args->row_start + args->num_rows; i++)
    {
        for(j = 0; j < args->row_length; j++)
        {
            args->added[i][j] = args->orig[i][j] + args->transpose[i][j];
        }
    }
    pthread_exit(0);
}

double** addMatrix(int num_ranks, int matrix_size, int num_threads, double** orig, double** transpose)
{
    int numrows = matrix_size / num_ranks;
    double** added = (double**) calloc(numrows, sizeof(double*));
    int i, j, rc;
    for(i = 0; i < numrows; i++)
    {
        added[i] = (double*) calloc(matrix_size, sizeof(double));
    }

    pthread_t* thread_ids = malloc(sizeof(pthread_t) * (num_threads - 1));
    int rows_per_thread = numrows/(num_threads-1);
    
    for(i = 0; i<(num_threads - 1); i++)
    {
        struct addThreadArgs *args = malloc(sizeof(struct addThreadArgs));
        args->transpose = transpose;
        args->orig = orig;
        args->row_start = (i+1)*rows_per_thread;
        args->num_rows = rows_per_thread;
        args->row_length = matrix_size;
        /*int p = */pthread_create(&thread_ids[i], NULL, handleAdd, (void*)args);
    }

    for(i = 0; i < rows_per_thread; i++)
    {
        for(j = 0; j < matrix_size; j++)
        {
            added[i][j] = orig[i][j];
        }
    }
    for(i = 0; i < num_threads-1; i++){
        rc = pthread_join(thread_ids[i], NULL);
        if(rc == 0){
            //Thread completed successfully
        } else {
            //Thread failed
        }
    }
    return added;
}
