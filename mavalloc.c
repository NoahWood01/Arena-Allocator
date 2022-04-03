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

enum ALGORITHM algorithmType;

typedef struct _memory
{
  char descriptor;  //either P for process allocation or H for hole in memory
  // int startAddress;
  int length;
  struct _memory *prev;  //prev _memory block
  struct _memory *next;  //next _memory block
} _memory;

_memory* sentinel = NULL;
_memory* head; //head of linked list NOT SURE THE TYPE

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  if(size < 0) //fails if size is <0
  {
    return -1;
  }
  //else calls malloc
  //should create linked list??
  size_t requested_size = ALIGN4(size);   //code from assignment
  head = (_memory*)malloc(requested_size);   //creates memory of requested_size
  head->next = sentinel;
  head->descriptor = 'H';
  head->length = size;       //not sure if size or requested_size;
  algorithmType = algorithm;  //sets algorithm type
  
  return 0;
}

void mavalloc_destroy( )
{
  //destroy linked list

  free(head);
  head = sentinel;
  return;
}

void * mavalloc_alloc( size_t size )
{
  //depends on algorithm type?
  //must traverse linked list


  _memory* traversingNode = head; //traversing node


  //FIRST_FIT
  if(algorithmType == 0)
  {
    while(traversingNode != sentinel) //traverse linked list
    {
      if(traversingNode->descriptor == 'P')
      {
        //do nothing
      }
      else
      {
        //check if size argument can fit in hole
        if(traversingNode->length >= size)
        {
          //suitable location new _memory
          traversingNode->descriptor = 'P';
          traversingNode->next = (_memory*)(traversingNode + size);

          //new Block will be a hole
          _memory* newBlock = (_memory*)traversingNode->next;
          newBlock->descriptor = 'H';
          // newBlock->startAddress = traversingNode->next;
          newBlock->length = traversingNode->length - size;
          newBlock->prev = (_memory*)traversingNode;
          newBlock->next = (_memory*)(newBlock + newBlock->length);
          traversingNode->length = size;
          return traversingNode;
        }
      }
      traversingNode = traversingNode->next;
    }
  }
  //NEXT_FIT
  else if(algorithmType == 1)
  {

  }
  //BEST_FIT
  else if(algorithmType == 2)
  {

  }
  //WORST_FIT
  else if(algorithmType == 3)
  {

  }
  // only return NULL on failure
  return NULL;
}

void mavalloc_free( void * ptr )
{
  return;
}

int mavalloc_size( )
{
  int number_of_nodes = 0;
  _memory* traversingNode = head;
  while(traversingNode != sentinel)
  {
    if(traversingNode->descriptor == 'P')
    {
      number_of_nodes++;
    }
    traversingNode = traversingNode->next;
  }
  return number_of_nodes;
}

void printList()
{
  _memory* traversingNode = head;
  if(traversingNode == sentinel)
  {
    printf("emptylist\n");
  }
  while(traversingNode != sentinel)
  {
    printf("%x %c %d\n",traversingNode, traversingNode->descriptor,traversingNode->length);
    traversingNode = traversingNode->next;
  }
  printf("\n");
}
