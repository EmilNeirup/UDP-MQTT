#include<stdio.h>
#include<stdlib.h>

int main() 
{
	char *a ="$(system(nc -z -v localhost 2399))";
	//system("nc -u -l -p 2399");
	
	return 0;
}
