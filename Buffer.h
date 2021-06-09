#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>
#include <pthread.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

typedef struct Buffer {
  int cyclicBufferSize;
  string *buffer;
  int start;
  int end;
  int count;
  int pathsRead;
  int totalPaths;
  pthread_mutex_t bufferMutex;
  pthread_cond_t cond_nonempty;
  pthread_cond_t cond_nonfull;

} Buffer;

void initialize(Buffer *buffer, int cyclicBufferSize, int count);
void place(Buffer *buffer, string path);
string obtain(Buffer *buffer);
int getPathsRead(Buffer *buffer);
void addPathRead(Buffer *buffer);
#endif