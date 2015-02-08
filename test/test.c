#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){

printf("%s\n", getenv("ABC"));
printf("%s\n", getenv("XYZ"));


/* char procid[5];
//      char cmd1[256], cmd2[256];
        char f[50];

        sprintf(procid, "%d", getpid());

        strcpy(f, "cat /proc/");
        strcat(f, procid);
        strcat(f, "/status\0");


char *line=NULL;
size_t a = 100;
FILE *fp = fopen("/proc/self/status", "r");

 while(getline(&line, &a, fp) != -1){
                if(strstr(line, "Name") || strstr(line, "VmRSS"))
                        printf("%s\n", line);
 }



//system("cat /proc/21267/status | grep Name");

//system("IFS=' '  read -a XYZ  <<<  \"$(ps -p 21267 | tail -1)\" ; echo ${XYZ[${#XYZ[@]} - 1]} ");
//system("IFS=' '  read -a XYZ  <<<  $(ps -p 21267 | tail -1) ; echo ${XYZ[${#XYZ[@]} - 1]} >> test.stat");

/*char *a = strcat("cat /proc/", "hello");

printf("%s\n", a);

//system("echo hello > test.log");

/*
printf("%s\n", argv[0]);

TIMESTAMP

char buff[20];
time_t now = time(NULL);
strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));

printf("%s\n", buff);
*/
return 0;
}
