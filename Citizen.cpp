#include <iostream>
#include <sstream>
#include <string>

#include "Citizen.h"

using namespace std;

CitizenNode::CitizenNode(int citizenID, string FirstName, string LastName,
                         string *Country, int Age, string *Virus,
                         bool VaccStatus, Date *date) {
  this->next = nullptr; // this constructor is for the CitizeNode that is a node
                        // for the CitizenList
  this->CitizenID = citizenID; // every node has this data members and a pointer
                               // to vaccination so to be easier
  this->FirstName = FirstName; // for a citizen to has more than one vaccination
  this->LastName = LastName;
  this->country = Country;
  this->age = Age;
  this->vaccinations = new Vaccinations();
  vaccinations->addVaccination(Virus, VaccStatus, date);
}
void CitizenNode::print() {
  cout << "Citizen ID : " << CitizenID << endl;
  cout << "First Name : " << FirstName << endl;
  cout << "Last Name : " << LastName << endl;
  cout << "Country : " << *country << endl;
  cout << "Age : " << age << endl;

  cout << "\n\n";
}

CitizenNode::~CitizenNode() // destructor for citizeNode
{                           // here i am deletinig vaccinations
  // because every citizenNode has a pointer to a list
  if (vaccinations != nullptr) // of vaccinations (*vaccinations)
  {
    delete vaccinations;
  }
}

CitizenList::CitizenList() {
  this->numCitizens = 0;
  this->start = nullptr;
}
// function that add CitizenNode to the List
CitizenNode *CitizenList::addCitizen(int citizenID, string FirstName,
                                     string LastName, string *Country, int Age,
                                     string *Virus, bool VaccStatus, Date *date,
                                     VirusList *viruses) {
  CitizenNode *current = start;
  CitizenNode *prev;
  VirusNode *virusNode;
  while (current) {
    if (current->getCitizenID() == citizenID) { // looking for the citizen
      // citizen already exists
      // add vaccination info in list of vaccinations
      current->getVaccinations()->addVaccination(Virus, VaccStatus, date);
      if ((virusNode = viruses->searchInVirusList(*Virus))) {
        if (VaccStatus == true) {

          virusNode->addToBloomFilter(
              citizenID); // adding citizen to the bloom filter
        }

        virusNode->addToSkipList(
            VaccStatus, current); // and in the skiplist, my skiplistnode has a
                                  // pointer to every citizenNode
      }
      return current; // return pointer to citizen node
    }

    prev = current;
    current = current->next;
  }
  // citizen doesn't exist, create citizen node
  if (start) {
    current = new CitizenNode(citizenID, FirstName, LastName, Country, Age,
                              Virus, VaccStatus, date);
    prev->next = current; // link previous node with new node
  } else {
    current = new CitizenNode(citizenID, FirstName, LastName, Country, Age,
                              Virus, VaccStatus, date);
    start = current;
  }
  if ((virusNode = viruses->searchInVirusList(*Virus))) {
    if (VaccStatus == true) {

      virusNode->addToBloomFilter(citizenID);
    }
    virusNode->addToSkipList(VaccStatus, current);
  }
  return current; // return pointer to new node
}

Vaccination *CitizenNode::existsVaccination(string virus) {
  return vaccinations->existsVaccination(virus);
}

CitizenNode *CitizenList::searchInCitizenList(
    int citizenID) // this function is searching everynode
{ // in the citizen list an returns it if the citizen with the citizenID

  CitizenNode *current =
      start; // given is in the list, otherwise it returns nullptr
  while (current != nullptr) {
    if ((current->getCitizenID()) == citizenID) {
      // cout << "CITIZEN LIST " << current->getFirstName() << endl;
      // cout << " CITIZEN LIST " << current->getCitizenID() << endl;
      return current; // citizen already exists
    }
    current = current->next;
  }
  return nullptr; // citizen doesn't exist, return null pointer
}
void CitizenList::print() // print function for the list, this function simple
                          // calls for every node print
{
  CitizenNode *current = start;
  while (current) {
    current->print();
    current = current->next;
  }
}
CitizenList::~CitizenList() // citizenList destructor, it starts from the start
                            // and delete every node
{
  CitizenNode *current = start;
  CitizenNode *temp;
  while (current) {

    temp = current->next;
    delete current;
    current = temp;
  }
}

void printDate(Date *date) {
  cout << date->day << " " << date->month << " " << date->year << endl;
}

Date *getCurentDate() { // this function gets the current date from the system
  // so to use it in 7th command

  time_t tt;
  time(&tt);
  struct tm *ti;
  ti = localtime(&tt);

  Date *date = new Date();
  date->day = ti->tm_mday;
  date->month = ti->tm_mon + 1;
  date->year = ti->tm_year + 1900;
  return date;
}
Date *getDate(string dateStr) { // this function takes from the record, the date
  // and and initializing the struct date that is
  // already created in main
  Date *date = new Date();
  stringstream datestream(dateStr);
  string datesRead[3];
  int i = 0;
  while (datestream.good() && i < 3) {
    getline(datestream, datesRead[i], '-');
    i++;
  }
  date->day = stoi(datesRead[0]);
  date->month = stoi(datesRead[1]);
  date->year = stoi(datesRead[2]);
  return date;
}
