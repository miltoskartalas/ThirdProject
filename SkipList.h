#ifndef SKIPLIST_H
#define SKIPLIST_H
#include "Citizen.h"
#include <iostream>
#include <string>

using namespace std;

typedef class CitizenNode CitizenNode;
class SkipListNode
{ //SkiplistNode is the node that every lavel has
private:
  CitizenNode *Citizen; //pointer to citizen so no data duplication
  SkipListNode *next;   //pointers so the connection from up levels to down is possible
  SkipListNode *down;

public:
  SkipListNode(CitizenNode *Citizen, SkipListNode *down);
  ~SkipListNode();
  int getCitizenID();                                 //return citizenID, through the pointer that the skiplistnode has to citizen, i can have access to the Id of the citizen
  CitizenNode *getCitizen() const { return Citizen; } //return the pointer that is in the node to citizen
  SkipListNode *getNext() { return next; }
  SkipListNode *getDown() { return down; }
  void SetCitizenID(int CitizenID);
  void SetNext(SkipListNode *next);
  void SetDown(SkipListNode *down);
  void print();
};

class SkipList_list
{
private: //SkipList_list is a class that represent a single level
  int level;
  int height;
  SkipListNode *start;
  SkipListNode *end;
  SkipList_list *down;

public:
  SkipList_list(int level, SkipList_list *down); //constructor, i need the level so to know which level i created every time
  ~SkipList_list();                              //destructor
  int getLevel() const { return level; }
  SkipListNode *getEnd() { return end; }
  SkipListNode *getStart() { return start; }
  void SetLevel(int level);
  void SetEnd(SkipListNode *end);
  void SetStart(SkipListNode *start);
  SkipListNode *insert(SkipList_list **top, CitizenNode *Citizen);
  SkipListNode *search(SkipList_list **top, int citizenID);
  bool remove(SkipList_list **top, int citizenID);
  void SetHeight(int height);
  SkipList_list *getDown() const { return down; }
  void SetDown(SkipList_list *down);
  int getHeight() const { return height; }
  void print();
};

#endif