/*
Solution v2 - uses pthread instead of thread
***************
Exercise: The following code uses two threads to find a digit in a range of numbers. 
Compile and check the run time. 

How can this be made more efficient?
*/
#include <iostream>
//#include <thread>
//#include <mutex>
#include <pthread.h>
#include <math.h>

using namespace std;

int flag = 1;
long range = 10000000000;
bool found;
//mutex found_mutex;
pthread_mutex_t found_mutex;

struct searchParam
{
    long first;
    long end;
    long x;
};

// When using pthread, return type of thread function needs to be 'void *'
// The function signature takes in the single parameter of type 'void *'. This needs to be converted to what you actually need. 
void * search(void * p) {
    // cast input p into the proper type
    struct searchParam * param = ((struct searchParam *) p);

    // extract out the values inside the parameter into local variables. 
    // This step can be omitted and the extraction can be done in-line in the loops. 
    long first, end, x;
    param -> first;
    param -> end;
    param -> x;

    for (long i=first; i<=end; i++){
        //found_mutex.lock();
        pthread_mutex_lock(&found_mutex);
        if (found) break;
        pthread_mutex_unlock(&found_mutex);
        //found_mutex.unlock();
        
        if (i==x){
            //found_mutex.lock();
            pthread_mutex_lock(&found_mutex);
            cout<<"Found it! i = " << i <<endl;
            found = true;
            pthread_mutex_unlock(&found_mutex);
            //found_mutex.unlock();
        }
    }
//      found_mutex.unlock();
}

int main(int argc, char const *argv[]) {
    long x = 10;
    found = false;

    // instantiate pthread and structure variables
    pthread_t t1, t2;
    searchParam p1, p2;

    // fill in parameter 1 to be passed as arguments for thread 1
    p1.first = 1;
    p1.end = ceil(range/2);
    p1.x = x;

    // fill in parameter 2 to be passed as arguments for thread 2
    p2.first = ceil(range/2);
    p2.end = range;
    p2.x = x;

    //thread t1(search, 1, ceil(range/2), x);
    //thread t2(search, ceil(range/2), range, x);

    pthread_create(&t1, NULL, search, &p1);
    pthread_create(&t2, NULL, search, &p2);
    
    //t1.join();
    //t2.join();

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
