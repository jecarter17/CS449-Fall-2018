#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "mymalloc.h"

struct Node{
	int occ;			// whether block is occupied
	int size;           // size of block (including header)
	struct Node *prev;  // pointer to previous node
	struct Node *next;  // pointer to next node
	void *base;			// pointer to base of block space
};

static struct Node *head = NULL;      	// head of list
static struct Node *end = NULL;			// end of list

void *my_malloc(int size){
	
	if(head == NULL){
		
		printf("Inititalizing head of list...\n");
		head = (struct Node *)sbrk(0);
		head->base = (void *)sbrk(sizeof(struct Node));
		sbrk(size);
		head->occ = 1;
		head->size = size;
		head->next = NULL;
		head->prev = NULL;
		end = head;
		return head->base;
	}else{
		
		int inc = size + 2*sizeof(struct Node);
		struct Node *curr = head;
		struct Node *temp = NULL;
	
		//allocate block according to next fit
		while(curr != end && (curr->occ == 1 || curr->size < inc)){
			
			printf("curr incrementing...\n");
			temp = curr;
			curr = curr->next;
			printf("curr.occ = %d\n", curr->occ);
			printf("curr.size = %d\n", curr->size);
		}
		
		if(curr == end){
			
			//create new node
			curr = (struct Node *)sbrk(0);
			curr->base = (void *)sbrk(sizeof(struct Node));
			sbrk(size);
			curr->occ = 1;
			curr->size = size;
			curr->next = NULL;
			curr->prev = temp;
			end = curr;
			return curr->base;
			
		}else if(curr->size > inc){
			if(curr->size > inc + sizeof(struct Node *)){
				//enough excess space, to break into two free blocks
				int blockSize = curr->size;
				curr->size = inc;
			}else{
				curr->size = size;
			}
			
			
		}else{
			//block is perfect, simply flip occ and return base ptr
			curr->occ = 1;
			return curr->base;
		}
	}
}

void my_free(void *ptr){
	
}

void dump_heap(){
  struct Node *cur;
  printf("%d", sizeof(struct Node));
  printf("brk: %p\n", sbrk(0));
  printf("head->");
  for(cur = head; cur != NULL; cur = cur->next) {
    printf("[%d:%d:%d]->", cur->occ, (char*)cur - (char*)head, cur->size);
    fflush(stdout);
    assert((char*)cur >= (char*)head && (char*)cur + cur->size <= (char*)sbrk(0)); // check that block is within bounds of the heap
    if(cur->next != NULL) {
      assert(cur->next->prev == cur); // if not last block, check that forward/backward links are consistent
      assert((char*)cur + cur->size == (char*)cur->next); // check that the block size is correctly set
    }
  }
  printf("NULL\n");
}