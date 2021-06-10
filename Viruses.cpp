
#include "Viruses.h"
#include <iostream>
#include <string>

using namespace std;

VirusNode::VirusNode(string VirusName, int filterSize) {
  this->VirusName = VirusName; // constructor for viruses node
  this->bloomFilter = new BloomFilter(
      filterSize); // at this point i create and a skiplist with vaccinations
  this->vaccinatedSkipList =
      new SkipList_list(0, nullptr); // and  one with no vaccinataion
  this->notVaccinatedSkipList = new SkipList_list(0, nullptr);
  this->next = nullptr;
}
VirusNode::~VirusNode() // destructor for VirusNode
{                       // from this destructor are also deleted the skiplists
  if (vaccinatedSkipList != nullptr) // and the bloomfilter
  {
    delete vaccinatedSkipList;
  }
  if (notVaccinatedSkipList != nullptr) {
    delete notVaccinatedSkipList;
  }
  if (bloomFilter != nullptr) {
    delete bloomFilter;
  }
}

bool VirusNode::removeFromSkipList(
    int citizenID) // simple call remove function from skipllist
{
  return notVaccinatedSkipList->remove(&notVaccinatedSkipList, citizenID);
}

void VirusNode::addToBloomFilter(int citizenID) {
  string id = to_string(citizenID);
  unsigned char *citizenHash = (unsigned char *)id.c_str();
  int size = bloomFilter->getSize();

  for (int i = 0; i < 16; i++) {
    unsigned long bloomBit = hash_i(citizenHash, i) % (size);

    bloomFilter->changeBit(bloomBit);
  }
}

bool VirusList::VirusPrintNotVaccSkiplist(string virusName) {

  VirusNode *current = start;
  while (current) {
    if (*(current->getVirusName()) == virusName) {
      current->getSkipList(false)->print();
      return true;
    }
    current = current->next;
  }
  return false;
  // virus doesn't exist return falser
}
// simple call insert for the right skiplist depending on isVaccinated,so to
// distinguish if we need the vaccinatedskiplist, or notVaccinatedSkipList
SkipListNode *VirusNode::addToSkipList(bool isVaccinated,
                                       CitizenNode *Citizen) {
  if (isVaccinated) {
    return vaccinatedSkipList->insert(&vaccinatedSkipList, Citizen);
  }
  return notVaccinatedSkipList->insert(&notVaccinatedSkipList, Citizen);
}

VirusList::VirusList() {

  this->numViruses = 0;
  this->start = nullptr;
};

VirusNode *
VirusList::searchInVirusList(string virusName) // this function is searching in
                                               // the Viruslist for a virusname
{ // if it is found it returns the pointer to the virusNode
  VirusNode *current =
      start; // if not, that means that does not exists, it will return nullptr
  while (current) {
    if (*(current->getVirusName()) == virusName) {
      // virus already exists
      return current; // return pointer to virusName
    }
    current = current->next;
  }
  // virus doesn't exist return null pointer
  return nullptr;
}
SkipList_list *VirusNode::getSkipList(
    bool isVaccinated) // simple function that return both skipllists
{ // depending on which one we need, for this reason i have isVaccinated as
  // parameter
  if (isVaccinated) {
    return vaccinatedSkipList;
  }
  return notVaccinatedSkipList;
}

bool VirusNode::checkBloomFilter(int citizenID) // checking
{
  string id = to_string(citizenID);
  unsigned char *citizenHash = (unsigned char *)id.c_str();

  int size = bloomFilter->getSize();
  for (int i = 0; i < 16; i++) {
    unsigned long bloomBit = hash_i(citizenHash, i) % (8 * size);
    if (!bloomFilter->TestBit(bloomBit)) {
      // free(citizenHash);
      return false;
    }
  }
  // free(citizenHash);
  return true;
}
void VirusList::vaccineStatusAll(
    int citizenID) // function that prints all the vaccinated nodes
{
  VirusNode *current = start;
  string virusName;
  SkipList_list *vaccSkipList = nullptr;
  SkipListNode *foundNode = nullptr;
  while (current) {
    virusName = *(current->getVirusName());
    vaccSkipList = current->getSkipList(true);
    foundNode = vaccSkipList->search(&vaccSkipList, citizenID);
    if (foundNode) {
      cout << virusName << " YES ";
      printDate(
          foundNode->getCitizen()->existsVaccination(virusName)->getDate());
    } else {
      cout << virusName << " NO " << endl;
      ;
    }
    current = current->next;
  }
}

string *VirusList::addVirus(string virusName, int filterSize) {
  VirusNode *current = start;
  VirusNode *prev;
  while (current) {
    if (*(current->getVirusName()) == virusName) {
      // virus already exists
      return current->getVirusName(); // return pointer to virusName
    }
    prev = current;
    current = current->next;
  }
  if (start) {
    current = new VirusNode(virusName, filterSize);
    prev->next = current; // link previous node with new node
  } else {
    current = new VirusNode(virusName, filterSize);
    start = current;
  }
  numViruses++;
  // virus doesn't exist, create new virus node

  // return pointer to new virus name
  return current->getVirusName();
}

void VirusList::print() {
  VirusNode *current = start;
  while (current) {
    current->print();
    current = current->next;
  }
}

VirusList::~VirusList() {
  VirusNode *current = start;
  VirusNode *temp;
  while (current) {

    temp = current->next;
    delete current;
    current = temp;
  }
}
BloomList *VirusList::getBloomList() {
  BloomList *bloomList = new BloomList();
  VirusNode *current = start;
  while (current) {
    string virusName = *(current->getVirusName());
    BloomFilter *filter = current->getBloomFilter();
    bloomList->addBloomNode(virusName, filter);
    current = current->next;
  }
  return bloomList;
}
