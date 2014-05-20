#include "net_def.h"
#define ERROR_PACK_NUM 70

void part_string(char* dest_name,char*src_name,char *input_str,MY_PACKAGE**p_head_in)
{

	printf("字符串打包中.........\n");
	if (!input_str)
	{

		printf("input_str error\n");
		return ;
	}
	if (!dest_name||!src_name)
	{

		printf("host_name error\n");
		return ;
	}
	srand(time(NULL));
	MY_PACKAGE* pack_head=(MY_PACKAGE*)malloc(strlen(input_str));
	*p_head_in=pack_head;
	//每个字符放在一个包里面
	size_t loc=0;
	size_t count_pack=0;
	char temp=input_str[loc];
	while (temp!='\0')
	{

		strcpy(pack_head[count_pack].dest,dest_name);
		strcpy(pack_head[count_pack].src,src_name);
		pack_head[count_pack].data[0]=temp;
		pack_head[count_pack].data_len=strlen(input_str);
		//设置错误包
		if (rand()%100<69)
		{
			pack_head[count_pack].flg=0;
		}else
		{
			pack_head[count_pack].flg=1;
		}
		printf("count=%ld,loc=%ld,flg=%d,char=%c\n",count_pack,loc,pack_head[count_pack].flg,pack_head[count_pack].data[0]);
		++count_pack;
		++loc;
		temp=input_str[loc];
	}
	printf("打包完毕........\n");
}

void * recv_thread_req(void*param)
{

	printf("recv_thread Start!\n");
	//暂时先不用考虑线程同步的问题
	int client_fd=((MY_THREAD_PARAM*)param)->client_fd;
	int flg=((MY_THREAD_PARAM*)param)->flg;//这个我该用来干什么的呢？当然是用来通知发送线程是否回传
	MY_THREAD_PARAM * p_param=(MY_THREAD_PARAM*)param;
	while (1)
	{
		//pthread_mutex_lock(&(p_param->param_lock));
		char temp[200]={};
		if (recv(client_fd,temp,sizeof(temp),0)<=0)
		{

			printf("接受线程的recv函数出错！\n");
			return (void*)0;
		}
		if (((MY_PACKAGE*)temp)->flg==0)
		{
			//2014059  明天再写
			printf("接受到错误包！  正在请求重传！\n");  
		}
		else
		{
			printf("\n------------------------------\n");
			printf("接受到正确的包\n");
			printf("\nRecvMSG:\n");
			printf("\n%c\n",(((MY_PACKAGE*)temp)->data[0]));
			printf("form:%s\n",(((MY_PACKAGE*)temp)->src));
			printf("\n------------------------------\n");
		}
		//pthread_mutex_unlock(&(p_param->param_lock));
	}
}
////////////////////////////
void *send_thread_req(void*param)
{
	printf("send_thread Strat!\n");
	//暂时先不用考虑线程同步的问题
	MY_THREAD_PARAM *p_param=(MY_THREAD_PARAM*)param;
	int client_fd=p_param->client_fd;

	MY_PACKAGE pack_start;
	strcpy(pack_start.src,p_param->src);
	strcpy(pack_start.dest,p_param->dest);
	send(client_fd,&pack_start,sizeof(pack_start),0);//发送握手包
	while(1)
	{

		//pthread_mutex_lock(&(p_param->param_lock));
		printf("Input string to send:\n");
		MY_PACKAGE *p_pack_head;
		char input_data[50]={};
		scanf("%*c");
		scanf("%s",input_data);
		//gets(input_data);
		part_string(p_param->dest,p_param->src,input_data,&p_pack_head);
		unsigned short pack_count=0;
		printf("count_pack:%d\n",p_pack_head[pack_count].data_len);
		while (pack_count<(p_pack_head[pack_count].data_len))
		{

			if (p_pack_head[pack_count].flg==0)
			{

				send(client_fd,&p_pack_head[pack_count],sizeof(MY_PACKAGE),0);
				printf("上次发送为错误包 正在重传！");
				p_pack_head[pack_count].flg=1;
				sleep(2);
				printf("flg=%d\n",p_pack_head[pack_count].flg);
				send(client_fd,&p_pack_head[pack_count],sizeof(MY_PACKAGE),0);
				printf("发送的为正确包！\n");
			}
			else
			{

				send(client_fd,&p_pack_head[pack_count],sizeof(MY_PACKAGE),0);
				printf("发送的为正确包！\n");
			}
			++pack_count;
		}
		free(p_pack_head);//好借好还  再借不难！
		//pthread_mutex_unlock(&(p_param->param_lock));
	}
}


