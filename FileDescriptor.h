#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include <iostream>
#include <string>

using namespace std;

class FileDescriptor {
private:
  int socket;

  int MonID;

public:
  FileDescriptor *next;
  FileDescriptor(int socket, int MonID);
  int getFileDescriptor() { return socket; };

  int getMonID() { return MonID; };
  ~FileDescriptor();
};

class FileDescriptorList {
private:
  FileDescriptor *start;
  int numOfFileDescriptors;

public:
  FileDescriptorList();
  ~FileDescriptorList();
  void addFileDescriptor(int socket, int MonID);
  int getFileDescriptor(int MonID);

  void removeFileDescriptor(int MonID);
};

#endif