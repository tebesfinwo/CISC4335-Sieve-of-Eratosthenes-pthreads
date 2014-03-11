#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

/*
 The following structure contains the necessary information
 to allow the function "dotprod" to access its input data and
 place its output into the structure.
 */


/* Define globally accessible variables and a mutex */

#define NUMTHRDS 8
#define VECLEN 5000000
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;
double sum;
long Comp[VECLEN];

/*
 The function dotprod is activated when the thread is created.
 All input to this routine is obtained from a structure
 of type DOTDATA and all output from this function is written into
 this structure. The benefit of this approach is apparent for the
 multi-threaded program: when a thread is created we pass a single
 argument to the activated function - typically this argument
 is a thread number. All  the other information required by the
 function is accessed from the globally accessible structure.
 */

void *dotprod(void *arg)
{
    
    /* Define and use local variables for convenience */
    
    int i, start, end, len ;
    long offset;
    double mysum;
    offset = (long)arg;
    printf ("I am thread number  %ld \n", offset);
    len = VECLEN/NUMTHRDS;
    start = 1 + offset*len;
    end   = start + len;
    
    /*
     Perform the dot product and assign result
     to the appropriate variable in the structure.
     */
    
    mysum = 0;
    for (i=start; i<end ; i++)
    {
        mysum += i;
    }
    
    /*
     Lock a mutex prior to updating the value in the shared
     structure, and unlock it upon updating.
     */
    pthread_mutex_lock (&mutexsum);
    sum += mysum;
    pthread_mutex_unlock (&mutexsum);
    
    pthread_exit((void*) 0);
}

/**
 
 1 : composite 
 0 : prime
 
 */
void *setTotalPrimes(void *arg) {
    /* Define and use local variables for convenience */
    
    int i, start, end, len ;
    long offset;
    double mysum = 0;
    offset = (long)arg;
    printf ("I am thread number  %ld \n", offset);
    len = VECLEN/NUMTHRDS;
    start = offset*len;
    if (NUMTHRDS == 3 && offset == 2) {
        end   = start + len + 2;
    }else {
        end   = start + len ;
    }
    
    for (i = start; i < end; i++) {
        if (i < 3){
            if (i < 2) {
                Comp[i] = 1;
            }else{
                Comp[i] = 0;
            }
        }else{
            if (i % 2 == 0){
                Comp[i] = 1;
            }
        }
    }
    
    for (i = start; i < end; i++) {
        if (i > 2) {
            if (Comp[i] == 0) {
                for (int x = 3 ; x < i; x ++){
                    if ( x*x > i)
                        break;
                    if ( x % 2 != 0 && i % x == 0 ){
                        Comp[i] = 1;
                        break;
                    }
                }
            }
        }
    }
    
    for (i = start; i < end; i++) {
        if (i > 1 && Comp[i] == 0) {
            mysum++;
        }
    }

    
    /*
     Lock a mutex prior to updating the value in the shared
     structure, and unlock it upon updating.
     */
    pthread_mutex_lock (&mutexsum);
//    std::cout << "Start : " << start << "End : " << end << std::endl;
    sum += mysum;
    pthread_mutex_unlock (&mutexsum);
    
    
    pthread_exit((void*) 0);
}

/*
 The main program creates threads which do all the work and then
 print out result upon completion. Before creating the threads,
 the input data is created. Since all threads update a shared structure,
 we need a mutex for mutual exclusion. The main thread needs to wait for
 all threads to complete, it waits for each one of the threads. We specify
 a thread attribute value that allow the main thread to join with the
 threads it creates. Note also that we free up handles when they are
 no longer needed.
 */

int main (int argc, char *argv[])
{
    long i;
    void *status;
    pthread_attr_t attr;
    int input = 0;
    int isLoop = 1;
    
    
    
    sum = 0;
    do {
        std::cout << "Do you want to do a loop (1 : yes / 0 : no) ?" << std::endl;
        std::cin >> isLoop;
        std::cout << std::endl;
        if (isLoop != 1 && isLoop != 0 ) {
            std::cout << "Wrong input!!!" << std::endl;
        }
    } while (isLoop != 1 && isLoop != 0);
    
    
    
    pthread_mutex_init(&mutexsum, NULL);
    
    /* Create threads to perform the dotproduct  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    
	for(i=0; i<NUMTHRDS; i++)
    {
        /*
         Each thread works on a different set of data.
         The offset is specified by 'i'. The size of
         the data for each thread is indicated by VECLEN.
         */
        pthread_create(&callThd[i], &attr, setTotalPrimes, (void *)i);
	}
    
 	pthread_attr_destroy(&attr);
    
    /* Wait on the other threads */
	for(i=0; i<NUMTHRDS; i++)
    {
        pthread_join(callThd[i], &status);
	}
    
    /* After joining, print out the results and cleanup */
    printf ("Total number of Prime =  %f \n", sum);
    
    std::cout << std::endl;
    while (isLoop == 1) {
        std::cout << "Prime or Not" << std::endl;
        std::cout << "Please enter a number." << std::endl;
        std::cin >> input;
        if (input == 0){
            isLoop = 0;
            std::cout << "Program terminate" << std::endl;
        } else if(input == 1){
            std::cout << "You number " << input << " is not considered as Prime nor not Prime" << std::endl << std::endl;
        }else{
            if (input >= VECLEN) {
                std::cout << "Please enter a smaller number." << std::endl;
            }else {
                std::cout << "Your number "<< input << " is "<< ((Comp[input] == 0) ? "Prime" : "Not Prime") << std::endl << std::endl;
            }
        }
    }
    
//    for (int i = 0;  i < VECLEN; i++) {
//        std::cout << i << ":" << Comp[i] << std::endl ;
//    }
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
    
    

    
}
