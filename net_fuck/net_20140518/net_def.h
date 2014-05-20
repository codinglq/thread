#ifndef NET_DEF_H__
#define NET_DEF_H__
/*
lq 201404227coding
*/
#define MY_PORT 8081
#define MAX_QUEUE 20
#define ADD_TO_FILE 0x01
#define CREATE_NEW_FILE 0x02
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
//数据包格式
typedef struct MY_PACKAGE
{
	char dest[20];//目的地址  当目的地址是all时候 转发给所有主机
	char src[20];//源地址
	unsigned short data_len;//数据长度
	char data[50];//数据
	int flg;
}MY_PACKAGE;
typedef struct CLIENT_INFO_NODE
{

	char name[20];
	int client_fd;
}CLIENT_INFO_NODE;
typedef struct MY_PARAM
{//多线程中用到的参数
	
	int client_fd;
	size_t client_queue_size;
	size_t curr_size;
	CLIENT_INFO_NODE * client_queue;
}MY_PARAM;

typedef struct MY_THREAD_PARAM
{

	char dest[20];
	char src[20];
	int client_fd;
	int flg;
	//MY_THREAD_PARAM *p_pack_head;
	pthread_mutex_t param_lock;//使用时候要初始化一下
}MY_THREAD_PARAM;
//服务器用到的函数
void init_server(const char *ip,const short port,int *server_fd);

void send_package_to_dest(MY_PACKAGE* backage,size_t queue_size,CLIENT_INFO_NODE*client_fd_queue);

void *sever_thread(void*param);//这个就是多线程使用的回调函数

void server_start(CLIENT_INFO_NODE* client_fd_queue,int sever_fd,size_t curr_szie);

//客户端的函数
void init_client(const char *ip,const short port,int * client_fd);
//接受数据的多线程函数
void *recv_thread(void*param);

void client_start(int *sock_fd,char * name,char*dest);

void * recv_thread_req(void *param);

void * send_thread_req(void *param);

void part_string(char* dest_name,char*src_name,char *input_str,MY_PACKAGE**p_head_in);
#endif
