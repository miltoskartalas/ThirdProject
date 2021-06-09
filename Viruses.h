#ifndef VIRUSES_H
#define VIRUSES_H
#include "BloomFilter.h"
#include "BloomList.h"
#include "Hash.h"
#include "SkipList.h"
#include <iostream>
#include <string>

using namespace std;

typedef class SkipList_list SkipList_list;
typedef class SkipListNode SkipListNode;
typedef class CitizenNode CitizenNode;

class VirusNode { // node for the VirusList
private:
  string VirusName;
  BloomFilter *bloomFilter;
  SkipList_list *vaccinatedSkipList;
  SkipList_list *notVaccinatedSkipList;

public:
  VirusNode *next;
  VirusNode(string virus, int filterSize);
  ~VirusNode();
  void print() const { cout << this->VirusName << endl; }
  void addToBloomFilter(int citizenID);
  bool checkBloomFilter(int citizenID);
  SkipListNode *addToSkipList(bool isVaccinated, CitizenNode *Citizen);
  bool removeFromSkipList(int citizenID);
  SkipList_list *getSkipList(bool isVaccinated);
  string *getVirusName() { return &VirusName; }
  BloomFilter *getBloomFilter() { return bloomFilter; }
};

class VirusList {
private:
  VirusNode *start;
  int numViruses;

public:
  VirusList();
  ~VirusList();
  BloomList *getBloomList();
  VirusNode *getStart() { return start; };
  int getNumViruses() { return numViruses; }
  VirusNode *searchInVirusList(string VirusName);
  string *addVirus(string virusName, int filterSize);
  void vaccineStatusAll(int citizenID);
  void print();
  bool VirusPrintNotVaccSkiplist(string virusName);
};
#endif