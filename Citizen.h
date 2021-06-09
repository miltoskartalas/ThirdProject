#ifndef CITIZEN_H
#define CITIZEN_H

typedef struct Date Date;
#include "Countries.h"
#include "Vaccinations.h"
#include "Viruses.h"
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

typedef class Vaccination Vaccination;
typedef class Vaccinations Vaccinations;
typedef class VirusList VirusList;
typedef struct Date {
  int day;   // struct that will be created
  int month; // only when a citizen is vaccinatedd ( YES)
  int year;
} Date;
class CitizenNode { // node for the list of citizensList
private:            // here i am keeping all the obvious data for every citizen
  int CitizenID; // and a pointer to string country because many citizens can be
                 // from tha same country
  string FirstName; // so like this there is no data dublciation
  string LastName;
  string *country;
  int age;
  Vaccinations *vaccinations; // pointer to vaccinations so a citizen can have
                              // more than vaccination

public:
  CitizenNode *next;
  CitizenNode(int citizenID, string FirstName, string LastName, string *Country,
              int Age, string *Virus, bool VaccStatus, Date *date = nullptr);
  ~CitizenNode();
  CitizenNode(int id) : CitizenID(id){};
  string getFirstName() const {
    return FirstName;
  } // accessors and mutators for the private data
  string getLastName() const { return LastName; }
  int getCitizenID() const { return CitizenID; }
  int getCitizenAge() const { return age; }
  string *getCountryName() const { return country; }
  Vaccinations *getVaccinations() const { return vaccinations; }
  Vaccination *existsVaccination(string virus);
  void print();
};

class CitizenList { // CitizenList is a List of CitizenNodes
private:
  CitizenNode *start;
  int numCitizens; // in case the length of the list needed

public:
  CitizenList();  // constructor
  ~CitizenList(); // destructor
  // addCitizen is the function that adds a node to the list with all the data
  // needed, data that the citizenNode Class has as private data members
  CitizenNode *addCitizen(int citizenID, string FirstName, string LastName,
                          string *Country, int Age, string *Virus,
                          bool VaccStatus, Date *date = nullptr,
                          VirusList *viruses = nullptr);
  CitizenNode *
  searchInCitizenList(int citizenID); // search function for the list
  void print();
};

void printDate(Date *date); // function for my struct
Date *getCurentDate();
Date *getDate(string dateStr);
#endif
