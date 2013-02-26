#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

struct queue* create_queue(){
    //create message queue
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    q->length = 0;
    return q;
}

struct queue_node* create_queue_node(char* m, int m_type){
    //create a queue_node
    struct queue_node* n = (struct queue_node*)malloc(sizeof(struct queue_node));
    n->message = m;
    n->message_length = strlen(m);
    n->message_type = m_type;
    return n;
}

int get_length(struct queue* q){
    //get length of queue
    return q->length;
}

void push(struct queue* q, char* message, int m_type){
    //push a message into the queue
    struct queue_node* n = create_queue_node(message, m_type);
    int i = 0;

    if (get_length(q) == 0){
	q->head = n;
	q->length++;
    }
    else{
	struct queue_node* cur = q->head;
	for (i;i<get_length(q)-1;i++){
	    cur = cur->next;
	}
	cur->next = n;
	q->length++;
    }
}

char* pop(struct queue* q){
    //pop a message of the queue

    if (q->length > 1){
	char* message = q->head->message;
	struct queue_node* new_head = q->head->next;
	free(q->head);
	q->head = new_head;
	q->length--;
	return message;
    }
    else if (q->length == 1){
	char* message = q->head->message;
	free(q->head);
	q->length--;
	return message;
    }
    else {
	printf("Error: Queue length 0\n");
	return "\0";
    }
}

int check_first_node_type(struct queue* q){
    //check the type of the first message in the queue
  if (get_length(q) == 0){
    return 0;
  }
  else{
    int type = q->head->message_type;
    return type;
  }
}

void show(struct queue* q){
    //print out the queue for debug reasons
    int i = 0;
    struct queue_node* n = q->head;
    for (i;i<get_length(q);i++){
	printf("Message: %s, Length: %d\n", n->message, n->message_length);
	n = n->next;
    }
}

void destroy_queue(struct queue* q){
    //free malloced nodes and free queue 
    struct queue_node* cur = q->head;
    int i = 0;
    for (i;i<get_length(q);i++){
	struct queue_node* next = cur->next;
	free(cur);
	cur = next;
    }
    free(q);
}
