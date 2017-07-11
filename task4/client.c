#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int sockfd = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 
	//argv[1]="127.0.0.1";
	//argv[2]="ls -a -b -c"; //server - pthreads

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    char sb[1025];

    char *command = malloc (1025);
	
	char b[1025];
    while(1){
		
		if (command == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("\ninput command:");

		fgets (command, 1025, stdin);

		if ((strlen(command)>0) && (command[strlen (command) - 1] == '\n'))
			command[strlen (command) - 1] = '\0';

		snprintf(sb, sizeof(sb), "%s\n", command);

		write(sockfd, sb, strlen(sb));
		usleep(100000);
		while(1)
		{
			read(sockfd,b,sizeof(b)-1);
				
			if(strcmp(b,"The end message\n"))
				printf("%s",b);
			else
				break;
			memset(&b[0], 0, sizeof(b));
		}
	}
	free (command);

    return 0;
}
