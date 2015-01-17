// Ryan King
// I ran this in the CSIF with the following compile command:
// g++ -fopenmp -o P2.out Problem2.cpp

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

using namespace std;

int* numcount(int *x, int n, int m);

struct node{
  int* array;
  int count;
};


int main( int argc, const char* argv[] ) {
  srand (time(NULL));  
  double begin, end;
  printf( "\nStarting...\n");

  
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  int* x = new int[n];
  for(int i = 0 ; i < n ; i++)
  {
    x[i] = rand() % 101;
    cout << x[i] << " ";
  }
  cout << "\n";
  

  numcount(x,n,m);





  return(0);
}


string keyFromArray(int* array,int length);

int *numcount(int *x, int n, int m) {
  // For this version, we will use a map as a global hash table that all threads access.
  // For now, we're using strings as keys for the hashtable. We cannot use an array of ints, which would be ideal,
  // because you must use a constant value as a key - c++ doesn't want you to modify the key while it's in the hash table.
  // TODO: Utilize a hashtable that does not need the costly int->string conversion 
  std::tr1::unordered_map<string,node*> sequenceCounts;
  int subsequences = 0;
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
      string subsequence = keyFromArray(&(x[i]),m);

      // Don't write without starting a critical section
      #pragma omp critical
      {
        // increase count by 1.
        // NOTE: If nothing is found, an element with count 0 is automatically inserted. This is a guaranteed behavior.
        if(sequenceCounts[subsequence] == NULL)
        {
          sequenceCounts[subsequence] = new node;
          sequenceCounts[subsequence]->array = &(x[i]);
          subsequences++;
        }
        (sequenceCounts[subsequence]->count)++;
      } // end critical section
    } // reached end of array
  } // end of parallel processing. Implied break
  // Print the results of the hashtable
  cout << "Results:\n";
  int* outputarray = (int*)malloc(sizeof(int*) * (subsequences*m+1));
  int currsubsequence = 0;
  for(typename std::tr1::unordered_map<string,node*>::iterator kv = sequenceCounts.begin(); kv != sequenceCounts.end() ; kv++) {
    for(int i = 0; i < m; i++)
    {
      outputarray[currsubsequence*(m+1) + i] = kv->second->array[i];
    }
    outputarray[currsubsequence*(m+1) + m] = kv->second->count;
  }    

  // TODO: convert output back to ints **vomit**
  return(outputarray);
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
