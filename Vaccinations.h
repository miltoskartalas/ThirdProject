#ifndef VACCINATIONS_H
#define VACCINATIONS_H
#include "Citizen.h"
#include <iostream>
#include <string>

using namespace std;

class Vaccination //vaccination is the node of the list Vaccinations
{
  string *virus;
  bool VaccStatus;
  Date *date;

public:
  Vaccination *next;
  Vaccination(string *virus, bool VaccStatus, Date *date);
  Date *getDate() const { return date; }
  bool isVaccinated() const { return VaccStatus; }
  ~Vaccination();
  string getVirus() const { return *virus; }
  void setNewInfo(Date *date);
};

class Vaccinations
{
  Vaccination *start;

public:
  Vaccination *addVaccination(string *virus, bool VaccStatus, Date *date = nullptr);
  Vaccination *existsVaccination(string virus);
  Vaccinations();
  ~Vaccinations();
};

#endif