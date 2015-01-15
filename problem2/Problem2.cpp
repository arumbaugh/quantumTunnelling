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

using namespace std;

int* numcount(int *x, int n, int m);

int main( int argc, const char* argv[] ) {
  printf( "\nStarting...\n");

  int* x = new int[16];
  for(int i = 0 ; i < 16 ; i++)
    x[i] = i%5;

  numcount(x,16,3);

  return(0);
}

string keyFromArray(int* array,int length);

int *numcount(int *x, int n, int m) {
  // For this version, we will use a map as a global hash table that all threads access.
  // For now, we're using strings as keys for the hashtable. We cannot use an array of ints, which would be ideal,
  // because you must use a constant value as a key - c++ doesn't want you to modify the key while it's in the hash table.
  // TODO: Utilize a hashtable that does not need the costly int->string conversion 
  std::tr1::unordered_map<string,int> sequenceCounts;
  
  // Start the threads
  #pragma omp parallel
  {
    int offset = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    int maxSubsequenceIndex = (((n - 1) - offset) / numThreads);  

    // Iterate through all subsequences
    for(int subsequenceIndex = 0 ; subsequenceIndex < maxSubsequenceIndex ; subsequenceIndex++) {
      // Convert the subsequence of integers to a string **vomit**
      string subsequence = keyFromArray(&(x[(subsequenceIndex*numThreads)+offset]),m);

      // Don't write without starting a critical section
      #pragma omp critical
      {
        // increase count by 1.
        // NOTE: If nothing is found, an element with count 0 is automatically inserted. This is a guaranteed behavior.
        sequenceCounts[subsequence] += 1;
      } // end critical section
    } // reached end of array
  } // end of parallel processing. Implied break

  // Print the results of the hashtable
  cout << "Results:\n";
  for(typename std::tr1::unordered_map<string,int>::iterator kv = sequenceCounts.begin(); kv != sequenceCounts.end() ; kv++) {
    cout << "subseq: " << kv->first << " count : " << kv->second;
    cout << endl;
  }    

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
