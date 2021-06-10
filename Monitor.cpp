#include "Buffer.h"
#include "Citizen.h"
#include "Countries.h"
#include "Viruses.h"
#include "functions.h"
#include <arpa/inet.h>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
string input_dir;
int fdr;
string directoryName;
int fdwr;
int sizeOfBloom;
VirusList *virusesList;
CitizenList *citizensList;
CountryList *countries = nullptr;
CountryList *DirCountries = nullptr;

int accepted = 0;
int rejected = 0;

int main(int argc, char **argv) {

  struct hostent *host;
  struct in_addr **addr_list;
  struct sockaddr_in client;
  struct sockaddr_in server;
  struct sockaddr *clientptr = (struct sockaddr *)&client;
  struct sockaddr *serverptr = (struct sockaddr *)&server;
  socklen_t clientlen = sizeof(clientptr);
  socklen_t serverlen = sizeof(serverptr);

  // taking arguments
  int port = atoi(argv[0]);
  int numThreads = atoi(argv[1]);
  int socketBufferSize = atoi(argv[2]);
  int cyclicBufferSize = atoi(argv[3]);
  int sizeOfBloom = atoi(argv[4]);
  int numPaths = argc - 5;
  string *paths = new string[numPaths];

  for (int i = 0; i < numPaths; i++) {
    string path(argv[5 + i]);
    paths[i] = path;
  }
  // in this list i am saving all my txts that each country has
  CountryList *countryFiles = pathsInDirectories(numPaths, paths);
  int numCountryPaths = countryFiles->getNumCountries();
  string *allCountryPaths = new string[numCountryPaths];

  for (int i = 0; i < numCountryPaths; i++) {

    allCountryPaths[i] = countryFiles->getCountryByName(i);
  }

  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    close(sock);
    perror("Socket creation failed");
  }
  int reuse = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, 4) < 0) {
    perror("Couldn't set Socket Options");
    close(sock);
    exit(EXIT_FAILURE);
  }
  char myPc[256];
  myPc[255] = '\0';
  int varGethostname = 0;
  if (varGethostname = gethostname(myPc, sizeof(myPc)) == -1) {
    perror("gethostname");
    close(sock);
    exit(1);
  }

  if ((host = gethostbyname(myPc)) == NULL) {
    perror("Problem with gethostbyname");
    close(sock);
    exit(EXIT_FAILURE);
  }
  char hostIP[256];
  hostIP[255] = '\0';
  addr_list = (struct in_addr **)host->h_addr_list;

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  memcpy(&(server.sin_addr), host->h_addr_list[0], host->h_length);

  if (bind(sock, serverptr, sizeof(*serverptr)) < 0) {
    perror("Binding went wrong");
    cout << errno << endl;
    close(sock);
    exit(EXIT_FAILURE);
  }
  if (listen(sock, 1) < 0) {
    perror("Server can't listen ");
    close(sock);
    exit(EXIT_FAILURE);
  }

  int socketFromAcc;
  if ((socketFromAcc = accept(sock, clientptr, &clientlen)) < 0) {
    perror("Couldn't accept");
    close(socketFromAcc);
    close(sock);
    exit(EXIT_FAILURE);
  }
  //  --------------------- Creating threads -------------------

  Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));

  threadInfo *threadArgs = (threadInfo *)malloc(sizeof(threadInfo));
  threadArgs->bloomSize = sizeOfBloom;
  CitizenList *citizensList = threadArgs->citizensList = new CitizenList();
  VirusList *virusesList = threadArgs->virusesList = new VirusList();
  CountryList *countriesList = threadArgs->countries = new CountryList();
  threadArgs->totalPaths = numCountryPaths;
  threadArgs->buffer = buffer;
  initialize(threadArgs->buffer, cyclicBufferSize, numCountryPaths);

  // cout << "Finished buffer" << endl;

  createThreads(numThreads, threadArgs);

  int completedPaths = 0;
  // giving to buffer the paths that it will need
  while (true) {
    place(buffer, allCountryPaths[completedPaths++]);
    if (completedPaths == numCountryPaths) {
      while (getPathsRead(buffer) < numCountryPaths) {
      }
      break;
    }
  }

  int numViruses = virusesList->getNumViruses();
  // send num viruses

  sentIntServer(numViruses, socketFromAcc);

  VirusNode *virus = virusesList->getStart();

  while (virus != nullptr) {
    // taking the name of the virus that his bloomfilters i am going to send
    // right above
    string sentVirus = *(virus->getVirusName());
    //  cout << "Sending " << sentVirus << endl;
    sentStringServer(sentVirus, socketBufferSize, socketFromAcc);

    int intsOfBloom = sizeOfBloom / sizeof(int);
    int leftovers = sizeOfBloom % sizeof(int);
    int i;
    int *BloomArray = virus->getBloomFilter()->getBloomArray();

    // sending till the amount of bytes that its division with buffer is
    // perfect
    for (i = 0; i < intsOfBloom; i++) {
      sentIntServer(BloomArray[i], socketFromAcc);
    }
    // here sending the reaminings if any
    if (leftovers > 0) {
      sentIntServer(BloomArray[i], socketFromAcc);
    }
    virus = virus->next;
  }

  // this is sent so to know that its finished
  sentIntServer(-1, socketFromAcc);

  readIntServer(socketFromAcc);

  //  waiting {for commands
  while (true) {

    string commandRecieved = readStringServer(socketBufferSize, socketFromAcc);
    string VirusNameRecieved = "";
    int citizenIDRecieved = 0;
    string answer = "";
    if (commandRecieved == "/travelRequest") {
      VirusNameRecieved = readStringServer(socketBufferSize, socketFromAcc);
      citizenIDRecieved = readIntServer(socketFromAcc);
    }
    VirusNode *skipV = virusesList->searchInVirusList(VirusNameRecieved);
    if (skipV != nullptr) {
      SkipList_list *skip = skipV->getSkipList(true);

      if (skip->search(&skip, citizenIDRecieved) == nullptr) {
        answer = "NO";

        sentStringServer(answer, socketBufferSize, socketFromAcc);
        rejected++;
      } else {
        answer = "YES";

        sentStringServer(answer, socketBufferSize, socketFromAcc);
        Date *dateOfVacc = skip->search(&skip, citizenIDRecieved)
                               ->getCitizen()
                               ->getVaccinations()
                               ->existsVaccination(VirusNameRecieved)
                               ->getDate();
        int day = (dateOfVacc->month);
        int month = (dateOfVacc->month);
        int year = (dateOfVacc->year);
        sentIntServer(day, socketFromAcc);
        sentIntServer(month, socketFromAcc);
        sentIntServer(year, socketFromAcc);
      }
    }
  }
}