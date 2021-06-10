#include "BloomList.h"
#include "ExceptionHandler.h"
#include "FileDescriptor.h"
#include "MonitorCountries.h"
#include "Parser.h"
#include "functions.h"
#include "sysexits.h"
#include <arpa/inet.h>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>

#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {

  checkArgs(argc, argv); // check Arguments
  int numMonitors = atoi(argv[2]);
  int socketBufferSize = atoi(argv[4]);
  int cyclicBufferSize = atoi(argv[6]);
  int sizeOfBloom = atoi(argv[8]);

  string input_dir = argv[10];
  int numThreads = atoi(argv[12]);
  DIR *input_DIR = opendir(argv[10]); // Open input directory

  if (ENOENT == errno) {
    perror("Couldn't open input directory");
    exit(EXIT_FAILURE);
  }
  struct dirent *emptyDir;
  bool isEmpty = true;
  while ((emptyDir = readdir(input_DIR)) &&
         (strcmp(emptyDir->d_name, ".") || strcmp(emptyDir->d_name, ".."))) {
    isEmpty = false;
  }
  rewinddir(input_DIR);
  if (isEmpty) {
    errno = ENOENT;
    perror("Input directory is empty");
    exit(EX_USAGE);
  }
  // Count number of countries
  struct dirent *counter;
  int numOfCountries = 0;
  while ((counter = readdir(input_DIR)) != NULL) {
    if ((!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, ".."))) {
      continue;
    }
    if (counter->d_type == DT_DIR) {
      numOfCountries++;
    }
  }

  rewinddir(input_DIR);
  FileDescriptorList *fdList = new FileDescriptorList();
  int *mPIDS = new int[numMonitors];
  MonitorCountriesList *mCountriesList = new MonitorCountriesList();
  for (int i = 0; i < numMonitors; i++) {
    mPIDS[i] = createMonitors(i, input_DIR, numMonitors, numThreads,
                              socketBufferSize, cyclicBufferSize, sizeOfBloom,
                              mCountriesList, numOfCountries, input_dir);
  }
  // initializing connection with every server
  for (int i = 0; i < numMonitors; i++) {
    struct hostent *host;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    socklen_t serverlen = sizeof(*serverptr);

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket creation failed");
      close(sock);
    }
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, 4) < 0) {
      perror("Couldn't set Socket Options");
      close(sock);
      exit(EXIT_FAILURE);
    }
    // adding to my list sock that each server has
    fdList->addFileDescriptor(sock, i);
    char myPc[256];
    myPc[255] = '\0';
    if (gethostname(myPc, 255) == -1) {
      perror("gethostname");
      close(sock);
      exit(1);
    }
    if ((host = gethostbyname(myPc)) == NULL) {
      perror("Problem with gethostbyname");
      close(sock);
      exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length);
    server.sin_port = htons(PORT + i);

    int connected;
    do {
      connected = connect(sock, serverptr, sizeof(*serverptr));
    } while (connected < 0);

    // cout << "Connected to Port " << PORT + i << endl;
  }
  BloomList *BloomFilters = new BloomList();
  for (int i = 0; i < numMonitors; i++) {
    // recieving bloomfilters fro every monitor server
    int numViruses = readIntClient(fdList, i);
    readBloomFiltersFromMonitor(fdList, i, BloomFilters, socketBufferSize,
                                sizeOfBloom);
  }
  for (int i = 0; i < numMonitors; i++) {
    // when i recieve bloomFilters i sent -1 that means each finished
    sentIntClient(fdList, i, -1);
  }

  Parser *parser = nullptr;
  parser = new Parser();
  do {
    parser->clean();
    if ((parser->parse()) == EXIT_FAILURE) {
      continue;
    }
    switch (parser->command) {
    case 1: {
      string firstCommand = "/travelRequest";
      // string firstCommand = "/travelRequest";
      Date *date = nullptr;
      int citizenID = stoi(parser->args[0]);
      date = getDate(parser->args[1]);
      string countrFrom = parser->args[2];
      string countrTo = parser->args[3];
      string VirusName = parser->args[4];

      if (BloomFilters->searchInBloomList(VirusName) != nullptr) {

        BloomFilter *bloomF =
            BloomFilters->searchInBloomList(VirusName)->getFilter();

        if (bloomF->checkBloomFilter(citizenID) == false) {
          cout << "  REQUEST REJECTED – YOU ARE NOT VACCINATED LINE " << endl;
          break;
        }

      } else {
        cout << "  REQUEST REJECTED - YOU ARE NOT VACCINATED LINE " << endl;
        break;
      }

      MonitorCountryNode *search =
          mCountriesList->getMonitorByCountryName(input_dir + "/" + countrFrom);
      if (search == nullptr) {
        cout << " REQUEST REJECTED – YOU ARE NOT VACCINATED LINE " << endl;
        break;
      } else {
        int MonitorID = search->getMonitorID();
        int sock = fdList->getFileDescriptor(MonitorID);
        sentStringClient(fdList, firstCommand, socketBufferSize, MonitorID);
        sentStringClient(fdList, VirusName, socketBufferSize, MonitorID);
        sentIntClient(fdList, MonitorID, citizenID);

        // waiting to recieve yes or no from monitor
        string answerFromMonitor =
            readStringClient(fdList, socketBufferSize, MonitorID);

        if (answerFromMonitor == "YES") {
          int dayReturned = readIntClient(fdList, MonitorID);
          int monthReturend = readIntClient(fdList, MonitorID);
          int yearReturned = readIntClient(fdList, MonitorID);
          int distanceOfDates =
              (date->day + date->month * 30 + date->year * 360) -
              (dayReturned + 30 * monthReturend + 360 * yearReturned);

          if (distanceOfDates > 180) {
            cout << "REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION "
                    "BEFORE "
                    "TRAVEL DATE"
                 << endl;
          } else if (distanceOfDates >= 0) {
            cout << " REQUEST ACCEPTED – HAPPY TRAVELS" << endl;
          } else {
            cout << " REQUEST REJECTED – YOU ARE NOT VACCINATED LINE " << endl;
          }
        } else {
          cout << " REQUEST REJECTED - YOU ARE NOT VACCINATED LINE " << endl;
        }
      }

      break;
    }
    case 2: {

      break;
    }
    case 3: {

      break;
    }
    case 4: {

      break;
    }
    case 5: {
    }
    }
  } while (true);
}
