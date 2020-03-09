# ParallelSort

### Experiments with MPI.
Different sorting algorithms are implemented. They are broken down into chunks and scattered over the cluster of MPI machines
The sortig algorithms implemented:
* QuickSort
* MergeSort - iterative
* MergeSort - recursive

### MPI
1. MPI_INIT
2. Divide the array in equal-sized chunks
3. Send each subarray to each node to process
4. Perform the sort on each process
5. Gather the sorted subarrays into one
6. Make the final mergeSort call
