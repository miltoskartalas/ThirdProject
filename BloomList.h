#ifndef BLOOMLIST_H
#define BLOOMLIST_H
#include "BloomFilter.h"
#include <iostream>
#include <string>

using namespace std;

class BloomNode {
private:
  string virusName;
  BloomFilter *filter;

public:
  string getVirusName() { return virusName; }
  BloomNode(string virusName, BloomFilter *filter);
  BloomFilter *getFilter() { return filter; };
  ~BloomNode();
  BloomNode *next;
};

class BloomList {
private:
  BloomNode *start;

public:
  BloomList();
  ~BloomList();
  BloomNode *searchInBloomList(string virusName);
  void addBloomNode(string virusName, BloomFilter *filter);
  BloomNode *getNodeByIndex(int index);
  BloomNode *getStart() { return start; }
  void print();
};
#endif