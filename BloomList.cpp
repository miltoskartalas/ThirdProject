#include "BloomList.h"
#include <iostream>
#include <string>
using namespace std;

BloomNode::BloomNode(string virusName, BloomFilter *filter) {
  this->virusName = virusName;
  this->filter = filter;
  this->next = nullptr;
}

BloomNode::~BloomNode() { delete filter; };

BloomList::BloomList() { this->start = nullptr; }
BloomList::~BloomList() {

  BloomNode *current = start;
  BloomNode *temp;
  while (current) {

    temp = current->next;
    delete current;
    current = temp;
  }
}

void BloomList::addBloomNode(string virusName, BloomFilter *filter) {
  BloomNode *current = start;
  BloomNode *newNode = nullptr;

  while (current && current->next) {
    current = current->next;
  }
  if (start == nullptr) {
    newNode = start = new BloomNode(virusName, filter);
  } else {
    newNode = current->next = new BloomNode(virusName, filter);
  }
}

BloomNode *
BloomList::searchInBloomList(string virusName) // this function is searching in
                                               // the Viruslist for a virusname
{ // if it is found it returns the pointer to the virusNode
  BloomNode *current =
      start; // if not, that means that does not exists, it will return nullptr
  while (current != nullptr) {

    if ((current->getVirusName()) == virusName) {
      // virus already exists
      return current; // return pointer to virusName
    }
    current = current->next;
  }
  // virus doesn't exist return null pointer
  return nullptr;
}

void BloomList::print() {
  BloomNode *current = start;

  while (current) {
    cout << (current->getVirusName()) << endl;
    current = current->next;
  }
}

BloomNode *BloomList::getNodeByIndex(int index) {
  BloomNode *current = start;
  int count = 0;
  while (current) {
    if (count == index) {
      return current;
    }
    count++;
    current = current->next;
  }
  return nullptr;
}