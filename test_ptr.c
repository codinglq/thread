/*************************************************************************
	> File Name: test_ptr.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2014年05月21日 星期三 11时15分50秒
 ************************************************************************/

#include<stdio.h>

typedef struct NODE
{

    int data;
    struct NODE * next;
}LINK;
int main()
{

    LINK * ptr;
    ptr->next=NULL;
    return 0;
}
