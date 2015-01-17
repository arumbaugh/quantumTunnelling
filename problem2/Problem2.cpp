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
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <cstring>
#include <iomanip>

using namespace std;
int outputarraylength = 0;


int* numcount(int *x, int n, int m);

void printOutputArray(int* array, int length, int m);


int main( int argc, const char* argv[] ) {
  srand (time(NULL));  
  clock_t begin, end;
  double time_spent;
  printf( "\nStarting...\n");

  
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  int* x = new int[n];
  for(int i = 0 ; i < n ; i++)
  {
    x[i] = rand() % 101;
    //x[i] = 10;
    //cout << x[i] << " ";
  }
  cout << "\n";
  
  begin = clock();
  int* output = numcount(x,n,m);
  end = clock();
  printf("hello! %d\n", output[0]);
  
  printOutputArray(output, outputarraylength, m);
  
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  cout << "Execution time: " << time_spent << "\n";
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
  int* count;
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
  node** hashtable = (node**) malloc(sizeof(node**)*(pow(2,bitsize)));
  int hashtablelength = pow(2,bitsize);
  int subsequences = 0;
  for(int i = 0; i < hashtablelength; i++)
  {
    hashtable[i] = NULL;
  }
  //int* sequence = new int[1000];
  // Start the threads
  #pragma omp parallel
  {
    clock_t setup_time = 0,  begin, thread_time, wait_time = 0, hash_time = 0, critical_time = 0;
    hash_time = thread_time = clock();
    int offset = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    #pragma omp single
    {
      printf("Num threads = %d ", numThreads);
      printf("length = %d ", hashtablelength);
      printf("\n");
      printf("%s \t %s \t %s \t %s \t %s \t %s\n","No","Setup","Wait","Hash","Crit", "Thread");
    }
    setup_time = clock() - setup_time;
    #pragma omp for
    for(int i = 0 ; i < n-m+1 ; i++) {   
      // Don't write without starting a critical section
      begin = clock();
      uint32_t hash32 = hash(&x[i],m);
      hash32 = (((hash32>>bitsize) ^ hash32) & TINY_MASK(bitsize));
      hash_time += clock() - begin;
      begin = clock();
      #pragma omp critical
      {
         wait_time += clock()-begin;
         begin = clock();
         //no entry yet for hash
         if(hashtable[hash32] == NULL)
         {
          node* newnode = (node*)malloc(sizeof(node*));
          newnode->array = (int**)malloc(sizeof(int**));
          newnode->array[0] = &x[i];
          newnode->count = (int*)malloc(sizeof(int*));
          newnode->count[0] = 1;
          newnode->amount = 1;
          hashtable[hash32] = newnode;
          subsequences++;
         }
        //possible collision
         else
         {
          int collision = 1;
          for(int j = 0; j < hashtable[hash32]->amount ; j++)
          {
            if(compareArray(hashtable[hash32]->array[j],&x[i],m) == 1)
            {
                (hashtable[hash32]->count[j])++;
                collision = 0;
                break;
            }
          }
          //collision
          if(collision == 1)
          {
            //reallocate the size
            hashtable[hash32]->array = (int**)realloc( hashtable[hash32]->array , ((hashtable[hash32]->amount)+1)*sizeof(int**) );
            hashtable[hash32]->count = (int*)realloc( hashtable[hash32]->count, ((hashtable[hash32]->amount)+1)*sizeof(int*) );
            
            hashtable[hash32]->array[hashtable[hash32]->amount] = &x[i];
            hashtable[hash32]->count[hashtable[hash32]->amount] = 1;
            (hashtable[hash32]->amount)++;
            subsequences++;
          }     
         }
       critical_time += clock() - begin;
      } // end critical section

    } // reached end of array
    //wait for all the threads to finish
    thread_time = clock() - thread_time;
    #pragma omp barrier
    #pragma omp critical
    { 
      printf("%d \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f\n", offset, 
      (double)setup_time / CLOCKS_PER_SEC, (double)wait_time / CLOCKS_PER_SEC, (double)hash_time / CLOCKS_PER_SEC,
      (double)critical_time / CLOCKS_PER_SEC, (double)thread_time / CLOCKS_PER_SEC);

    }
  } // end of parallel processing. Implied break
   //now we will place the results into the output array
  printf("Moving to output array...\n");
  int* outputarray = (int*) malloc(sizeof(int*) * (subsequences*(m+1)));
  outputarraylength = subsequences*(m+1);
  int currsubseqno = 0;
  for(int i = 0; i <  hashtablelength; i++)
  {
    if(hashtable[i]!=NULL)
    {
      for(int j = 0; j < hashtable[i]->amount; j++)
      {
        for(int k = 0; k < m; k++)
        {
          outputarray[currsubseqno*(m+1)+k] = hashtable[i]->array[j][k];
        }
        outputarray[currsubseqno*(m+1)+m]=hashtable[i]->count[j];
        currsubseqno++;
      }
      free(hashtable[i]->count);
      free(hashtable[i]->array);
      free(hashtable[i]);
    }
    if(currsubseqno >= subsequences)
      break;
  }
  printf("Done copying to output array...\n");
  free(hashtable);
  printf("\n");
} 


void printOutputArray(int* array, int length, int m)
{
  for(int i = 0; i < length/(m+1); i++)
  {
    printf("Subsequence: ");
    for(int j = 0; j<m;j++)
    {
      printf("%d, ", array[i*(m+1)+j]);
    }
    printf(" Count: %d\n", array[i*(m+1)+m]); 
  }
}
