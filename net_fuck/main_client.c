#include "net_def.h"
/*
 *客户端
 * */
int main()
{

	char *ip="127.0.0.1";
	short port=MY_PORT;
	int client_fd;
	char name[20]={};
	char dest_table[10][20]={};

	printf("input host name :");
	scanf("%s",name);
    input_dest_queue(dest_table,10);
	init_client(ip,port,&client_fd);
	client_start_queue(&client_fd,name,dest_table);
	return 0;
}
