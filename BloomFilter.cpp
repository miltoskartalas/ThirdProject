#include "BloomFilter.h"
#include "Hash.h"
#include <iostream>
#include <string>

using namespace std;

BloomFilter::BloomFilter(
    int SizeOfBloomFilter) { // creating size of bloom filter
  this->SizeOfBloomFilter =
      SizeOfBloomFilter / (sizeof(int)); // dependin on input
  Array = new int[this->SizeOfBloomFilter];
  for (int i = 0; i < this->SizeOfBloomFilter; i++) {
    this->Array[i] = 0;
  }
};

BloomFilter::~BloomFilter() { delete[] Array; };

int BloomFilter::TestBit(int k) {
  int i = k / 32;
  int position = k % 32;

  unsigned int flag = 1;
  flag = flag << position;

  if (Array[i] & flag)
    return 1;
  else
    return 0;
};

void BloomFilter::changeBit(int k) {

  int i = k / 32;
  int position = k % 32;
  unsigned int flag = 1;
  flag = flag << position;

  Array[i] = Array[i] | flag;
};

bool BloomFilter::checkBloomFilter(int citizenID) // checking
{
  string id = to_string(citizenID);
  unsigned char *citizenHash = (unsigned char *)id.c_str();

  int size = this->getSize();
  for (int i = 0; i < 16; i++) {
    unsigned long bloomBit = hash_i(citizenHash, i) % (8 * size);
    if (!this->TestBit(bloomBit)) {
      // free(citizenHash);
      return false;
    }
  }
  // free(citizenHash);
  return true;
}