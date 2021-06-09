#ifndef MONITORCOUNTRIES_H
#define MONITORCOUNTRIES_H

#include <iostream>
#include <string>

using namespace std;

class MonitorCountryNode {
private:
  string CountryName;
  int monitorID;

public:
  int pid;
  MonitorCountryNode *next;
  MonitorCountryNode(string CountryName);
  ~MonitorCountryNode();
  void setMonitorID(int monitorID);
  string *getMonitorCountryNodeName() { return &CountryName; }
  int getMonitorID() { return monitorID; }
  void print() { cout << this->CountryName << endl; }
};

class MonitorCountriesList {
private:
  MonitorCountryNode *start;

  int numCountries;

public:
  string *getCountryByIndex(int index);
  int getNumCountriesOfMonitor(int index);
  string *getCountriesByMonitorIndex(int index);
  int getMonitorIDbyCountryName(string CountryName);
  pid_t getMonitorPIDbyCountryName(string CountryName);
  MonitorCountryNode *getMonitorByCountryName(string CountryName);
  MonitorCountriesList();
  ~MonitorCountriesList();
  string *addMonitorCountryNode(string CountryName, int monitorID);
  int getMonitorNumCountries() { return numCountries; }
  void print();
};

#endif