#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_ID 1

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

void *foo(void*param)
{

	size_t count=100;
	while (count>0)
	{
		pthread_mutex_lock(&lock);
		--count;
		printf("多线程0中......\n");
		pthread_mutex_unlock(&lock);
	}
	return (void*)0;
}

void* foo1(void* param)
{

	size_t count=100;
	while (count>0)
	{
		pthread_mutex_lock(&lock);
		--count;
		printf("多线程1......\n");
		pthread_mutex_unlock(&lock);
	}
	return (void*)(0);
}
int main()
{

	void * re;
	pthread_t pid1;
	pthread_t pid2;
	int res=pthread_create(&pid1,NULL,foo,NULL);
	int res1=pthread_create(&pid2,NULL,foo1,NULL);
	if (res&&res1)
	{

		perror("ERROR:");
		printf("多线程创建失败！\n");
		exit(-1);
	}
	//pthread_join(pid1,&re);
	//pthread_join(pid2,&re);
	printf("主线程中....\n");
	pthread_mutex_destroy(&lock);
	while (1)
	{

	}
	return 0;
}
