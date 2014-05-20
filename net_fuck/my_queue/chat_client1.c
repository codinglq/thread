#include "net_def.h"

int sockfd;//客户端socket
char* IP="127.0.0.1";//服务器的IP
short PORT=10222;//服务器服务端口
typedef struct sockaddr SA;

char name[30];

void init(){
	printf("客户端开始启动\n");
	sockfd=socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(PORT);		
	addr.sin_addr.s_addr=inet_addr(IP);
	if(connect(sockfd,(SA*)&addr, sizeof(addr))==-1){
		perror("无法连接到服务器");
		printf("客户端启动失败\n");
		exit(-1);
	} 
	printf("客户端启动成功\n");
}
void start(){
	pthread_t id;
	void* recv_thread(void*);
	//这里只是声明一下
	pthread_create(&id, 0, recv_thread, 0);
	while(1){
		char buf[100] = {};
		scanf("%s", buf);
		char msg[100] = {};
		sprintf(msg, "%s: %s", name, buf);
		send(sockfd, msg, strlen(msg), 0);
	}
}

void* recv_thread(void* p){
	while(1){
		char buf[100] = {};
		if(recv(sockfd, buf, sizeof(buf), 0)<=0){
			return ;
		}
		printf("%s\n", buf);
	}
}


int main()
{
	init();
	printf("请输入您的名字:");
	scanf("%s", name);
	start();
}

