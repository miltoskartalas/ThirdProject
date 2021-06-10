#include "Buffer.h"
#include <iostream>
using namespace std;

void initialize(Buffer *buffer, int cyclicBufferSize, int count) {

  buffer->start = 0;
  buffer->end = -1;
  buffer->count = 0;
  buffer->pathsRead = 0;
  buffer->totalPaths = count;
  buffer->cyclicBufferSize = cyclicBufferSize;

  buffer->buffer = new string[cyclicBufferSize];

  pthread_mutex_init(&(buffer->bufferMutex), 0);

  buffer->cond_nonempty = PTHREAD_COND_INITIALIZER;
  buffer->cond_nonfull = PTHREAD_COND_INITIALIZER;
}

void place(Buffer *buffer, string path) {
  pthread_mutex_lock(&(buffer->bufferMutex));

  while (buffer->count >= buffer->cyclicBufferSize) {
    pthread_cond_wait(&(buffer->cond_nonfull), &(buffer->bufferMutex));
  }

  buffer->end = (buffer->end + 1) % buffer->cyclicBufferSize;
  buffer->buffer[buffer->end] = path;
  buffer->count++;

  // cout << "[Place]  count " << buffer->count << " pathsRead : " <<
  // buffer->pathsRead << "path : " << path << endl;
  pthread_mutex_unlock(&(buffer->bufferMutex));

  pthread_cond_signal(&(buffer->cond_nonempty));
}

string obtain(Buffer *buffer) {
  string pathToGet;

  pthread_mutex_lock(&(buffer->bufferMutex));

  while (buffer->count <= 0) {
    pthread_cond_wait(&(buffer->cond_nonempty), &(buffer->bufferMutex));
  }
  pathToGet = buffer->buffer[buffer->start];
  buffer->start = (buffer->start + 1) % buffer->cyclicBufferSize;
  buffer->count--;
  // cout << "[Get]  count " << buffer->count << " pathsRead : " <<
  // buffer->pathsRead << " path : " << pathToGet << endl;
  pthread_mutex_unlock(&(buffer->bufferMutex));
  pthread_cond_signal(&(buffer->cond_nonfull));
  return pathToGet;
}

int getPathsRead(Buffer *buffer) {
  int pathsRead;
  pthread_mutex_lock(&(buffer->bufferMutex));
  pathsRead = buffer->pathsRead;
  pthread_mutex_unlock(&(buffer->bufferMutex));

  return pathsRead;
}

void addPathRead(Buffer *buffer) {
  pthread_mutex_lock(&(buffer->bufferMutex));

  buffer->pathsRead = (buffer->pathsRead) + 1;
  pthread_mutex_unlock(&(buffer->bufferMutex));
}