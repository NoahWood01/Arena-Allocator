// The MIT License (MIT)
//
// Copyright (c) 2022 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>

enum ALGORITHM allocationType;


enum TYPE
{
  FREE = 0,
  USED
};


typedef struct _memory
{
  size_t size;
  enum TYPE type;
  void* arena;
  struct _memory *prev;  //prev _memory block
  struct _memory *next;  //next _memory block
} _memory;

_memory* previous_node = NULL;
_memory* alloc_list;

void* arena;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  if(size < 0) //fails if size is <0
  {
    return -1;
  }
  //else calls malloc
  //should create linked list??
  allocationType = algorithm;  //sets algorithm type

  // get allgned size
  arena = malloc(ALIGN4(size));

  // allocate the head to the entire memory space
  alloc_list = (_memory*)malloc(sizeof(_memory));

  alloc_list->arena = arena;
  alloc_list->size = ALIGN4(size);
  alloc_list->type = FREE;
  alloc_list->next = NULL;
  alloc_list->prev = NULL;

  previous_node = alloc_list;
  return 0;
}

void mavalloc_destroy( )
{
  //destroy linked list
  _memory* node = alloc_list;
  alloc_list = NULL;
  while(node != NULL) //traverse alloc_list
  {
      _memory* temp = node->next;
      free(node);
      node = temp;
  }
  free(arena);
  return;
}

void * mavalloc_alloc( size_t size )
{
  //depends on algorithm type?
  //must traverse linked list


  _memory* traversingNode = alloc_list; //traversing node

  size_t aligned_size = ALIGN4(size);

  // if next fit get previous traversed node
  if(allocationType != NEXT_FIT)
  {
    traversingNode = alloc_list;
  }
  else if(allocationType == NEXT_FIT)
  {
    traversingNode = previous_node;
  }
  else
  {
    exit(0);
  }

  //FIRST_FIT
  if(allocationType == FIRST_FIT)
  {
    while(traversingNode != NULL) //traverse linked list
    {
        // open node for size needed
      if(traversingNode->size >= aligned_size && traversingNode->type == FREE)
      {
        // allocate for node
        int leftover_size = 0;

        traversingNode->type = USED;
        leftover_size = traversingNode->size - aligned_size;
        traversingNode->size = aligned_size;

        // turn left over space into new node
        if(leftover_size > 0)
        {
          _memory* previous_node = traversingNode->next;
          _memory* leftover_node = (_memory*)malloc(sizeof(_memory));

          leftover_node->arena = traversingNode->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_node;
          leftover_node->prev= traversingNode;

          traversingNode->next = leftover_node;
        }
        previous_node = traversingNode;
        return (void*)traversingNode->arena;
      }
      traversingNode = traversingNode->next;
    }
  }
  //NEXT_FIT
  //should be same functionality of FIRST_FIT until mavalloc_free is called
  else if(allocationType == NEXT_FIT)
  {
      // if no previous_node start at begining
      if(previous_node == NULL)
      {
          traversingNode = alloc_list;
      }
      _memory* start = traversingNode;
    do //traverse linked list but start with last node traversed and end there
    {
      if(traversingNode->size >= aligned_size && traversingNode->type == FREE)
      {
          // allocate for node
        int leftover_size = 0;

        traversingNode->type = USED;
        leftover_size = traversingNode->size - aligned_size;
        traversingNode->size = aligned_size;

        // turn left over space into new node
        if(leftover_size > 0)
        {
          _memory* previous_node = traversingNode->next;
          _memory* leftover_node = (_memory*)malloc(sizeof(_memory));

          leftover_node->arena = traversingNode->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_node;
          leftover_node->prev= traversingNode;

          traversingNode->next = leftover_node;
        }
        previous_node = traversingNode;
        return (void*)traversingNode->arena;
        }
      if(traversingNode->next == NULL)
      {
          traversingNode = alloc_list;
      }
      else
      {
          traversingNode = traversingNode->next;
      }
    }while(traversingNode != start);
  }
  //BEST_FIT
  else if(allocationType == BEST_FIT)
  {
      // temp will hold current winner
    _memory* tempAddress = NULL;
    int leftover_size = 0;
    int greatestLeftoverSpace = 2147483627;
    while(traversingNode != NULL)
    {
      if(traversingNode->size >= aligned_size && traversingNode->type == FREE)
      {

        if(traversingNode->size - aligned_size < greatestLeftoverSpace)
        {
            // store current winner
          greatestLeftoverSpace = traversingNode->size - aligned_size;
          tempAddress = traversingNode;
        }
      }
      traversingNode = traversingNode->next;
    }
    if(tempAddress != NULL)
    {
        // allocate for node
      tempAddress->type = USED;
      leftover_size = tempAddress->size - aligned_size;
      tempAddress->size = aligned_size;

      // turn left over space into new node
      if(leftover_size > 0)
      {
        _memory* previous_node = tempAddress->next;
        _memory* leftover_node = (_memory*)malloc(sizeof(_memory));

        leftover_node->arena = tempAddress->arena + size;
        leftover_node->type = FREE;
        leftover_node->size = leftover_size;
        leftover_node->next = previous_node;
        leftover_node->prev= traversingNode;

        (tempAddress)->next = leftover_node;

        previous_node = tempAddress;

      }
      return (void*)tempAddress->arena;
    }
  }
  //WORST_FIT
  else if(allocationType == WORST_FIT)
  {
      // temp will hold current winner
      _memory* tempAddress = NULL;
      int leftover_size = 0;
      int greatestLeftoverSpace = 0;
      while(traversingNode != NULL)
      {
        if(traversingNode->size >= aligned_size && traversingNode->type == FREE)
        {
          if(traversingNode->size - aligned_size >= greatestLeftoverSpace)
          {
              // store current winner
            greatestLeftoverSpace = traversingNode->size - aligned_size;
            tempAddress = traversingNode;
          }
        }
        traversingNode = traversingNode->next;
      }
      if(tempAddress != NULL)
      {
          // allocate for node
        tempAddress->type = USED;
        leftover_size = tempAddress->size - aligned_size;
        tempAddress->size = aligned_size;

        // turn left over space into new node
        if(leftover_size > 0)
        {
          _memory* previous_node = tempAddress->next;
          _memory* leftover_node = (_memory*)malloc(sizeof(_memory));

          leftover_node->arena = tempAddress->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_node;
          leftover_node->prev= traversingNode;

          (tempAddress)->next = leftover_node;

          previous_node = tempAddress;
        }
        return (void*)tempAddress->arena;
      }
  }
  // only return NULL on failure
  return NULL;
}

void mavalloc_free( void * ptr )
{
    _memory* node = alloc_list;

    while(node != NULL) //traverse alloc_list
    {

        if(node->arena == ptr)
        {
          _memory* tempNode = node; //defaults current node
          //check for prev is free
          //if so combine
          //then check if next is free then combine

          if(node->prev != NULL)
          {
            if(node->prev->type == FREE)
            {
              //combine free block
              tempNode = node->prev; //redefines tempNode to be previous node
              tempNode->size += node->size; //combine size;
              tempNode->next = node->next;

            }
          }

          if(node->next != NULL)
          {
            node->next->prev = tempNode; //updates next node's link to previous node
            if(node->next->type == FREE)
            {
              tempNode->size += node->next->size;
              tempNode->next = node->next->next; //combine size
            }
          }
          //free(ptr);
          tempNode->type = FREE;
          return;
        }
        node = node->next;
    }
  return;
}

int mavalloc_size( )
{
  int number_of_nodes = 0;
  _memory* traversingNode = alloc_list;
  // traverse until end of list
  while(traversingNode != NULL)
  {
    number_of_nodes++;
    // get next in list
    traversingNode = traversingNode->next;
  }
  return number_of_nodes;
}

void printList()
{
  _memory* traversingNode = alloc_list;
  // if null then list will be empty
  if(traversingNode == NULL)
  {
    printf("emptylist\n");
  }
  //traverse until end of list
  while(traversingNode != NULL)
  {
    printf("%x  %d %d\n",traversingNode->arena,traversingNode->size, traversingNode->type);
    traversingNode = traversingNode->next;
  }
  printf("\n");
}
