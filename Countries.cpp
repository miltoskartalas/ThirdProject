#include "Countries.h"
#include <iostream>
#include <string>

using namespace std;

CountryNode::CountryNode(string CountryName) // constructor for my CountryNode
{
  this->CountryName = CountryName;
  this->next = nullptr;
}

CountryNode::~CountryNode(){}; // destructor for CountryNode

CountryList::CountryList() // constructor for CountryList
{
  this->numCountries = 0;
  this->start = nullptr;
}

string *CountryList::addCountryNode(string country) {
  CountryNode *current = start;
  CountryNode *prev;
  while (current) {
    if (*(current->getCountryName()) == country) {
      // country already exists
      return current->getCountryName(); // return ptr to country name
    }
    prev = current;
    current = current->next;
  }
  if (start) {
    current = new CountryNode(country);
    prev->next = current; // link previous node with new node
  } else {
    current = new CountryNode(country);
    start = current;
  }
  // reached end of list (current = nullptr, prev = last node)
  numCountries++;
  return current->getCountryName(); // return ptr to country name
}
string *CountryList::alreadyExists(string country) {
  CountryNode *current = start;
  while (current) {
    if (*(current->getCountryName()) == country) {
      // country already exists
      return current->getCountryName(); // return pointer to country name
    }
    current = current->next;
  }
  return nullptr; // country doesn't exists return null pointer
}

void CountryList::print() // print function for the list
{ // which calls for every CountryNode its print function
  CountryNode *current = start;
  while (current) {
    current->print();
    cout << " -> ";
    current = current->next;
  }
  cout << endl;
}

CountryList::~CountryList() // destructor for Countrylists
{                           // iterating all the list and deleting every node
  CountryNode *current = start;
  CountryNode *temp;
  while (current) {

    temp = current->next;
    delete current;
    current = temp;
  }
}
string CountryList::getCountryByName(int index) {
  CountryNode *current = start;
  int counter = 0;
  while (start) {
    // cout << *(current->getCountryName()) << endl;
    if (counter == index) {
      return *(current->getCountryName());
    }
    current = current->next;
    counter++;
  }
  return "";
}
string *CountryList::getCountryByIndex(int index) {
  CountryNode *temp = start;
  int finder = 0;
  while (temp) {

    if (finder == index) {

      return temp->getCountryName();
    }
    temp = temp->next;
    finder++;
  }
  return nullptr;
}