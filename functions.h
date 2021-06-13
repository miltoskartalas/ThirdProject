#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "Buffer.h"
#include "Citizen.h"
#include "Countries.h"
#include "FileDescriptor.h"
#include "MonitorCountries.h"
#include "Viruses.h"
#include <dirent.h>
#include <pthread.h>
#include <string>

#define PORT 7777

typedef struct threadInfo {
  CountryList *countries;
  VirusList *virusesList;
  CitizenList *citizensList;
  string *paths;
  int bloomSize;
  Buffer *buffer;
  int totalPaths;

} threadInfo;

void addTofile(string fileName, CountryList **countries,
               VirusList **virusesList, CitizenList **citizensList,
               int bloomSize);

pid_t createMonitors(int monitorIndex, DIR *input_DIR, int numMonitors,
                     int numThreads, int socketBufferSize, int cyclicBufferSize,
                     int sizeOfBloom, MonitorCountriesList *mCountriesList,
                     int numOfCountries, string input_dir);
// void sentInitializingData(FileDescriptorList *fdList, int monitorIndex,
//                           int bufferSize, int sizeOfBloom, int
//                           numOfCountries, int numMonitors, string input_dir);
// void sentCountriesDirectories(FileDescriptorList *fdList,
//                               MonitorCountriesList *mCountriesList,
//                               DIR *input_DIR, int numMonitors, int
//                               bufferSize);

void readBloomFiltersFromMonitor(FileDescriptorList *fdList, int monID,
                                 BloomList *BloomFilters, int bfSize,
                                 int sizeOfBloom);

// Read/Send from Client
string readStringClient(FileDescriptorList *fdList, int bufferSize, int MonID);
int readIntClient(FileDescriptorList *fdList, int MonID);
void sentStringClient(FileDescriptorList *fdList, string sentString,
                      int bufferSize, int MonID);
void sentIntClient(FileDescriptorList *fdList, int MonID, int integer);

// Read/Send from  Server
void sentStringServer(string sentString, int bufferSize, int sock);
string readStringServer(int bufferSize, int sock);
void sentIntServer(int integer, int sock);
int readIntServer(int sock);

void createThreads(int numThreads, threadInfo *threadArgs,
                   pthread_t *threadArray);
CountryList *pathsInDirectories(int numdirs, string *dirs);

#endif
