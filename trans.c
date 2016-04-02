#include <stdlib.h>
#include <pthread.h>

#include "trans.h"

struct copyThreadArgs { //This could theoretically be optimized further by simply having the orig_matrix, transpose, num_rows, and row_length as globals
    double** transpose;
    double** orig_matrix;
    int matrix_row_start;
    int num_rows;
    int row_length;
}

//Function that handles copying num_rows rows from the orig_matrix to the transpose matrix
void * handleCopy(void * arg) {
    struct copyThreadArgs *arg_struct = args;
    double** transpose = (*arg_struct).transpose;
    double** orig_matrix = (*arg_struct).orig_matrix;
    int matrix_row_start = (*arg_struct).matrix_row_start;
    int num_rows = (*arg_struct).num_rows;
    int row_length = (*arg_struct).row_length;
    int i, j;
    
    for(i = matrix_row_start; i < matrix_row_start + num_rows; i++){  //For each row
        for(j = 0; j < row_length; j++){
            transpose[j][i] = orig_matrix[i][j];
        }
    }
    pthread_exit(0);
}
//A(i,j) = At(j,i)
double** CalculateTranspose(int num_rows, int row_length, int num_threads, double** orig_matrix){
    
    int i, rc;
    pthread_t * thread_ids = malloc(sizeof(pthread_t) * (num_threads-1)); //Init thread id array
    
    int rows_per_thread = num_rows/(num_threads-1); //num_threads is adjusted by 1 since we will be reusing the parent thread
    
    double** transpose = (double**) malloc(row_length * sizeof(double*)); //Init first dimension of transpose
    
    for(i = 0; i < row_length; i++){
        transpose[i] = malloc(num_rows * sizeof(double)); //This must be done before the creation of the threads since each thread requires the every row of the transpose
    }
    
    for(i = 0; i < (num_threads-1)){ //Once again adjust by 1 to allow for reuse of parent thread
        struct copyThreadArgs *new_args = malloc(sizeof(struct copyThreadArgs)); //Create arg struct for each thread, might want to free this at some point, not sure where
        (*new_args).transpose = transpose;
        (*new_args).orig_matrix = orig_,atrix;
        (*new_args).matrix_row_start = (i+1) * rows_per_thread;
        (*new_args).num_rows = rows_per_thread;
        (*new_args).row_length = row_length;
        int ptc_rc = pthread_create(&thread_ids[i], NULL, handleCopy, (void*)new_args);
        if(ptc_rc != 0){
            //thread failed to create, not sure how we want to handle
        }
    }
    for(i = 0; i < rows_per_thread; i++){
        for(j = 0; j < row_length; j++){
            transpose[j][i] = orig_matrix[i][j];
        }
    }
    for(i = 0; i < num_threads-1){
        rc = pthread_join(thread_ids[i), NULL);
        if(rc == 0){
            //Thread completed successfully
        } else {
            //Thread failed
        }
    }
    return transpose;
}