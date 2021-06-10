#include "functions.h"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
pthread_t *threadArray;

// function that read line by line from txt and filling all my data structures

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void addTofile(string fileName, CountryList **countriesList,
               VirusList **virusesList, CitizenList **citizensList,
               int bloomSize) {

  fstream file(fileName); // open file provided in arguments
  string str;

  while (getline(file, str)) {

    bool vaccinationStatus;
    stringstream ssin(str);
    string wordRead[8];
    int i = 0;
    while (ssin.good() && i < 8) {
      ssin >> wordRead[i];
      i++;
    }
    Date *date = nullptr;

    if (wordRead[6] == "NO") {
      vaccinationStatus = false;
    } else { // if he is vaccinated then i continue with the date
      vaccinationStatus = true;
      date = getDate(wordRead[7]);
    }
    // creating my lists
    string *country = (*countriesList)->addCountryNode(wordRead[3]);
    string *virus = (*virusesList)->addVirus(wordRead[5], bloomSize);
    CitizenNode *citizen = nullptr;
    CitizenNode *exists =
        (*citizensList)->searchInCitizenList(stoi(wordRead[0]));
    //ΝΑ ΤΣΕΚΑΡΩ ΟΤΙ ΘΑ ΠΑΡΩ ΙΔΙΟ ΟΝΟΜΑ ΚΤΛ ΓΙΑ ΚΑΠΟΟΙ ΠΡΟΗΓΟΥΜΕΝΟ ΙD
    if (exists && exists->existsVaccination(wordRead[5])) {
      // citizen already exists and has same virus name, skip record
      cout << "ERROR IN RECORD: " << endl;
      cout << stoi(wordRead[0]) << " " << wordRead[1] << " " << wordRead[2]
           << " " << *country << " " << stoi(wordRead[4]) << " " << *virus
           << " " << vaccinationStatus;
      if (vaccinationStatus) {
        cout << " " << wordRead[7];
      }
      cout << endl;
      if (date != nullptr) {
        delete date;
      }
      continue;
    }

    if (vaccinationStatus) {
      citizen = (*citizensList)
                    ->addCitizen(stoi(wordRead[0]), wordRead[1], wordRead[2],
                                 country, stoi(wordRead[4]), virus,
                                 vaccinationStatus, date, ((*virusesList)));

    } else { // else if citizen has not been vaccinated
      citizen = (*citizensList)
                    ->addCitizen(stoi(wordRead[0]), wordRead[1], wordRead[2],
                                 country, stoi(wordRead[4]), virus,
                                 vaccinationStatus, nullptr, (*virusesList));
    }
  }
}

pid_t createMonitors(int monitorIndex, DIR *input_DIR, int numMonitors,
                     int numThreads, int socketBufferSize, int cyclicBufferSize,
                     int sizeOfBloom, MonitorCountriesList *mCountriesList,
                     int numOfCountries, string input_dir) {
  // Find countries for monitor
  struct dirent *country;
  int monitorIndexRound = 0;

  while ((country = readdir(input_DIR))) {
    if (!strcmp(country->d_name, ".") || !strcmp(country->d_name, "..")) {
      continue; // ignore dots
    }
    if (country->d_type == DT_DIR) {
      // file is a directory
      if (monitorIndexRound % numMonitors == 0)
        monitorIndexRound = 0; // reset index for round robin

      string directory = country->d_name;
      if (monitorIndex == monitorIndexRound) {
        mCountriesList->addMonitorCountryNode(input_dir + "/" + country->d_name,
                                              monitorIndexRound);
      }
      monitorIndexRound++;
    }
  }
  rewinddir(input_DIR);
  string *paths = mCountriesList->getCountriesByMonitorIndex(monitorIndex);
  int numPaths = mCountriesList->getNumCountriesOfMonitor(monitorIndex);
  pid_t childpid;
  if ((childpid = fork()) < 0) {
    perror("Couldn't create Monitor");
    exit(EXIT_FAILURE);
  }
  if (childpid == 0) {

    char *arguments[6 + numPaths];
    for (int i = 0; i < 6 + numPaths; i++) {

      arguments[i] = new char[1024];
      arguments[i][1023] = '\0';
    }
    // converting to proper type arguments
    snprintf((char *)arguments[0], sizeof(long) + 1, "%d",
             (PORT + monitorIndex));
    snprintf((char *)arguments[1], sizeof(long), "%d", (numThreads));
    snprintf((char *)arguments[2], sizeof(long), "%d", (socketBufferSize));
    snprintf((char *)arguments[3], sizeof(long), "%d", (cyclicBufferSize));
    snprintf((char *)arguments[4], sizeof(long), "%d", (sizeOfBloom));

    int i = 0;
    while (i < numPaths) {
      arguments[5 + i] = (char *)paths[i].c_str();
      i++;
    }
    arguments[5 + numPaths] = NULL;
    execvp("./monitorServer", (char **)arguments);
    exit(EXIT_SUCCESS);
  } else {
    return childpid;
  }
}

void sentInitializingData(FileDescriptorList *fdList, int monitorIndex,
                          int bufferSize, int sizeOfBloom, int numOfCountries,
                          int numMonitors, string input_dir) {
  int writeFileDescriptor = (fdList->getFileDescriptor(monitorIndex));

  // cout << "TravelMonitor sending bufferSize to mon " << i << endl;
  int transferBufferSize = bufferSize;
  if (send(writeFileDescriptor, &transferBufferSize, sizeof(int), 0) == -1) {
    perror(" Couldnt sent bufferSize");
  }
  int transferSizeOfBloom = sizeOfBloom;

  if (send(writeFileDescriptor, &transferSizeOfBloom, sizeof(int), 0) == -1) {
    perror(" Couldnt sent sizeOfBloom");
  }
  // round robin
  int countriesPerMonitor = 0;
  if (monitorIndex < numOfCountries % numMonitors) {
    countriesPerMonitor = numOfCountries / numMonitors + 1;
  } else {
    countriesPerMonitor = numOfCountries / numMonitors;
  }
  if (send(writeFileDescriptor, &countriesPerMonitor, sizeof(int), 0) == -1) {
    perror(" Couldnt sent countries per monitor");
  }

  const char *sentInputDirectory = input_dir.c_str();
  int sizeOfsentDirectory = strlen(sentInputDirectory);

  if (send(writeFileDescriptor, &sizeOfsentDirectory, sizeof(int), 0) == -1)
    perror("Error writting to pipe size input_dir");

  int ptr = 0;
  int transferSize = bufferSize;
  if (bufferSize > sizeOfsentDirectory) {
    transferSize = sizeOfsentDirectory;
  }
  // creating list that will keep which countries each monitor has
  for (int i = 0; i <= sizeOfsentDirectory / bufferSize; i++) {
    if (i == sizeOfsentDirectory / bufferSize) {
      transferSize = sizeOfsentDirectory % bufferSize;
    }
    if (send(writeFileDescriptor, &sentInputDirectory[ptr], transferSize, 0) ==
        -1) {
      perror("Error writing to pipe input_dir ");
    }
    ptr += bufferSize;
  }
}

// function that sends  directories to each monitor

void sentCountriesDirectories(FileDescriptorList *fdList,
                              MonitorCountriesList *mCountriesList,
                              DIR *input_DIR, int numMonitors, int bufferSize) {
  struct dirent *country;
  int monitorIndexRound = 0;
  while ((country = readdir(input_DIR))) {
    if (!strcmp(country->d_name, ".") || !strcmp(country->d_name, "..")) {
      continue; // ignore dots
    }
    if (country->d_type == DT_DIR) {
      // file is a directory
      if (monitorIndexRound % numMonitors == 0)
        monitorIndexRound = 0; // reset index for round robin

      int writeFileDescriptor = fdList->getFileDescriptor(monitorIndexRound);
      mCountriesList->addMonitorCountryNode(country->d_name, monitorIndexRound);
      monitorIndexRound++;

      string directory = country->d_name;
      //  cout << "first " << directory << endl;
      const char *sentDirectory = directory.c_str();
      int sizeOfsentDirectory = strlen(sentDirectory);

      if (send(writeFileDescriptor, &sizeOfsentDirectory, sizeof(int), 0) ==
          -1) {
        perror("Error writting to pipe size of country");
      }
      // cout << "SUCCESSFUL WRITE " << endl;
      int ptr = 0;
      int transferSize = bufferSize;
      if (bufferSize > strlen(sentDirectory)) {
        transferSize = strlen(sentDirectory);
      }
      for (int j = 0; j <= strlen(sentDirectory) / bufferSize; j++) {
        // cout << "second " << sentDirectory[ptr] << endl;
        if (j == strlen(sentDirectory) / bufferSize) {
          transferSize = strlen(sentDirectory) % bufferSize;
        } // NA TO TSEKARW
        if (send(writeFileDescriptor, &sentDirectory[ptr], transferSize, 0) ==
            -1)
          perror("Error writing to pipe name of country ");
        ptr += bufferSize;
      }
    }
  }
}
// function that read bloom filters when they are ready to be send by monitors
void readBloomFiltersFromMonitor(FileDescriptorList *fdList, int monID,
                                 BloomList *BloomFilters, int bfSize,
                                 int sizeOfBloom) {
  string virusName = "";
  while (true) {
    virusName = readStringClient(fdList, bfSize, monID);
    // cout << "Travel Monitor got " << virusName << endl;
    if (virusName == "") {
      return;
    }

    int transferSize = bfSize;
    int bufferSize = bfSize;
    if (BloomFilters->searchInBloomList(virusName) == nullptr) {

      BloomFilter *filter = new BloomFilter(sizeOfBloom);
      BloomFilters->addBloomNode(virusName, filter);
    }

    int intsOfBloom = sizeOfBloom / sizeof(int);

    int *bloomF = BloomFilters->searchInBloomList(virusName)
                      ->getFilter()
                      ->getBloomArray();
    int leftovers = sizeOfBloom % sizeof(int);
    int i;
    for (i = 0; i < intsOfBloom; i++) {

      int bloomInt = readIntClient(fdList, monID);
      bloomF[i] = bloomF[i] | bloomInt;
    }
    //  bytes that may not be send because buffersize division with int
    // gives a module left
    if (leftovers > 0) {
      int bloomIntLeft = readIntClient(fdList, monID);
      bloomF[i] = bloomF[i] | bloomIntLeft;
    }
  }
}
// function that read String from server,  fdList is the list that has each sock
// travelMonitor uses this function
string readStringClient(FileDescriptorList *fdList, int bufferSize, int MonID) {

  int sock = fdList->getFileDescriptor(MonID);

  string wholeStr = "";
  int transportSize = bufferSize;

  int sizeOfWholeStr = readIntClient(fdList, MonID);

  if (sizeOfWholeStr == -1) {

    return "";
  }
  if (bufferSize > sizeOfWholeStr) {
    transportSize = sizeOfWholeStr;
  }

  for (int i = 0; i <= sizeOfWholeStr / bufferSize; i++) {
    char readPart[transportSize];
    // this if function is checking cases that the size of string
    // is not perfeclty divided by buffersize, so there will be some bytes
    // remaining that have to be sent
    if (i == sizeOfWholeStr / bufferSize) {
      transportSize = sizeOfWholeStr % bufferSize;
    }
    if (recv(sock, &readPart, transportSize, MSG_WAITALL) == -1) {
      perror("Couldnt read part of the string  ");
    }
    readPart[transportSize] = '\0';

    wholeStr.append(readPart);
  }
  // sentStringClient(fdList, "FIN", bufferSize, MonID);
  return wholeStr;
}
// function for server that reads from client
string readStringServer(int bufferSize, int sock) {

  string wholeStr = "";
  int transportSize = bufferSize;

  int sizeOfWholeStr = readIntServer(sock);
  if (bufferSize > sizeOfWholeStr) {
    transportSize = sizeOfWholeStr;
  }

  for (int i = 0; i <= sizeOfWholeStr / bufferSize; i++) {
    char readPart[transportSize];
    if (i == sizeOfWholeStr / bufferSize) {
      transportSize = sizeOfWholeStr % bufferSize;
    }
    if (recv(sock, &readPart, transportSize, MSG_WAITALL) == -1) {
      perror("Couldnt read part of the string  ");
    }

    readPart[transportSize] = '\0';
    wholeStr.append(readPart);
  }

  return wholeStr;
}
// function that sents string from client
void sentStringClient(FileDescriptorList *fdList, string sentString,
                      int bufferSize, int MonID) {

  int sock = fdList->getFileDescriptor(MonID);
  int sizeOfsentString = strlen(sentString.c_str());

  if (send(sock, &sizeOfsentString, sizeof(int), 0) == -1)
    perror("Error writting sizeOfsentString");

  int ptr = 0;

  int transportSize = bufferSize;
  if (bufferSize > sizeOfsentString) {
    transportSize = sizeOfsentString;
  }
  for (int i = 0; i <= sizeOfsentString / bufferSize; i++) {
    if (i == sizeOfsentString / bufferSize) {
      transportSize = sizeOfsentString % bufferSize;
    }
    if (send(sock, &sentString[ptr], transportSize, 0) == -1)
      perror("Error writing string ");
    ptr += transportSize;
  }
}
// sending string to server from client
void sentStringServer(string sentString, int bufferSize, int sock) {

  int sizeOfsentString = strlen(sentString.c_str());
  sentIntServer(sizeOfsentString, sock);
  int ptr = 0;

  int transportSize = bufferSize;
  if (bufferSize > sizeOfsentString) {
    transportSize = sizeOfsentString;
  }
  for (int i = 0; i <= sizeOfsentString / bufferSize; i++) {
    if (i == sizeOfsentString / bufferSize) {
      transportSize = sizeOfsentString % bufferSize;
    }
    if (send(sock, &sentString[ptr], transportSize, 0) == -1)
      perror("Error writing string ");
    ptr += transportSize;
  }

  // while (readStringServer(bufferSize, sock) != "FIN")
  //   ;
}
// function that reads int from server so fdlist is needed and monID
// so travelMonitor knows from which monitor will read

int readIntClient(FileDescriptorList *fdList, int MonID) {

  int sock = fdList->getFileDescriptor(MonID);
  int integer = 0;

  if (recv(sock, &integer, sizeof(int), MSG_WAITALL) == -1) {

    perror("couldnt read integer ");
  }
  return integer;
}

// function for server so to read int from client
int readIntServer(int sock) {
  int integer = 0;
  if (recv(sock, &integer, sizeof(int), MSG_WAITALL) == -1) {

    perror("couldnt read integer  ");
  }

  return integer;
}
// function that send int to server
void sentIntClient(FileDescriptorList *fdList, int MonID, int integer) {

  int sock = fdList->getFileDescriptor(MonID);

  if (send(sock, &integer, sizeof(int), 0) == -1) {
    perror(" Couldnt send integer  CLIENT ");
  }
}
// function that sent int to client
void sentIntServer(int integer, int sock) {
  //  cout << "sent int server " << integer << endl;
  if (send(sock, &integer, sizeof(int), 0) == -1) {
    perror(" Couldnt send integer SERVER ");
  }
}

// this function is the one that each thread will execute

void *threadFunction(void *args) {
  threadInfo *threadArgs = (threadInfo *)args;
  Buffer *buffer = threadArgs->buffer;
  int bloomSize = threadArgs->bloomSize;
  CitizenList *citizens = threadArgs->citizensList;
  CountryList *countries = threadArgs->countries;
  VirusList *viruses = threadArgs->virusesList;
  int totalPaths = threadArgs->totalPaths;

  int pathsRead;

  while (true) {
    string path = obtain(buffer);
    pthread_mutex_lock(&mtx);
    addTofile(path, &countries, &viruses, &citizens, bloomSize);
    pthread_mutex_unlock(&mtx);
    // cout << "Processed " << path << endl;
    addPathRead(buffer);
  }
  return nullptr;
}

void createThreads(int numThreads, threadInfo *threadArgs) {

  threadArray = new pthread_t[numThreads];

  for (int i = 0; i < numThreads; i++) {
    if (pthread_create(&threadArray[i], nullptr, threadFunction, threadArgs) !=
        0) {
      perror("Error in creating threads");
      exit(EXIT_FAILURE);
    }
  }
}
// this function gets paths from directories
CountryList *pathsInDirectories(int numdirs, string *dirs) {
  CountryList *countries = new CountryList();
  dirent *countryFile;
  for (int i = 0; i < numdirs; i++) {
    DIR *countryDir = opendir(dirs[i].c_str());
    while ((countryFile = readdir(countryDir)) &&
           (strcmp(countryFile->d_name, ".") ||
            strcmp(countryFile->d_name, ".."))) {
      if (countryFile->d_type == DT_REG) {
        string fileName = dirs[i] + "/" + countryFile->d_name;
        countries->addCountryNode(fileName);
      }
    }
  }

  return countries;
}
