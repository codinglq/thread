#ifndef CMY_QUEUQ_H
#define CMY_QUEUQ_H
#include <iostream>
#include <string.h>
#include <vector>
#define MAX_QUEUE_LEN 6
class my_queue
{

	public:
		my_queue(size_t queue_size);
	public:
		void push_back(int elem);
		void pop_font(int* elem);
		int get_font();
		int get_rear();
	private:
		int *p_queue;
		int font;
		int rear;
		size_t queue_size;
};
#endif
