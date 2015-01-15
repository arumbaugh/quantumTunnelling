#include "Problem2.cpp"
#include <random>
#include <chrono>


using namespace std::chrono;

int main( int argc, const char* argv[] ) {
  printf( "\nStarting...\n");
  int n = argv[1];
  int m = argv[2];

  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0,100);

  int* x = new int[size];
  for(int i = 0 ; i < n ; i++)
    x[i] = distribution(generator);

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  numcount(x,n,m);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

  cout << duration;

  return(0);
}
