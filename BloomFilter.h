#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H
#include <iostream>
#include <string>

using namespace std;

class BloomFilter { // bloom filter is just one class
private:            // with size and a pointer array
  int SizeOfBloomFilter;
  int *Array;

public:
  void print();
  bool checkBloomFilter(int citizenID);
  int TestBit(int k);
  void changeBit(int k);
  int *getBloomArray() { return Array; };
  int getSize() { return SizeOfBloomFilter * 32; };
  BloomFilter(int SizeOfBloomFilter);
  ~BloomFilter();
};
#endif