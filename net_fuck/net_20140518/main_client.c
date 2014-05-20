#include "net_def.h"

int main()
{

	char *ip="127.0.0.1";
	short port=MY_PORT;
	int client_fd;
	char name[20]={};
	char dest[20]={};

	printf("input host name :");
	scanf("%s",name);
	printf("input dest:");
	scanf("%s",dest);
	init_client(ip,port,&client_fd);
	client_start(&client_fd,name,dest);
	return 0;
}
