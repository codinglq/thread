#include "cmy_queue.h"
using namespace std;

int main()
{
	my_queue *queue=new my_queue(4);
	queue->push_back(6);
	int temp;
	queue->pop_font(&temp);
	cout<<"queue pop_font="<<temp<<endl;
	return 0;
}
