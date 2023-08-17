#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

// node struct used to hold the data we're storing, and points to the next node in the queue
struct queue_node
{
	void* data; // void pointer to hold our data
	struct queue_node *next; // tail->next head->next
};

typedef struct queue_node* node;

// FIFO Queue object with head and tail pointers for enqueue and dequeue and a size variable
struct queue
{
	struct queue_node* head; // top of our queue
	struct queue_node* tail; // bottom of our queue
	size_t queue_size; // this will be the size of our queue
};

// initializes and returns the queue, sets head/tail to NULL which is their initial state
queue_t queue_create(void)
{
	queue_t the_queue = (queue_t)malloc(sizeof(struct queue)); // initialize our queue object
	// error check if malloc fails, return NULL
	if (the_queue == NULL)
	{
		return NULL;
	}
	// initialize head and tail to null and queue is empty
	the_queue->head = NULL;
	the_queue->tail = NULL;
	the_queue->queue_size = 0;
	return the_queue;
}

// empties the queue and frees all memory + the head
int queue_destroy(queue_t queue)
{
	// error check if queue is NULL or queue not empty
	if (queue == NULL)
	{
		return -1;
	}
	// deallocate memory until queue is empty
	while (queue_length(queue) > 1)
	{
		node save_head = queue->head;
		queue->head = queue->head->next;
		free(save_head);
		queue->queue_size--;
	}
	queue->queue_size--;
	free(queue->head);
	free(queue);
	return 0;
}

// add an item void* data to the queue at the end 
int queue_enqueue(queue_t queue, void *data)
{
	node new_node = (node)malloc(sizeof(struct queue_node)); // make queue_node using typedef struct node
	// Error check for @queue @data is NULL or mem alloc error
	if (new_node == NULL || queue == NULL || data == NULL)
	{
		return -1;
	}
	else
	{
		// set node data to data and next node to NULL
		new_node->data = data;
		new_node->next = NULL;
	}

	if (queue->head == NULL || queue->tail == NULL) // check if it is the first node
	{
		queue->head = new_node;
		queue->tail = new_node;
	}
	else // not the first node
	{
		queue->tail->next = new_node; // tail is prev node, tail.next is set to new node 
		queue->tail = new_node; // tail becomes the new tail node
	}
	queue->queue_size++; // increase size of queue after adding new node
	return 0;
}

// pops the head off the queue and returns it in void** data to where function is called
int queue_dequeue(queue_t queue, void **data)
{
	// error check for @queue @data is NULL or queue is empty
	if (queue_length(queue) <= 0)
	{
		return -1;
	}
	// save data and free node and decrease queue size
	*data = queue->head->data;
	node save_head = queue->head;
	queue->head = queue->head->next; // slide the head to the next oldest node
	free(save_head); // free the popped off node
	queue->queue_size--;
	return 0;
}

// finds the node in queue = void * data and deletes it
int queue_delete(queue_t queue, void *data)
{
	// error check if queue is null or no data found
	if (queue == NULL || data == NULL)
	{
		return -1;
	}
	node nodeToTest = queue->head;
	// first node is node to be deleted
	if (data == queue->head->data)
	{
		if(queue->head->next != NULL)
		{
			queue->head = nodeToTest->next; // set head to head.next 
		}
		else
		{
			queue->head = NULL;
		}
		queue->queue_size--;
		return 0;
	}
	for (int i = 1; i < queue_length(queue); i++) // node is somewhere between head.next - tail
	{
		if (nodeToTest->next != NULL)
		{
			node savedPrev = nodeToTest; // save prev for linkage pres
			nodeToTest = nodeToTest->next; // take next
			if (data == nodeToTest->data)
			{
				savedPrev->next = nodeToTest->next; // skip nodeToTest
				queue->queue_size--;
				return 0;
			}
		}
	}
	
	return 0;
}

// iterates through the queue based on a passed function pointer queue_func_t func
int queue_iterate(queue_t queue, queue_func_t func)
{
	// error check if queue or funct is null
	if (queue == NULL || func == NULL)
	{
		return -1;
	}
	int size = queue_length(queue);
	node nodeToTest = queue->head;
	// iterate through queue till empty
	while (size > 0)
	{
		// while queue is not empty
		// iterate items in the list, start with head and then go to the next items
		// need to save node from func
		func(queue, nodeToTest->data);
		if (size != 0)
		{
			nodeToTest = nodeToTest->next;
		}
		size--;
	}
	return 0;
}

// returns the length of queue
int queue_length(queue_t queue)
{
	// Check if queue is NULL, if not NULL, return size
	if (queue == NULL)
	{
		return -1;
	}
	return queue->queue_size;
}