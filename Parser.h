#ifndef PARSER_H
#define PARSER_H

#include <string>

using namespace std;
class Parser {
  /*
  1 - /travelRequest citizenID  date countryFrom CountryTo virusName
  2 - /travelStats virusName date1 date2 [country]
  3 - /addVaccinationRecords country
  4 - /searchVaccinationStatus CitizenID
  5 - /exit

  */
public:
  string *args; // array that i keep all the args given
  int numArgs;
  int command;
  void
  clean(); // this function is cleaning the data from previous command in main
  Parser();
  ~Parser();
  int parse();
};
#endif