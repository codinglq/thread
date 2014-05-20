#include "cmy_queue.h"

my_queue::my_queue(size_t queue_size)
{

	if (!queue_size)
	{
		this->p_queue=new int[MAX_QUEUE_LEN];
		this->font=0;
		this->rear=0;
	}else
	{

		this->p_queue=new int[queue_size];
		this->font=0;
		this->rear=0;
	}
}

void my_queue::push_back(int elem)
{

	this->p_queue[this->rear++]=elem;
}

void my_queue::pop_font(int*elem)
{

	*elem=this->p_queue[this->font++];
}
int my_queue::get_font()
{
	return this->p_queue[this->font];
}

int my_queue::get_rear()
{

	return this->p_queue[this->rear-1];
}
