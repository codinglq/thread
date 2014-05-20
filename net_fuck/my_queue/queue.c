#include <stdio.h>
#include <stdlib.h>

/*
   queue
 */
#define MAX_QUEUE_LEN 4

typedef struct{

	int *p_head;
	int font;
	int rear;
}MY_QUEUE;

void init_queue(MY_QUEUE *my_queue)
{

	my_queue->p_head=(int*)malloc(sizeof(int)*MAX_QUEUE_LEN);
	my_queue->font=0;//队头 指向队列中第一个元素
	my_queue->rear=0;//队尾 指向队列中最后一个的下一个
}

void push_back_queue(MY_QUEUE *my_queue,int elem)
{
	my_queue->p_head[my_queue->rear++]=elem;
}

void pop_font_queue(MY_QUEUE* my_queue,int* elem)
{
	*elem=my_queue->p_head[my_queue->font++];
}

int get_head_queue(MY_QUEUE*my_queue)
{

	return my_queue->p_head[my_queue->font];
}

int get_rear_queue(MY_QUEUE*my_queue)
{

	return my_queue->p_head[my_queue->rear-1];
}
int main()
{

	MY_QUEUE queue;
	init_queue(&queue);
	int count=0;
	while(count<MAX_QUEUE_LEN)
	{

		push_back_queue(&queue,count);
		++count;
	}
	count=0;
	while(count<MAX_QUEUE_LEN)
	{

		int tmp;
		pop_font_queue(&queue,&tmp);
		printf("queue=%d\n",tmp);
		printf("font=%d rear=%d\n",queue.font,queue.rear);
		++count;
	}
	return  0;
}
