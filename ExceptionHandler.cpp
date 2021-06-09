#include "ExceptionHandler.h"

using namespace std;

int checkArgs(int argc, char **argv) {
  //Check if user provided required number of argsg

  if (argc < 12) {
    perror("Wrong number of arguments");
    exit(EXIT_FAILURE);
  } else if (strcmp(argv[1], "-m") || strcmp(argv[3], "-b") || strcmp(argv[5], "-c") || strcmp(argv[7], "-s") || strcmp(argv[9], "-i") || strcmp(argv[11], "-t")) {
    perror("Wrong arguments provided");
    exit(EXIT_FAILURE);
  } else if (atoi(argv[2]) <= 0 || atoi(argv[4]) <= 0 || atoi(argv[6]) <= 0 || atoi(argv[8]) <= 0 || atoi(argv[12]) <= 0) {
    perror("Arguments must have a positive value");
    exit(EXIT_FAILURE);
  }
  return 0;
}