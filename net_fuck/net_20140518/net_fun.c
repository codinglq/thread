#include "net_def.h"


void init_server(const char *ip,const short port,int *server_fd)
{

    printf("启动服务器！\n");
    *server_fd=socket(PF_INET,SOCK_STREAM,0);	
    if (-1==*server_fd)
    {

        perror("sock create error");
        printf("start server error!\n");
        exit(-1);
    }
    printf("socket sucess!\n");
    struct sockaddr_in addr;
    addr.sin_family=PF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    if (-1==bind(*server_fd,(struct sockaddr*)(&addr),sizeof(addr)))
    {

        perror("bind error");
        printf("start server error!\n");
        exit(-1);
    }
    printf("bind sucess!\n");
    if (listen(*server_fd,10)==-1)
    {

        perror("listen error");
        printf("start server error!\n");
        exit(-1);
    }
    printf("listen sucess Server Sucess!\n");
}

void send_package_to_dest(MY_PACKAGE * backage,size_t queue_size,CLIENT_INFO_NODE*client_fd_queue)
{

    printf("what:%s\n",backage->data);
    //发送给对应的主机
    size_t count=0;
    while (count<queue_size)
    {
        printf("client_fd_queue[%ld] name:%s\n",count,client_fd_queue[count].name);
        if (client_fd_queue[count].client_fd!=0&&(strcmp(backage->dest,client_fd_queue[count].name)==0))
        {

            printf("转发包从%s到%s\n",backage->src,client_fd_queue[count].name);
            send(client_fd_queue[count].client_fd,backage,sizeof(MY_PACKAGE),0);
            printf("------------------\n");
        }
        ++count;
    }
}


void *sever_thread(void*param)//这个就是多线程使用的回调函数
{

    MY_PARAM *the_param=(MY_PARAM*)param;
    size_t client_queue_size=the_param->client_queue_size;
    int client_fd=the_param->client_fd;
    size_t curr_size=the_param->curr_size;
    CLIENT_INFO_NODE * p_client_queue=the_param->client_queue;
    printf("create thread for sockfd :%d",client_fd);
    while (1)
    {

        char package_temp[200]={};
        if (recv(client_fd,package_temp,sizeof(package_temp),0)<=0)
        {

            printf("client_fd:%d下线了\n",client_fd);
            size_t count=0;
            while (count<client_queue_size)
            {

                if (p_client_queue[count].client_fd==client_fd)
                {

                    p_client_queue[count].client_fd=0;
                    strcpy(p_client_queue[count].name,"");
                    count=0;
                }
                return (void*)0;
            }

        }
        printf("server_thread src:%s\n",((MY_PACKAGE*)package_temp)->src);
        strcpy(p_client_queue[curr_size-1].name,((MY_PACKAGE*)package_temp)->src);
        printf("cuur size=%ld\n",curr_size);
        send_package_to_dest((MY_PACKAGE*)package_temp,client_queue_size,p_client_queue);
    }
}

void server_start(CLIENT_INFO_NODE *client_fd_queue,int server_fd,size_t curr_size)
{

    printf("server start!");
    curr_size=0;
    while (1)
    {

        struct sockaddr_in form_addr;
        socklen_t len=sizeof(form_addr);
        int fd=accept(server_fd,(struct sockaddr*)(&form_addr),&len);
        client_fd_queue[curr_size++].client_fd=fd;
        printf("主机：%d链接到服务器\n",fd);
        // 准备参数
        MY_PARAM *param=(MY_PARAM*)malloc(sizeof(MY_PARAM));
        param->client_fd=fd;
        param->client_queue_size=MAX_QUEUE;
        param->curr_size=curr_size;
        printf("severstart_curr_size=%ld\n",curr_size);
        param->client_queue=client_fd_queue;
        pthread_t pid;
        pthread_create(&pid,0,sever_thread,param);
    }
}


//客户端的函数


void init_client(const char *ip,const short port,int * client_fd)
{

    printf("start client\n");

    *client_fd=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=PF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    if (*client_fd==-1)
    {

        perror("socket errer");
        exit(-1);
    }
    if (connect(*client_fd,(struct sockaddr*)(&addr),sizeof(addr))==-1)
    {

        perror("connect error!");
        exit(-1);
    }
    printf("client start sucsee!\n");

}


void *recv_thread(void*param)
{

    int client_fd=*(int*)param;
    while (1)
    {

        char package_temp[200]={};
        if (recv(client_fd,package_temp,sizeof(package_temp),0)<=0)
        {
            printf("接收失败。。。。");
            return (void*)0;
        }
        printf("\n----------------------------------------\n");
        printf("\nMSG:\n");
        printf("\nfrom:%s\n",((MY_PACKAGE*)package_temp)->src);
        printf("to:%s\n",((MY_PACKAGE*)package_temp)->dest);
        printf("data:%s\n",((MY_PACKAGE*)package_temp)->data);
        printf("\n----------------------------------------\n");
        printf("input data to send:");
    }
}

void client_start(int *sock_fd,char * name,char *dest)
{

    printf("OK client!\n");
    pthread_t pid;
    pthread_create(&pid,0,recv_thread,(void*)sock_fd);	
    //在这里先发一个报名的包
    MY_PACKAGE package_temp;
    //strcpy(package_temp.dest,dest);
    strcpy(package_temp.src,name);
    strcpy(package_temp.data,"Hello ,world");
    //printf("input dtat to send:");
    //scanf("%s",package_temp.data);
    package_temp.data_len=strlen(package_temp.data);
    //ooooooooooo
    send(*sock_fd,&package_temp,sizeof(package_temp),0);
    while (1)
    {

        MY_PACKAGE package_temp;
        printf("input data to send:");
        gets(package_temp.data);
        strcpy(package_temp.dest,dest);
        strcpy(package_temp.src,name);
        package_temp.data_len=strlen(package_temp.data);
        send(*sock_fd,&package_temp,sizeof(package_temp),0);
    }
}


