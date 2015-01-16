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

using namespace std;

int* numcount(int *x, int n, int m);


int main( int argc, const char* argv[] ) {
  srand (time(NULL));  
  clock_t begin, end;
  double time_spent;
  printf( "\nStarting...\n");

  int* x = new int[16];
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  for(int i = 0 ; i < n ; i++)
  {
    x[i] = rand() % 101;
    //cout << x[i] << " ";
  }
  cout << "\n";
  
  begin = clock();
  numcount(x,n,m);
  end = clock();

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


string keyFromArray(int* array,int length);

int *numcount(int *x, int n, int m) {
  // For this version, we will use a map as a global hash table that all threads access.
  // For now, we're using strings as keys for the hashtable. We cannot use an array of ints, which would be ideal,
  // because you must use a constant value as a key - c++ doesn't want you to modify the key while it's in the hash table.
  // TODO: Utilize a hashtable that does not need the costly int->string conversion 
  //std::tr1::unordered_map<string,int> sequenceCounts;
  
  int bitsize = ceil(log2(n-m+1));
  node** hashtable = (node**) malloc(sizeof(node**)*((n-m+1)));
  int length = (n-m+1);
  for(int i = 0; i < length; i++)
  {
    hashtable[i] = NULL;
  }
  //int* sequence = new int[1000];
  // Start the threads
  #pragma omp parallel
  {
    int offset = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    #pragma omp single
    {
      printf("Num threads = %d\n", numThreads);
    }
    // Iterate through all subsequences
    #pragma omp for
    for(int i = 0 ; i < n-m+1 ; i++) {
      // Convert the subsequence of integers to a string **vomit**
      #pragma omp critical
      {
      // Don't write without starting a critical section
         uint32_t hash32 = hash(&x[i],m);
         hash32 = (((hash32>>bitsize) ^ hash32) & TINY_MASK(bitsize));
         if(hashtable[hash32] == NULL)
         {
          node* newnode = (node*)malloc(sizeof(node*));
          newnode->array = (int**)malloc(sizeof(int**));
          newnode->array[0] = &x[i];
          newnode->count = (int*)malloc(sizeof(int*));
          newnode->count[0] = 1;
          newnode->amount = 1;
          hashtable[hash32] = newnode;
         }
         else
         {
         //possible collision
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
          if(collision == 1)
          {
            //reallocate the size
            hashtable[hash32]->array = (int**)realloc( hashtable[hash32]->array , ((hashtable[hash32]->amount)+1)*sizeof(int**) );
            hashtable[hash32]->count = (int*)realloc( hashtable[hash32]->count, ((hashtable[hash32]->amount)+1)*sizeof(int*) );
            
            hashtable[hash32]->array[hashtable[hash32]->amount] = &x[i];
            hashtable[hash32]->count[hashtable[hash32]->amount] = 1;
            (hashtable[hash32]->amount)++;
          }
          
         }
         
        // increase count by 1.
        // NOTE: If nothing is found, an element with count 0 is automatically inserted. This is a guaranteed behavior.

      } // end critical section

    } // reached end of array
  } // end of parallel processing. Implied break

  // Print the results of the hashtable
  /*
  cout << "Results:\n";
  for(typename std::tr1::unordered_map<string,int>::iterator kv = sequenceCounts.begin(); kv != sequenceCounts.end() ; kv++) {
    cout << "subseq: " << kv->first << " count : " << kv->second;
    cout << endl;
  } 
   */

  // TODO: convert output back to ints **vomit**
  return(x);
}

// Given an array of integers, converts that array to a string of comma-seperated integers.
std::string keyFromArray(int* array, int length) {
  string returnString;

  for(int i = 0; i < length ; i++) {
    returnString.append(static_cast<ostringstream*>(&(ostringstream() << array[i]) )->str());
    returnString.append(",");
  }

  return (returnString);
}
