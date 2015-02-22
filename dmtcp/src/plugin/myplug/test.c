#include<stdio.h>
#include<stdlib.h>
//#include<unistd.h>

int main(){

int a=50;

char *b = getenv("test");

printf("%s\n", b);

/*while(a--){
	sleep(2);
	printf("%d\n",a);
}*/
return 0;
}
