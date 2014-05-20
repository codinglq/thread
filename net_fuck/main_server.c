#include "net_def.h"
/*
服务器
*/
int main()
{

	int server_fd;
	char *ip="127.0.0.1";
	short port=8081;
	CLIENT_INFO_NODE client_queue[20];
    init_client_queue(client_queue,20);
	init_server(ip,port,&server_fd);
	size_t curr_size=20;
	server_start(client_queue,server_fd,curr_size);
	//这里我暂时不释放线程
	return 0;
}
