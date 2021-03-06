# Hetrogenous Parallel Computing

### MPI Wiki
  Message Passing Interface (MPI) is a standardized and portable message-passing standard designed by a group of researchers from academia and industry to function on a wide variety of parallel computing architectures. 
  The standard defines the syntax and semantics of a core of library routines useful to a wide range of users writing portable message-passing programs in C, C++, and Fortran. There are several well-tested and efficient implementations of MPI, many of which are open-source or in the public domain. These fostered the development of a parallel software industry, and encouraged development of portable and scalable large-scale parallel applications.

### Experiments with MPI.
Different sorting algorithms are implemented. They are broken down into chunks and scattered over the cluster of MPI machines
The sortig algorithms implemented:
* QuickSort
* MergeSort - iterative
* MergeSort - recursive
* Mandelbrots set https://en.wikipedia.org/wiki/Mandelbrot_set
### MPI - general flow
1. MPI_INIT
2. Divide the workload in equal-sized chunks
3. Send each subarray to each node to process
4. Perform the computation on each node
5. Gather the result subarrays into one node (master)
6. Make the final mergeSort/compute call
