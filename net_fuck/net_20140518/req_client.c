#include "net_def.h"

int main()
{

	void * re_thread_1;
	void * re_thread_2;
	char *ip="127.0.0.1";
	short port=MY_PORT;
	int client_fd;
	char host[20]={};
	char dest[20]={};

	printf("input host name :");
	scanf("%s",host);
	printf("input dest:");
	scanf("%s",dest);
	init_client(ip,port,&client_fd);
	pthread_t pid_recv;
	pthread_t pid_send;
	//准备参数
	MY_THREAD_PARAM param;
	strcpy(param.src,host);
	strcpy(param.dest,dest);
	param.client_fd=client_fd;
	if (pthread_mutex_init(&param.param_lock,NULL))
	{

		printf("互斥量创建失败！！！！！\n");
		return 0;
	}
	
	pthread_create(&pid_send,0,send_thread_req,(void*)&param);
	pthread_create(&pid_recv,0,recv_thread_req,(void*)&param);
	
	pthread_join(pid_recv,&re_thread_1);
	pthread_join(pid_recv,&re_thread_2);
	return 0;
}
