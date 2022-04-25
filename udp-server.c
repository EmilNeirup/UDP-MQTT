#include<stdio.h>
#include<stdlib.h>

int main() 
{
	system("nc -u -l localhost -p 2399");
	return 0;
}
