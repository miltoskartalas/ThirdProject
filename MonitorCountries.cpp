#include "MonitorCountries.h"
#include <iostream>
#include <limits.h>
#include <string>
using namespace std;
// this structure is created so i can know for each monitor which countries
// has
MonitorCountryNode::MonitorCountryNode(string CountryName) {
  this->CountryName = CountryName;
  this->next = nullptr;
}

MonitorCountryNode::~MonitorCountryNode(){

};

MonitorCountriesList::MonitorCountriesList() {

  this->numCountries = 0;
  this->start = nullptr;
}

string *MonitorCountriesList::addMonitorCountryNode(string country,
                                                    int monitorID) {
  MonitorCountryNode *current = start;
  MonitorCountryNode *prev;
  while (current) {
    if (*(current->getMonitorCountryNodeName()) == country) {
      // country already exists
      return current->getMonitorCountryNodeName(); // return ptr to country name
    }
    prev = current;
    current = current->next;
  }
  if (start) {
    current = new MonitorCountryNode(country);
    current->setMonitorID(monitorID);
    prev->next = current; // link previous node with new node
  } else {
    current = new MonitorCountryNode(country);
    current->setMonitorID(monitorID);
    start = current;
  }
  // reached end of list (current = nullptr, prev = last node)
  numCountries++;
  return current->getMonitorCountryNodeName(); // return ptr to country name
}

void MonitorCountriesList::print() // print function for the list
{ // which calls for every CountryNode its print function
  MonitorCountryNode *current = start;
  while (current) {
    current->print();
    current = current->next;
  }
  cout << endl;
}

int MonitorCountriesList::getNumCountriesOfMonitor(int index) {
  MonitorCountryNode *current = start;
  int numCountries = 0;
  while (current) {
    if (current->getMonitorID() == index) {
      numCountries++;
    }
    current = current->next;
  }
  return numCountries;
}

string *MonitorCountriesList::getCountriesByMonitorIndex(int index) {
  MonitorCountryNode *current = start;
  string *countries = new string[getNumCountriesOfMonitor(index)];
  int currentCountryIndex = 0;
  while (current) {
    if (current->getMonitorID() == index) {
      countries[currentCountryIndex++] =
          *(current->getMonitorCountryNodeName());
    }
    current = current->next;
  }
  return countries;
}

MonitorCountriesList::~MonitorCountriesList() {
  MonitorCountryNode *current = start;
  MonitorCountryNode *temp;
  while (current) {

    temp = current->next;
    delete current;
    current = temp;
  }
}

int MonitorCountriesList::getMonitorIDbyCountryName(string CountryName) {
  MonitorCountryNode *temp = start;

  while ((temp->next != nullptr)) {

    if (*temp->getMonitorCountryNodeName() == CountryName) {
      return temp->getMonitorID();
    }
    temp = temp->next;
  }
  return INT_MIN;
}
MonitorCountryNode *
MonitorCountriesList::getMonitorByCountryName(string CountryName) {
  MonitorCountryNode *temp = start;

  while ((temp != nullptr)) {

    if (*temp->getMonitorCountryNodeName() == CountryName) {
      return temp;
    }
    temp = temp->next;
  }
  return nullptr;
}
void MonitorCountryNode::setMonitorID(int monitorID) {
  this->monitorID = monitorID;
}

string *MonitorCountriesList::getCountryByIndex(int index) {
  MonitorCountryNode *temp = start;
  int finder = 0;
  while (temp) {
    if (finder == index) {
      return temp->getMonitorCountryNodeName();
    }
    temp = temp->next;
  }
  return nullptr;
}