// Ryan King
// I ran this in the CSIF with the following compile command:
// g++ -fopenmp -o P2.out Problem2.cpp

#define TINY_MASK(x) (((u_int32_t)1<<(x))-1)

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
// We are using an unordered map as our hash table for this version
#include <tr1/unordered_map>
#include <string>
#include <sstream>
#include <iostream>
#include <iterator>
#include <ctime>
#include <sys/time.h>
#include <math.h>
#include <stdint.h>
#include <cstring>
#include <iomanip>

using namespace std;
int* numcount(int *x, int n, int m);
double get_wall_time();
void printOutputArray(int* array, int m);


int main( int argc, const char* argv[] ) {
  srand (time(NULL));  
  double begin, end;
  printf( "\nStarting...\n");

  // length of array
  int n = atoi(argv[1]);
  // length of patterns  
  int m = atoi(argv[2]);

  // building the array to search through
  int* x = new int[n];
  for(int i = 0 ; i < n ; i++)
  {
    x[i] = rand() % 101;
    //x[i] = 10;
    cout << x[i] << " ";
  }
  cout << "\n";
  
  begin = get_wall_time();
  int* output = numcount(x,n,m);
  end = get_wall_time();


  printOutputArray(output, m);
 
  cout << "Execution time: " << end - begin << "\n";
  return(0);
}




//hashing function
//FNV hashing algorithm
// http://www.isthe.com/chongo/tech/comp/fnv/
uint32_t hash(int *data, int length)
{
    uint32_t hash = 2166136261;
    unsigned int FNV_prime = 16777619;
    for (int i = 0; i < length; i++)
    {
      hash = hash ^ data[i];
      hash = hash * FNV_prime;
    }
    return hash;
}
 
//
struct node{
  int** array;
  int amount;
};

int compareArray(int* array1, int* array2, int m)
{
  for (int i = 0; i < m; i++)
  {
    if(array2[i] != array1[i])return 0;
  }
  return 1;
}

void printArraySequence(int* array, int m)
{
  for(int k = 0; k < m; k++)
  {
    printf("%d, ",array[k]);
  }
}

string keyFromArray(int* array,int length);

int *numcount(int *x, int n, int m) {
  // For this version, we will use a map as a global hash table that all threads access.
  // For now, we're using strings as keys for the hashtable. We cannot use an array of ints, which would be ideal,
  // because you must use a constant value as a key - c++ doesn't want you to modify the key while it's in the hash table.
  

  int bitsize = ceil(log2(n-m+1));
  int hashtablelength = pow(2,bitsize);
  node** hashtable = (node**) malloc(sizeof(node**)*(hashtablelength));
  int subsequences = 0;
  // Array of locks for individual locking
  omp_lock_t* lock = (omp_lock_t*)malloc(sizeof(omp_lock_t*)*(hashtablelength));
  int* outputarray = (int*) malloc(sizeof(int*) * (1 + (n-m+1)*(m+1)) );
  outputarray[0] = 0;
  int currsubseqno = 0;
  omp_lock_t* outputindexlock = (omp_lock_t*)malloc(sizeof(omp_lock_t*));
  omp_init_lock(outputindexlock);
  for(int i = 0; i < hashtablelength; i++)
  {
    hashtable[i] = NULL;
    //initializes the array of locks
    omp_init_lock(&(lock[i])); 
  }

  // Start the threads
  #pragma omp parallel //for shared(lock)
  {    
    double setup_time = get_wall_time(), begin, thread_time, wait_time = 0, hash_time = 0, critical_time = 0;
    thread_time = get_wall_time();
    int offset = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    int outputindex = 0;
    #pragma omp single
    {
      printf("Num threads = %d ", numThreads);
      printf("length = %d ", hashtablelength);
      printf("\n");
      printf("%s \t %s \t %s \t %s \t %s \t %s\n","No","Setup","Wait","Hash","Crit", "Thread");
    }
    setup_time = get_wall_time() - setup_time;
    #pragma omp for
    for(int i = 0 ; i < n-m+1 ; i++) {   
      // Don't write without starting a critical section
      begin = get_wall_time();
      uint32_t hash32 = hash(&x[i],m);
      hash32 = (((hash32>>bitsize) ^ hash32) & TINY_MASK(bitsize));
      hash_time += get_wall_time() - begin;
      begin = get_wall_time();
 
//      #pragma omp critical
      omp_set_lock(&(lock[hash32]));
      {
         wait_time += get_wall_time()-begin;
         begin = get_wall_time();
         //no entry yet for hash
         if(hashtable[hash32] == NULL)
         {
          node* newnode = (node*)malloc(sizeof(node*));
          newnode->array = (int**)malloc(sizeof(int**));
          
          
          //aquire lock
          omp_set_lock(outputindexlock);
          {
            outputindex = currsubseqno;
            outputarray[0]++;
            currsubseqno++;
            subsequences++;
          }
          omp_unset_lock(outputindexlock);

          //load into the outputarray
          for(int j =0; j < m; j++)
          {
            outputarray[1+ outputindex * (m+1) + j] = x[i+j];
          }
          outputarray[1+ outputindex * (m+1) + m] = 1;
          newnode->array[0] = &outputarray[1+ outputindex * (m+1)];
          newnode->amount = 1;
          hashtable[hash32] = newnode;
          
         }
        //possible collision
         else
         {
          int collision = 1;
          for(int j = 0; j < hashtable[hash32]->amount ; j++)
          {
            if(compareArray(hashtable[hash32]->array[j],&x[i],m) == 1)
            {
                (hashtable[hash32]->array[j][m])++;                  
                collision = 0;
                break;
            }
          }
          //collision
          if(collision == 1)
          {
            //reallocate the size
            hashtable[hash32]->array = (int**)realloc( hashtable[hash32]->array , ((hashtable[hash32]->amount)+1)*sizeof(int**) );
            
            //aquire lock
            omp_set_lock(outputindexlock);
            {
              outputindex = currsubseqno;
              outputarray[0]++;
              currsubseqno++;
              subsequences++;
            }
            omp_unset_lock(outputindexlock);
            
            //load into the outputarray
            for(int j =0; j < m; j++)
            {
              outputarray[1+ outputindex * (m+1) + j] = x[i+j];
            }
            outputarray[1+ outputindex * (m+1) + m] = 1;
            
            hashtable[hash32]->array[hashtable[hash32]->amount] = &outputarray[1+ outputindex * (m+1)];
            (hashtable[hash32]->amount)++;
            
          }     
         }
       critical_time += get_wall_time() - begin;
      } // end critical section
      omp_unset_lock(&(lock[hash32]));

    } // reached end of array
    //wait for all the threads to finish
    thread_time = get_wall_time() - thread_time;
    #pragma omp critical
    { 
      printf("%d \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f\n", offset, 
      setup_time, wait_time,hash_time,
      critical_time, thread_time);
    }
  } // end of parallel processing. Implied break
   //now we will place the results into the output array
  outputarray[0]=subsequences;
  free(hashtable);
  free(lock);
  printf("\n");
  return(outputarray);
} 


void printOutputArray(int* array, int m)
{
  int subsequences = array[0];  
  for(int i = 0; i < subsequences; i++)
  {
    printf("Subsequence: ");
    for(int j = 0; j<m;j++)
    {
      printf("%d, ", array[1+ i*(m+1)+j]);
    }
    printf(" Count: %d\n", array[1+ i*(m+1)+m]); 
  }
}

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
