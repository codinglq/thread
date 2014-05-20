#include "net_def.h"

int main()
{

	char *str="a";
	MY_PACKAGE *head;	
	part_string("a","b",str,head);
	free(head);
	return 0;
}
