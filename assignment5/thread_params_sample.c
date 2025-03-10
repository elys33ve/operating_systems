#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*  passing parameters to threads */
                      
double c[100];  // stored in process DATA segment with global scope.

typedef struct arg { 
    int start_index;  // Input parameter
    int end_index;    // Input parameter
    double sum;       // Space for return value
}  arg_type;


typedef arg_type *arg_ptr;

                     
void *sum_array_elements(void *input) {
    // send in ALL the parameters as a single pointer to struct.
    // send pointer as void type, but cast it to the right type INSIDE the function.  
    int i;                
    int start_index;
    int end_index;
    double sum;
    
    // copy parameters FROM the parameter block into local variables for use.
    // also could have used the pointer arithmetic expressions directly, but 
    // for sake of clarity (if not efficiency) I'm doing it here.
    start_index = ((arg_ptr)input)->start_index;
    end_index   = ((arg_ptr)input)->end_index;

    // sum of all elements of c[] between the start and end indices.  
    // Note we're doing NO error checking on bounds here.
    sum = 0.0;
    for(i=start_index; i <= end_index; i++)
       sum += c[i];
     
    // How do we return values from threads? Lots of options here.  
    // but here, we're stuffing into the struct used to get stuff INTO the thread...
    ((arg_ptr)input)->sum = sum;
      
    pthread_exit(0);
}


int main() {
    int i;  // counter to fill array c[] with stuff. Local to main()
    double average; // average of all of the values in c[]
    int rt1, rt2;   // return values from the routine that launches threads 
    pthread_t t1, t2;   // variables of pthread_t to hold bookkeeping values for threads.
    arg_type args_1, args_2;    // structs to hold input parameters to the threads

    // put some random data into the global array c[]
    time_t t;
    srand((unsigned)time(&t));        
    for (i=0; i < 100; i++)
        c[i] = (double)((rand()%1000)/1000.0);
       
    int trial_count = 0;    // keep track of current trial
    
    // each trial, zero out the counter and run the count routine “twice”
    // as threads that can be scheduled onto independent cores instead of running
    // in sequence.
    for (trial_count = 0; trial_count < 10; trial_count++) {  
        // thread 1 works on first half of c[] and thread two works on second half
        args_1.start_index = 0;
        args_1.end_index   = 49;
        args_2.start_index = 50;
        args_2.end_index   = 99;

        // create two thread with the routine pthread_create()
        if((rt1=pthread_create( &t1, NULL, sum_array_elements, (void *)&args_1)))                  
            printf("Thread creation failed: %d\n", rt1);
        if((rt2=pthread_create( &t2, NULL, sum_array_elements, (void *)&args_2)))
            printf("Thread creation failed: %d\n", rt2);

        // wait for threads to finish. the main process thread blocks until threads terminate.
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        // take two partial sums and compute average of ALL values in c[].
        average = (args_1.sum + args_2.sum)/100.0;
        printf("The average of all the values in array c[] is %lf\n", average);
        printf("\n");
    }

    return 0;
}
