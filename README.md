# ParallelHW45

If you work on something, update the readme to explain what you did and how your code works.

If the global matrix is NxN (N = 131,072), then each rank gets an (N/numranks) x N chunk of matrix to work on. 
Each element in the matrix is a uniformly generated random number between -bound and +bound (bound defined in code),
so that when the matrix sum is calculated it should average to 0 for an easy sanity check.

Data sending/receiving may need to be rethought, as it currently would require ~8 million MPI messages on full scale
Data transfer and addition need to be tested and are likely wrong

* Joe - Initial reading of data
* Ezra - Transpose and threading
* Malcolm - Matrix sums
* Alex - Writing
* Everyone does file IO
