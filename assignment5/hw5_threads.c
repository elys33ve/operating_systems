#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARR_SIZE 100

// global int arrays
int unsorted_arr[ARR_SIZE];
int sorted_arr[ARR_SIZE];

/*  ASSIGNMENT 5 - Task 02: Multi-Threaded Sort Program (Textbook Ch4: Project 2)
Fiona O'Connell

Multithreaded sorting program:
    - List of integers (unsorted_arr) is divided into two sublists of equal size. 
    - Two separate threads sort each sublist using merge sort. 
        - Each thread works on half of the global array.
    - Third merging thread merges the sublists into one sorted list (sorted_arr).

-- passes parameters to each of the sorting threads with struct.
-- parent thread outputs the sorted array once all sorting threads have exited.
*/

// struct to pass parameters to threads
typedef struct arg { 
    int start_index;
    int end_index;
} arg_type;
typedef arg_type *arg_ptr;

// ================================================================

// --- Print array values
void print_arr(int n) {
    for (int i=0; i<ARR_SIZE; i++) {
        if (n == 1) {
            printf("%d ", unsorted_arr[i]);
        } else {
            printf("%d ", sorted_arr[i]);
        }

        // print 20 values per line
        if (i != 0 && i % 20 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

// ================================================================

// --- Merge Sort functions
void merge(int arr[], int l, int m, int r, int dest[]) {
    // merge two sub-arrays
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];   // temp arrays

    // copy data to temp arrays
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // merge temp arrays into dest
    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            dest[k] = L[i];
            i++;
        } else {
            dest[k] = R[j];
            j++;
        }
        k++;
    }

    // copy any remaining L[] elements
    while (i < n1) {
        dest[k] = L[i];
        i++;
        k++;
    }
    // copy any remaining R[] elements
    while (j < n2) {
        dest[k] = R[j];
        j++;
        k++;
    }
}
void mergeSort(int arr[], int l, int r, int dest []) {
    // recursive merge sort
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m, dest);
        mergeSort(arr, m + 1, r, dest);
        merge(arr, l, m, r, dest);
    }
}

// --- Merging thread function
void *merge_thread() {
    // merges unsorted_arr into sorted_arr after sorting threads finish
    int mid = (ARR_SIZE / 2) - 1;     // find middle idx

    merge(unsorted_arr, 0, mid, ARR_SIZE-1, sorted_arr); // merge into sorted_arr
    
    pthread_exit(NULL);
}

// ================================================================

// --- Sorting thread function
void *sort_thread(void *input) {
    // sorts half of global unsorted_arr[]
    int start_index, end_index;

    // copy parameters into local variables
    start_index = ((arg_ptr)input)->start_index;
    end_index = ((arg_ptr)input)->end_index;

    // sort sublists in unsorted_arr
    mergeSort(unsorted_arr, start_index, end_index, unsorted_arr);

    pthread_exit(0);
}

// ================================================================

int main() {
    int rt1, rt2, rt3;          // return values for threads
    pthread_t t1, t2, t3;       // hold bookkeeping values for threads
    arg_type args_1, args_2;    // thread input parameter structs

    // fill unsorted array with random data
    time_t t;
    srand((unsigned)time(&t));
    for (int i = 0; i < ARR_SIZE; i++) {
        unsorted_arr[i] = rand() % 1000;
    }

    // print unsorted list values
    printf("Unsorted array:\n");
    print_arr(1);        


    // sort thread 1: first half of unsorted_arr[]. 
    args_1.start_index = 0;
    args_1.end_index = (ARR_SIZE / 2) - 1;
    // sort thread 2: second half of unsorted_arr[]
    args_2.start_index = ARR_SIZE / 2;
    args_2.end_index = ARR_SIZE - 1;

    // -- sorting threads
    if((rt1=pthread_create( &t1, NULL, sort_thread, (void *)&args_1)))                  
        printf("Sort thread creation failed: %d\n", rt1);
    if((rt2=pthread_create( &t2, NULL, sort_thread, (void *)&args_2)))
        printf("Sort thread creation failed: %d\n", rt2);

    // wait for and join sorting threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // -- merge thread 
    if((rt3=pthread_create( &t3, NULL, merge_thread, NULL)))
        printf("Merge thread creation failed: %d\n", rt3);

    // wait for and join merge thread
    pthread_join(t3, NULL);


    // print sorted list values
    printf("Sorted array:\n");
    print_arr(2);

    return 0;
}





