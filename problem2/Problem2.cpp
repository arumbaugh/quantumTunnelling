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
#include <iomanip>

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
    cout << x[i] << " ";
  }
  cout << "\n";
  
  begin = clock();
  numcount(x,n,m);
  end = clock();

  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  cout << "Execution time: " << time_spent << "\n";
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
    clock_t setup_time = 0, wait_time = 0, subseq_time = 0, begin, thread_time;
    setup_time = clock();
    thread_time = clock();
    int offset = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    #pragma omp single
    {
      printf("Num threads = %d\n", numThreads);
      cout << "No\tSetup\tWait\tSubSeq\tThread\n";
    }
    int maxSubsequenceIndex = (((n - 1) - offset) / numThreads);  
    setup_time = clock() - setup_time;
    // Iterate through all subsequences
    for(int subsequenceIndex = 0 ; subsequenceIndex < maxSubsequenceIndex ; subsequenceIndex++) {
      // Convert the subsequence of integers to a string **vomit**
      begin = clock();
      string subsequence = keyFromArray(&(x[(subsequenceIndex*numThreads)+offset]),m);
      subseq_time += clock() - begin;
      begin = clock();
      // Don't write without starting a critical section
      #pragma omp critical
      {
        wait_time += clock() - begin;
        // increase count by 1.
        // NOTE: If nothing is found, an element with count 0 is automatically inserted. This is a guaranteed behavior.
        sequenceCounts[subsequence] += 1;
      } // end critical section
    } // reached end of array
    thread_time = clock() - thread_time;
    #pragma omp critical
    { 
      cout << setprecision(2)<< offset << "\t" << (double)setup_time / CLOCKS_PER_SEC  << "\t" << (double)wait_time / CLOCKS_PER_SEC << "\t" 
                             << (double)subseq_time / CLOCKS_PER_SEC << "\t" << (double)thread_time / CLOCKS_PER_SEC << "\n";
    }
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
