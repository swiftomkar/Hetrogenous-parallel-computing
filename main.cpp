#include <stdlib.h>
#include <mpi.h>
#include <iostream>
#include<fstream>
using namespace std;
/********** sorting Function **********/
void QuickSort(int* list,int n,int left,int right){
    if(right<=left){
        return;
    }
    int pivotIndex=rand() %n;
    int pivot=list[pivotIndex];
    //cout<< pivot<<"pivot\n";
    int i=left,j=right;
    while(i<j){
        while(list[i]<pivot) {
            i++;
        }
        while(list[j]>=pivot) {
            j--;
        }
        //if(list[j]<list[i]){
        if(i<j){
            int temp;
            temp=list[i];
            list[i]=list[j];
            list[j]=temp;
        }
    }
    QuickSort(list,n,left,i-1);
    QuickSort(list,n,i,right);
}

void merge(int *a, int *b, int l, int m, int r) {

    int h, i, j, k;
    h = l;
    i = l;
    j = m + 1;

    while((h <= m) || (j <= r)) {

        if(a[h] <= a[j]) {

            b[i] = a[h];
            h++;

        }

        else {

            b[i] = a[j];
            j++;

        }

        i++;

    }
    /*

    if(m < h) {

        for(k = j; k <= r; k++) {

            b[i] = a[k];
            i++;

        }

    }

    else {

        for(k = h; k <= m; k++) {

            b[i] = a[k];
            i++;

        }

    }
*/
    for(k = l; k <= r; k++) {

        a[k] = b[k];

    }

}

/********** Recursive Merge Function **********///sub_array,temp_array, 0,size-1
void mergeSort(int *a, int *b, int l, int r) {

    int m;

    if(l < r) {

        m = (l + r)/2;

        mergeSort(a, b, l, m);
        mergeSort(a, b, (m + 1), r);
        merge(a, b, l, m, r);

    }

}

int main() {
    //----------------------MPI--------------------//
    int world_rank;
    int world_size;

    //MPI_INIT(&argc, &argv);
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //*************write random numbers to a file**********//
    ofstream fileWrite("/home/odesai/Desktop/numbers.dat");
    int num=100;
    fileWrite<<num<<endl;
    for(int i=0;i<num;i++){
        fileWrite <<rand()%num;
        cout<<" ";
    }
    fileWrite.close();

    //------------------data read -----------------//
    double totalTime=0;
    totalTime-=MPI_Wtime();
    ifstream fileA;
    fileA.open("/home/odesai/Desktop/numbers.dat");
    int n;
    fileA>>n;
    if(world_rank==0){
        cout<< n<<endl;
    }
    int original_array[n];
    for(int i=0;i<n;i++){
        fileA>>original_array[i];
    }

    /********** Divide the array in equal-sized chunks **********/
    int size = n/world_size;

    /********** Send each subarray to each process **********/
    //int *sub_array = malloc(size * sizeof(int));
    int *sub_array=new int[size];
    MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);

    /********** Perform the mergesort on each process **********/
    //int *tmp_array = malloc(size * sizeof(int));
    int * tmp_array=new int[size];
    mergeSort(sub_array, tmp_array, 0, (size - 1));
    //QuickSort(sub_array,(size-1),0,(size-1));


    /********** Gather the sorted subarrays into one **********/
    int *sorted = NULL;
    if(world_rank == 0) {

        //sorted = malloc(n * sizeof(int));
        sorted= new int[n];
    }

    MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);

    /********** Make the final mergeSort call **********/
    if(world_rank == 0) {
        totalTime+=MPI_Wtime();
        cout<<"time: "<<totalTime<<endl;
        //int *other_array = malloc(n * sizeof(int));
        int * other_array=new int[n];
        mergeSort(sorted, other_array, 0, (n - 1));
        //QuickSort(sorted,n,0,(n-1));

        /********** Display the sorted array **********/
        cout<<"sorted array: ";
        for(int j = 0; j < n; j++) {
            cout<< sorted[j]<<" ";
        }

        cout<<endl;
        /********** Clean up root **********/
        //free(sorted);
        //free(other_array);

    }

    /********** Clean up rest **********/
    //free(original_array);
    //free(sub_array);
    //free(tmp_array);

    /********** Finalize MPI **********/
    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

}