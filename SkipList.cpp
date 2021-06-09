#include "SkipList.h"
#include "Citizen.h"
#include <iostream>
#include <random>
#include <string>
#include <limits.h>

using namespace std;

SkipListNode::~SkipListNode() //destructor for the node
{
  if (next != nullptr)
  {
    delete next;
  }
} //constructor for the skiplist node, my Skiplist node has a pointer to citizen
SkipListNode::SkipListNode(CitizenNode *Citizen, SkipListNode *down)
{
  this->Citizen = Citizen; //node for the skiplist, every level of skiplist
  this->next = nullptr;    // will have this type of nodes
  this->down = down;
}
SkipList_list::~SkipList_list() //skiplist destructor for the list
{

  delete this->start;
  if (this->getDown() != nullptr)
  {
    delete this->getDown();
  }
}
SkipList_list::SkipList_list(int level, SkipList_list *down)
{
  this->level = level; //constructor for skiplist
  this->down = down;
  if (down != nullptr)
  {
    height = down->getLevel() + 1;
    SkipList_list *temp = down;
    while (temp != nullptr)
    {
      temp->SetHeight(height);
      temp = temp->getDown();
    }
    start = new SkipListNode(nullptr, down->getStart());
    end = new SkipListNode(nullptr, down->getEnd());
  }
  else
  {
    height = 0;
    start = new SkipListNode(nullptr, nullptr);
    end = new SkipListNode(nullptr, nullptr);
  }

  start->SetNext(end);
}

SkipListNode *SkipList_list::insert(SkipList_list **top, CitizenNode *Citizen)
{ //inserting starts from the very first level (top level) and continue down
  int count = 0;

  while (count < height + 1 && count < 9)
  {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec); // tv_nsec to change the value with nanoseconds so the function dont return the same thing every time

    if (rand() % 2) //calculating random probability for a node to be inserted and int he up level
    {
      count++;
    }
    else
    {
      break;
    }
  }
  SkipListNode *current = (*top)->getStart(); //inserting starts from the top of the lists
  SkipListNode *next = current->getNext();
  SkipList_list *currentLevel = *top;
  SkipListNode *upNode = nullptr;

  if (count > height) // if only a new level is needed i create one
  {                   // in order to avoid many levels with few nodes
    SkipList_list *newLevel = new SkipList_list(height + 1, *top);
    SkipListNode *insertNode = new SkipListNode(Citizen, nullptr);
    newLevel->getStart()->SetNext(insertNode);
    insertNode->SetNext(newLevel->getEnd());
    upNode = insertNode;
    *top = newLevel; //updating top with the new level, now my skiplist is higher
  }

  while (1)
  {
    if (Citizen->getCitizenID() < next->getCitizenID()) //κατεβαινουμε κατω
    {
      //if we are the lowest level, this is why pointer that points down is null, because there isn't and other level
      if (current->getDown() == nullptr)
      {
        SkipListNode *insertNode = new SkipListNode(Citizen, nullptr);
        current->SetNext(insertNode);
        insertNode->SetNext(next);
        if (count != 0)
        {
          upNode->SetDown(insertNode);
        }

        return insertNode;
      }
      else
      { //if i am in the right level
        if (currentLevel->getLevel() <= count)
        {
          SkipListNode *insertNode = new SkipListNode(Citizen, nullptr);
          current->SetNext(insertNode);
          insertNode->SetNext(next);
          if (upNode != nullptr) //an den einai to prwto insert
          {
            upNode->SetDown(insertNode);
          }
          upNode = insertNode;
        }
        current = current->getDown();
        next = current->getNext();
        currentLevel = currentLevel->getDown();
      }
    }
    else if (Citizen->getCitizenID() > next->getCitizenID()) //προχωραμε στο επομενο
    {
      current = next;
      next = next->getNext();
    }
    else
    {
      cout << "Citizen's IDS are the same " << endl;
      return nullptr;
    }
  }
}

SkipListNode *SkipList_list::search(SkipList_list **top, int citizenID)
{
  SkipListNode *current = (*top)->getStart(); // start searching from the top level
  SkipListNode *next = current->getNext();

  while (1)
  {
    if (citizenID < next->getCitizenID()) // if our citizen it is lower than the next one
    {                                     //we simple go down becuase we know that our list
                                          // is sorted it is not possible our value to exists
      //lowest level
      if (current->getDown() == nullptr)
      {
        return nullptr; // in case we reached the lowest level we are sure that the citizenId doesnot
      }                 // exists in the whole skiplist in every level
      else
      { //differently we are on the level we want

        current = current->getDown();
        next = current->getNext();
      }
    }                                          // then if our citizen id is bigger that the next one, we are searching in this
    else if (citizenID > next->getCitizenID()) //level untill we reach the end of the list, the last node which is defined for convient as INT_MAx
    {                                          //because we know that it is not possible to get a citizen id that big
      if (next->getCitizenID() == INT_MAX)
      {
        return nullptr;
      }
      current = next;
      next = next->getNext();
    }
    else
    {
      //      cout << "Citizen's IDS are the same " << endl;
      return next;
    }
  }
  return nullptr;
}
bool SkipList_list::remove(SkipList_list **top, int citizenID)
{

  SkipListNode *current = (*top)->getStart();
  SkipListNode *next = current->getNext();
  SkipList_list *currentLevel = *top;
  SkipListNode *upNode = nullptr;

  while (1)
  {
    if (citizenID < next->getCitizenID()) //κατεβαινουμε κατω
    {

      //sto katw epipedo
      if (current->getDown() == nullptr)
      {

        return false;
      }
      else
      { //an eimai sto swsto level

        current = current->getDown();
        next = current->getNext();
        currentLevel = currentLevel->getDown();
      }
    }
    else if (citizenID > next->getCitizenID()) //προχωραμε στο επομενο
    {
      current = next;
      next = next->getNext();
    }
    else
    {
      while (1)
      {
        SkipListNode *n = next->getNext();
        current->SetNext(n);
        SkipListNode *d = next->getDown();
        delete next;
        if (d != nullptr)
        {
          next = d;
          current = current->getDown();
          while (current->getNext() != next)
          {
            current = current->getNext();
          }
        }
        else
        {
          break;
        }
      }
      if (((*top)->getStart()->getNext()->getCitizenID() == INT_MAX) && (*top)->getDown() != nullptr)
      {
        SkipList_list *newTop = (*top)->getDown();
        delete *top;
        *top = newTop;
        SkipList_list *temp = *top;
        while (temp != nullptr)
        {
          temp->SetHeight(temp->getHeight() - 1);
          temp = temp->getDown();
        }
      }
      return true;
    }
  }
}

void SkipList_list::SetLevel(int level)
{
  this->level = level;
}
void SkipList_list::SetEnd(SkipListNode *end)
{
  this->end = end;
}
void SkipList_list::SetStart(SkipListNode *start)
{
  this->start = start;
}
void SkipList_list::SetHeight(int height)
{
  this->height = height;
}
void SkipList_list::SetDown(SkipList_list *down)
{
  this->down = down;
}
void SkipListNode ::SetNext(SkipListNode *next)
{
  this->next = next;
}

void SkipListNode ::SetDown(SkipListNode *down)
{
  this->down = down;
}

int SkipListNode::getCitizenID()
{
  if (Citizen == nullptr)
  {
    if (next == nullptr)
    {
      return INT_MAX;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return Citizen->getCitizenID();
  }
}

void SkipList_list::print() //this print calls for every node his print
{
  SkipList_list *temp = this;

  while (temp != nullptr)
  {
    if (temp->getDown() == nullptr)
    {
      temp->getStart()->print();
    }

    temp = temp->getDown();
  }
}

void SkipListNode ::print() //this print takes from the citizenNode tha the pointer οφ the skiplistNode points to all the data members of the citizen and print them
{
  SkipListNode *temp = this;
  temp = temp->getNext();

  while (temp != nullptr)
  {
    cout << temp->getCitizenID() << " " << temp->getCitizen()->getFirstName() << " " << temp->getCitizen()->getLastName() << " " << *(temp->getCitizen()->getCountryName()) << " " << temp->getCitizen()->getCitizenAge() << endl;

    temp = temp->getNext();
    if (temp->getCitizen() == nullptr)
    {
      break;
    }
  }
}