#include "FileDescriptor.h"
#include <iostream>
#include <string>

using namespace std;

FileDescriptor::FileDescriptor(int socket, int MonID) {
  this->socket = socket;

  this->MonID = MonID;
}

FileDescriptor::~FileDescriptor() {
  if (next != nullptr) {
    delete next;
  }
};

FileDescriptorList::FileDescriptorList() {
  this->start = nullptr;
  this->numOfFileDescriptors = 0;
}

FileDescriptorList::~FileDescriptorList() {
  FileDescriptor *current = start;
  FileDescriptor *temp = nullptr;
  while (current != nullptr) {

    temp = current->next;
    delete current;
    current = temp;
  }
}

void FileDescriptorList::addFileDescriptor(int socket, int MonID) {
  FileDescriptor *toAdd = new FileDescriptor(socket, MonID);
  FileDescriptor *temp = start;

  if (temp == nullptr) {
    start = toAdd;
    numOfFileDescriptors++;
    return;
  }
  for (int i = 0; i < numOfFileDescriptors - 1; i++) {
    temp = temp->next;
  }
  temp->next = toAdd;

  numOfFileDescriptors++;
}

int FileDescriptorList::getFileDescriptor(int MonID) {
  FileDescriptor *temp = start;
  for (int i = 0; i < numOfFileDescriptors; i++) {
    if ((temp->getMonID() == MonID)) {
      return temp->getFileDescriptor();
    } else
      temp = temp->next;
  }
  return -1;
}

void FileDescriptorList::removeFileDescriptor(int MonID) {
  FileDescriptor *temp = start;
  FileDescriptor *prev = start;
  while (temp != nullptr) {
    if (temp->getMonID() == MonID) {
      FileDescriptor *next = temp->next;
      delete temp;
      if (MonID == 0) {
        start = next;
      } else {
        prev->next = next;
      }
      return;
    }
    prev = temp;
    temp = temp->next;
  }
}