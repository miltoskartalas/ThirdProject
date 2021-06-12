CC = g++ 
CFLAGS = -g -std=c++11

TRAVELOBJS= travelMonitorClient.o  Buffer.o Vaccinations.o SkipList.o MonitorCountries.o Citizen.o Viruses.o Countries.o  BloomFilter.o Hash.o ExceptionHandler.o FileDescriptor.o BloomList.o functions.o Parser.o
MONITOROBJS =   MonitorServer.o Buffer.o  MonitorCountries.o ExceptionHandler.o Countries.o Viruses.o Citizen.o Hash.o BloomFilter.o Vaccinations.o SkipList.o functions.o BloomList.o FileDescriptor.o


travelMonitorClient: clean $(TRAVELOBJS) $(MONITOROBJS)
	$(CC) $(CFLAGS)  -o travelMonitorClient $(TRAVELOBJS) -lpthread
	$(CC) $(CFLAGS) -o monitorServer $(MONITOROBJS) -lpthread

Countries.o:
	$(CC) $(CFLAGS)  -c Countries.cpp
Viruses.o:
	$(CC) $(CFLAGS)  -c Viruses.cpp
Citizen.o:
	$(CC) $(CFLAGS)  -c Citizen.cpp
Buffer.o:
	$(CC) $(CFLAGS) -c Buffer.cpp
travelMonitor.o:
	$(CC) $(CFLAGS)  -c travelMonitorClient.cpp
Hash.o:
	$(CC) $(CFLAGS)  -c Hash.cpp
BloomFilter.o:
	$(CC) $(CFLAGS)  -c BloomFilter.cpp
BloomList.o:
	$(CC) $(CFLAGS)  -c BloomList.cpp
Vaccinations.o:
	$(CC) $(CFLAGS)  -c Vaccinations.cpp
SkipList.o:
	$(CC) $(CFLAGS)  -c SkipList.cpp
Monitor.o:
	$(CC) $(CFLAGS) -c MonitorServer.cpp
ExceptionHandler.o:
	$(CC) $(CFLAGS) -c ExceptionHandler.cpp
FileDescriptor.o:
	$(CC) $(CFLAGS) -c FileDescriptor.cpp
functions.o:
	$(CC) $(CFLAGS) -c functions.cpp
MonitorCountries.o:
	$(CC) $(CFLAGS) -c MonitorCountries.cpp
Parser.o:
	$(CC) $(CFLAGS) -c Parser.cpp

.PHONY: clean 

clean:
	rm -rf $(MONITOROBJS) $(TRAVELOBJS) travelMonitorClient monitorServer logfiles/*