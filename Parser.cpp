#include "Parser.h"
#include <iostream>
#include <sstream>
using namespace std;

// Parser is made in order to split the input arguments

Parser::Parser() {
  this->numArgs = 0;
  this->command = -1;
  this->args = new string[5]; // 8 because of the maximum arguments expected
  for (int i = 0; i < 5; i++) {
    this->args[i] = ""; // initializing
  }
}
void Parser::clean() // i have create this function so every time between
                     // commands
{ // this function is called and practically it deletes all the values
  this->numArgs =
      0; // that the data member of parser has ( numArgs, args array etc)
  this->command =
      -1; // so i dont have to delete the object and create every time a new one
          // just "clearing" the data from the previous command
  for (int i = 0; i < 5; i++) {
    this->args[i] = "";
  }
}
Parser::~Parser() // destructor for parser
{
  delete[] args;
}

int Parser::parse() // this function is the main function that get all the input
                    // from the
{                   // user
  cout << "Enter Command : " << endl;
  string line, command;
  cin.seekg(0, ios::end);
  cin.clear();
  getline(cin, line);
  stringstream ssin(line);
  ssin >> command;

  if (command[0] != '/') { // command doesn't begin with "/"
    cout << "Commands must begin with slash(/) " << endl;
    return EXIT_FAILURE;
  }

  while (ssin.good() && numArgs <= 4) {
    ssin >> args[numArgs++];
  }
  // if (numArgs == 5) {
  //   numArgs = 4;
  // }
  string commandList[5] = {// this array has all the commands so
                           // every time that a command is given
                           // i am checking if it is a one of them

                           "/travelRequest", "/travelStats",
                           "/addVaccinationRecords", "/searchVaccinationStatus",
                           "/exit"};
  int maxArgs[5] = {5, 4, 1, 1, 0};
  int minArgs[5] = {5, 3, 1, 1, 0};
  int i;
  for (i = 0; i <= 4; i++) {
    // searching my command in the array
    if (command.compare(commandList[i]) == 0) {
      this->command = i + 1;
      break;
    }
  }

  if (i == 5) {
    cout << "Command not found" << endl;
    return EXIT_FAILURE;
  }
  cout << this->command << endl;
  cout << numArgs << " " << minArgs[this->command - 1] << " "
       << maxArgs[this->command - 1] << endl;
  if (numArgs >= minArgs[this->command - 1] &&
      numArgs <= maxArgs[this->command - 1]) {
    return EXIT_SUCCESS;
  } else {
    cout << "Wrong number of arguments given" << endl;
    return EXIT_FAILURE;
  }
}