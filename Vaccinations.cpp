#include "Vaccinations.h"
#include <iostream>
#include <string>

using namespace std;

Vaccination::Vaccination(string *virus, bool VaccStatus, Date *date)
{
  this->virus = virus;           //destructor for vaccination
  this->VaccStatus = VaccStatus; //vaccination is node for the list vaccinations
  this->date = date;
  this->next = nullptr;
}

Vaccination::~Vaccination()
{
  //destructor for each node, vacciantion
  if (date != nullptr)
  {
    delete date;
  }
}
Vaccinations::~Vaccinations()
{
  Vaccination *current = start; //destructor for the list which hold
  Vaccination *temp;            // each vaccination
  while (current != nullptr)
  {

    temp = current->next;
    delete current;
    current = temp;
  }
}

void Vaccination::setNewInfo(Date *date)
{                    //this function is used so that i can enter new VaccInfo
  VaccStatus = true; //for the command that enters new info
  if (this->date != nullptr)
  {
    delete this->date;
  }
  this->date = date;
}
Vaccinations::Vaccinations()
{
  start = nullptr;
}

Vaccination *Vaccinations::addVaccination(string *virus, bool VaccStatus, Date *date)
{
  Vaccination *current = start;
  Vaccination *prev; //fucntion that add to the list a new node
  while (current)    //node is vaccination
  {
    prev = current;
    current = current->next;
  }
  if (start)
  {
    prev->next = current = new Vaccination(virus, VaccStatus, date);
  }
  else
  {
    current = start = new Vaccination(virus, VaccStatus, date);
  }
  return current;
}

Vaccination *Vaccinations::existsVaccination(string virus)
{
  Vaccination *current = start; //function that searching in list
  while (current)               //with the string virus and sees if there is
  {
    if (current->getVirus() == virus)
    {
      return current;
    }
    current = current->next;
  }
  return nullptr;
}
