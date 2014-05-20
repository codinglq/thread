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
    int on=1;
    //设置允许地址重用
    setsockopt(*server_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
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


void send_package_to_dest_queue(MY_PACKAGE* backage,size_t queue_size,CLIENT_INFO_NODE*client_fd_queue)
{
    /*发包给包中站表的函数*/
    //包中的二维目的站表的最后一行应该为NULL这样才能方便判断结束
    printf("包中数据：%s\n",backage->data);
    size_t count=0;
    while (count<queue_size)
    {
        printf("客户端表：%s\n",client_fd_queue[count].name);
        size_t count_back_dest_queue=0;
        while (count_back_dest_queue<10)
        {
            // printf("-------------------test\n");
            //       printf("client_fd=%d,back_dest=%s,client_fd_queue_name=%s\n",client_fd_queue[count].client_fd,backage->dest_queue[count_back_dest_queue],client_fd_queue[count].name);
            if (client_fd_queue[count].client_fd!=0&&(strcmp(backage->dest_queue[count_back_dest_queue],client_fd_queue[count].name)==0))
            {

                printf("转发包从%s到%s\n",backage->src,client_fd_queue[count].name);
                send(client_fd_queue[count].client_fd,backage,sizeof(MY_PACKAGE),0);
                printf("转发结束！\n");
            }
            ++count_back_dest_queue;
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

        MY_PACKAGE package_temp;
        if (recv(client_fd,&package_temp,sizeof(package_temp),0)<=0)
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
                //运行到这里  这线程就退出了
            }
        }
        printf("server_thread src:%s\n",(package_temp).src);
        strcpy(p_client_queue[curr_size-1].name,(package_temp).src);
        printf("cuur size=%ld\n",curr_size);
        send_package_to_dest(&package_temp,client_queue_size,p_client_queue);
    }
}

void init_client_queue(CLIENT_INFO_NODE * head,size_t count)
{
    /*初始化客户端表*/
    size_t this_count=0;
    printf("开始初始化客户端表\n");
    while (this_count<count)
    {
        strcpy(head[this_count].name,"");
        head[this_count].client_fd=0;
        ++this_count;
    }
    printf("客户端表初始化完毕！\n");
}

void *sever_thread_dest_queue(void*param)
{
    MY_PARAM *the_param=(MY_PARAM*)param;
    size_t client_queue_size=the_param->client_queue_size;
    int client_fd=the_param->client_fd;
    size_t curr_size=the_param->curr_size;
    CLIENT_INFO_NODE * p_client_queue=the_param->client_queue;

    int flg=0;
    printf("create thread for sockfd :%d",client_fd);
    while (1)
    {

        MY_PACKAGE package_temp;
        if (recv(client_fd,&package_temp,sizeof(MY_PACKAGE),/*0*/MSG_WAITALL)<=0)
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
                //运行到这里  这线程就退出了
            }
        }
        printf("server_thread name:%s\n",(package_temp).src);
        if (!flg)
        {
            //这个建立客户端表的过程只执行一次
            strcpy(p_client_queue[curr_size-1].name,(package_temp).src);
            flg=1;
        }
        printf("cuur size=%ld\n",curr_size);
        send_package_to_dest_queue(&package_temp,client_queue_size,p_client_queue);
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
        pthread_create(&pid,0,sever_thread_dest_queue,param);
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

    printf("接收线程中.........\n");
    MY_ROUT_PARAM *rout_param=(MY_ROUT_PARAM*)param;
    int client_fd=*(rout_param->client_fd);
    MY_ROUT_TABLE * the_rout_table=rout_param->the_rout_table;
    while (1)
    {

        MY_PACKAGE package_temp;//这个是接收缓存包 里面的路由表是其他路由器传过来的
        if (recv(client_fd,&package_temp,sizeof(MY_PACKAGE),MSG_WAITALL)<=0)
        {
            printf("接收失败。。。。");
            return (void*)0;
        }
        /*printf("\n----------------------------------------\n");
        printf("\nMSG:\n");
        printf("\nfrom:%s\n",(package_temp).src);
        printf("data:%s\n",(package_temp).data);
        printf("\n----------------------------------------\n");
        printf("input data to send:");*/

        start_my_rip(&(package_temp.the_rout_table),the_rout_table);
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


void input_dest_queue(char buf[10][20],size_t dest_count)
{
    size_t count=0;
    char dest_temp[20];
    scanf("%*c");
    printf("输入目的地名字，以NULL结束：\n");
    scanf("%s",dest_temp);
    while (strcmp(dest_temp,"NULL")&&count<dest_count)
    {

        strcpy(buf[count],dest_temp);
        ++count;
        scanf("%*c");
        memset(dest_temp,'\0',sizeof(dest_temp));
        scanf("%s",dest_temp);
    }
    printf("输入结束。\n");
}


void copy_dest_queue(char buf[10][20],char dest_queue[10][20],size_t dest_count)
{
    /*不做缓冲区溢出检查*/
    size_t count=0;
    while (count<dest_count&&strcmp(buf[count],"NULL"))
    {

        strcpy(dest_queue[count],buf[count]);
        ++count;
    }
}

void client_start_queue(int *sock_fd,char * name,char dest_table[10][20])
{

    /*
     *路由器的包和表在这里准备！  就用一个 爱咋咋地！
     * */
    MY_PACKAGE package_temp;
    MY_ROUT_TABLE/*用这个指针初始化路由表*/ * the_rout_table=&(package_temp.the_rout_table);

    int host_ID;
    printf("请输入当前主机号：\n");

    scanf("%*c");
    scanf("%d",&host_ID);
    //初始化路由表
    init_rout_table(the_rout_table,host_ID);
    //为路由表赋值
    //这个地方要手动填写 或者整个配置文件 自己看着办吧
    the_rout_table->rout_table[0][0]=2;
    the_rout_table->rout_table[0][1]=3;
    the_rout_table->rout_table[0][2]=4;

    the_rout_table->rout_table[1][0]=3;
    the_rout_table->rout_table[1][1]=4;
    the_rout_table->rout_table[1][2]=5;

    the_rout_table->curr_size=2;
    //.................................
    //初始化路由表互斥量
    pthread_mutex_init(&(the_rout_table->rout_table_lock),NULL);
    /*
     *由于接受线程要接受其他路由器发送的路由表
     所以接收线程还要负责合并两个路由表所以要把路由表传进去给接收线程
     * */
    //准备参数
    MY_ROUT_PARAM rout_param;
    rout_param.client_fd=sock_fd;
    rout_param.the_rout_table=the_rout_table;


    printf("OK client!\n");
    pthread_t pid;
    pthread_create(&pid,0,recv_thread,
            /*(void*)sock_fd*/(void*)&rout_param);	

    //在这里先发一个报名的包
    strcpy(package_temp.src,name);
    printf("host name:%s\n",package_temp.src);
    strcpy(package_temp.data,"Hello ,world");
    package_temp.data_len=strlen(package_temp.data);
    send(*sock_fd,&package_temp,sizeof(MY_PACKAGE),0);
    while (1)
    {

        //memset(&package_temp,'\0',sizeof(MY_PACKAGE));
        //这个地方应该不是输入数据进行发送了
        //，而是间隔一段时间发送一个路由表
        //printf("input data to send:");
        //gets(package_temp.data);
        //strcpy(package_temp.dest,dest);
        display_rout_table(the_rout_table);
        sleep(10);
        printf("发送路由表\n");
        copy_dest_queue(dest_table,(package_temp.dest_queue),10);
        strcpy(package_temp.src,name);
        package_temp.data_len=strlen(package_temp.data);
        strcpy(package_temp.data,"");
        //printf("dest_queue=%s\n",package_temp.dest_queue[0]);
        send(*sock_fd,&package_temp,sizeof(package_temp),0);
        printf("发送完毕\n");
    }
}


//初始化路由表
void init_rout_table(MY_ROUT_TABLE * the_rout_table,int host_ID)
{
    /*将路由表中的数据都初始化成-1*/
    printf("开始初始化路由表！\n");
    size_t size_table_col=0;
    size_t size_table_row=0;
    while (size_table_col<MAX_MY_ROUT_TABLE_SIZE)
    {
        while (size_table_row<3)
        {

            the_rout_table->rout_table[size_table_col][size_table_row]
            =-1;
            ++size_table_row;
        }
        ++size_table_col;
        size_table_row=0;
    }
    the_rout_table->host_ID=host_ID;//表示发包的主机number
    the_rout_table->curr_size=0;
    printf("初始化路由表结束！\n");
}

void start_my_rip(MY_ROUT_TABLE* input_rout_table,MY_ROUT_TABLE *the_rout_table)
{

    if(!pthread_mutex_lock(&(the_rout_table->rout_table_lock)))
    {
        printf("路由表加锁成功！\n");
    }else{
        printf("路由表加锁失败！\n");
    }
    /*rip协议在这里实现
    *input_rout_table是其他路由器传过来的路由表
    the_rout_table是当前路由器的路由表
    * */
    size_t size_table_col=0;//控制行的指针
    printf("rip协议算法第一步 更改网络距离和主机号\n");
    while (size_table_col<input_rout_table->curr_size)
    {

        input_rout_table->rout_table[size_table_col][1]+=1;
        input_rout_table->rout_table[size_table_col][2]=
        input_rout_table->host_ID;
        ++size_table_col;
    }
    printf("rip第一步结束！\n");

    printf("本机路由表：\n");
    display_rout_table(the_rout_table);
    printf("修改后的接收到得路由表：\n");
    display_rout_table(input_rout_table);
    /*rip 第二部分*/
    size_table_col=0;
    size_t size_the_table_size=0;
    int exist_flg[MAX_MY_ROUT_TABLE_SIZE]={};

    int count_flg=0;
    while (count_flg<MAX_MY_ROUT_TABLE_SIZE)
    {
        exist_flg[count_flg]=0;
        ++count_flg;
    }
    //为了后面的两表合并做准备
    //标记着输入路由表中某一个项目是否存在在本主机表中
    
    printf("rip第二步\n");
    while (size_the_table_size<the_rout_table->curr_size)
    {
        while (size_table_col<input_rout_table->curr_size)
        {
            
            if (the_rout_table->rout_table[size_the_table_size][0]
                ==input_rout_table->rout_table[size_table_col][0])
            {//网络号相等

             exist_flg[size_table_col]=1;//表示在本主机的路由表中已经存在这个网络

             if (the_rout_table->rout_table[size_the_table_size][2]==
                 input_rout_table->host_ID)
             {//下一跳地址相等

              the_rout_table->rout_table[size_the_table_size][1]=
              input_rout_table->rout_table[size_table_col][1];
              //更新距离
             }else if (the_rout_table->rout_table[size_the_table_size][1]>
                       input_rout_table->rout_table[size_table_col][1])
             {
                 //更新成新的下一跳主机
                 the_rout_table->rout_table[size_the_table_size][1]=
                 input_rout_table->rout_table[size_table_col][1];
                
                 the_rout_table->rout_table[size_the_table_size][2]=
                 input_rout_table->rout_table[size_table_col][2];

                 the_rout_table->rout_table[size_the_table_size][2]=
                 input_rout_table->host_ID;
             }
            }  
             //下面的语句可能会有问题
            /*
            }else if (size_the_table_size==(the_rout_table->curr_size))
            {
                //新的网络号  要添加新的条目
                ++the_rout_table->curr_size;
                the_rout_table->rout_table[the_rout_table->curr_size][0]=
                input_rout_table->rout_table[size_table_col][0];

                the_rout_table->rout_table[the_rout_table->curr_size][1]=
                input_rout_table->rout_table[size_table_col][1];

                the_rout_table->rout_table[the_rout_table->curr_size][2]=
                input_rout_table->rout_table[size_table_col][2];
            }
            */
            ++size_table_col;
        }
        size_table_col=0;
        ++size_the_table_size;
    }
    printf("rip第二步结束\n");
    //两个路由表要合并啦～～～  ：）
    size_table_col=0;
    printf("rip第三部\n");
    while (size_table_col<input_rout_table->curr_size)
    {
        if (exist_flg[size_table_col]!=1)
        {
            //说明这个项目的网络在本机路由器中不存在
            //向本机添加这个网络号
            the_rout_table->rout_table[the_rout_table->curr_size][0]=
            input_rout_table->rout_table[size_table_col][0];
            
            the_rout_table->rout_table[the_rout_table->curr_size][1]=
            input_rout_table->rout_table[size_table_col][1];
            
            the_rout_table->rout_table[the_rout_table->curr_size][2]=
            input_rout_table->rout_table[size_table_col][2];

            ++the_rout_table->curr_size;
        }
        ++size_table_col;
    }
    printf("rip第三部结束 rip结束\n");

    pthread_mutex_unlock(&(the_rout_table->rout_table_lock));
    printf("路由表解锁。\n");
}


void display_rout_table(MY_ROUT_TABLE * rout_table)
{
    /*输出路由表*/
    printf("输出主机%d的路由表项目\n",rout_table->host_ID);
    printf("网络号|距离|下一跳\n");
    size_t size_rout_table=0;
    while (size_rout_table<(rout_table->curr_size))
    {

        printf("%4d%5d%6d\n",
               rout_table->rout_table[size_rout_table][0]
               ,rout_table->rout_table[size_rout_table][1]
              ,rout_table->rout_table[size_rout_table][2]);
        ++size_rout_table;
    }
    printf("路由表输出完毕\n");
}
/*
 *上面的代码时间戳20140519
 * */
void create_rout_table(MY_ROUT_TABLE * rout_table)
{
    printf("手动生成路由表 :)\n");

    printf("输入路由表的项目数：\n");
    scanf("%ld",&(rout_table->curr_size));
   size_t count_col=0;
   while (count_col<MAX_MY_ROUT_TABLE_SIZE)
   {
       if (count_col==MAX_MY_ROUT_TABLE_SIZE)
       {

            printf("输入项目个数超限！ 停止输入！\n");
            break;
       }else
       {
            scanf("%*c");
            printf("请输入网络号，距离，下一跳地址 用空格分开：结束输入-1 -1 -1\n");
            scanf("%d %d %d",&(rout_table->rout_table[count_col][0])
                    ,&(rout_table->rout_table[count_col][1])
                    ,&(rout_table->rout_table[count_col][2])
                    );
            if (rout_table->rout_table[count_col][0]==-1)
            {
                
                printf("输入完成\n");
                break;
            }
            ++count_col;
       }
   }
   printf("Has create rout_table\n");
}
