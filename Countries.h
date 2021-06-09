#ifndef COUNTRIES_H
#define COUNTRIES_H

#include <iostream>
#include <string>

using namespace std;

class CountryNode { // class CountryNode is the type of the node that my
private:            // class has
  string CountryName;

public:
  CountryNode *next;
  CountryNode(string CountryName);
  ~CountryNode();
  string *getCountryName() { return &CountryName; }
  void print() { cout << this->CountryName; };
};

class CountryList // CountryList is the list Of Countrynodes
{
private:
  CountryNode *start;
  int numCountries;

public:
  CountryList();
  ~CountryList();
  string *addCountryNode(string country); // add function to the list
  string *alreadyExists(string country);
  string
  getCountryByName(int index); // function that search all the list if there is
  int getNumCountries() { return numCountries; } // already the country
  string *getCountryByIndex(int index);
  void print();
};

#endif