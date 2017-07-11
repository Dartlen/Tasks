#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#define PORT 5000

int fd[2];

int execute(char *argslist,int sock)
{
    int pid;
    int r;
    int flag=1;
    int pipe_fd_1[2];
    int pipe_fd_2[2];
    
    /*void ALARMhandler(int sig)
    {
        //signal(SIGALRM, SIG_IGN);          
        //printf("Hello\n");
        flag=0;
        signal(SIGALRM, ALARMhandler);     
    }*/
    pipe( pipe_fd_1);
    pipe( pipe_fd_2);
   
    char buf[1024] = {0};
    char *a = malloc(256);
    strcpy(a, "The end message\0");

    pid = fork();

    switch(pid)
    {
        case -1:
            perror("fork failed");
            exit(1);
        case 0:
            
            close(pipe_fd_1[1]);
            close(pipe_fd_2[0]);

            
            dup2( pipe_fd_1[0], STDIN_FILENO);
            dup2( pipe_fd_2[1], STDOUT_FILENO);
            
            
            execlp("/bin/bash", "bash", NULL);
            perror("execvp failed");
            exit(1);
        default:
            close(pipe_fd_1[0]);
            close(pipe_fd_2[1]);
            
            fcntl(pipe_fd_2[0], F_SETFL, fcntl(pipe_fd_2[0], F_GETFL, NULL ) | O_NONBLOCK );
            
            strcpy(buf,argslist);
            r=strlen(buf);
            //signal(SIGALRM, ALARMhandler);
            //alarm(3);
            while(flag)
            {
                if( r > 1 )
                {
                    buf[r] = '\0';
                    write(pipe_fd_1[1], &buf, r);
                }

                usleep(100000);
                
                while( ( r = read( pipe_fd_2[0], &buf, 1024 ) ) > 0 )
                {
                    buf[r-1] = '\0';
                    write(sock, buf, strlen(buf) + 1);
                    memset(&buf[0], 0, sizeof(buf));
                }
                sleep(10);
                flag=0;
            }
            char *message = malloc(256);;
            message="The end message\n";
            write(sock,message,strlen(message)+1);
    }
    return 0;
}

int make_server_socket()
{
    int listenfd = 0;// connfd = 0;
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    return listenfd;
}
int processmode(int listenfd)
{
    int new,pid,counter=0;
    for(;;)
    {
        new = accept(listenfd, (struct sockaddr *)NULL, NULL);

        if ((pid = fork()) == -1)
        {
            close(new);
            continue;
        }
        else if(pid > 0)
        {
            close(new);

            continue;
        }
        else if(pid == 0)
        {
            counter++;
            printf("here2\n");
            printf("server is run process\n");

            while(1)
            {
                char recvBuff[1024];
                if (new == -1)
                    break;
                read(new, recvBuff, sizeof(recvBuff)-1);
                execute(recvBuff,new);
                memset(&recvBuff[0], 0, sizeof(recvBuff));
                if (fputs(recvBuff, stdout) == EOF)
                {
                    printf("\n Error : Fputs error\n");
                }
            }
            close(new);
            break;
        }
    }
    printf("server is run process\n");
    return 0;
}
void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;

    while(1)
    {
        char recvBuff[1024];
        if (sock == -1)
            break;
        read(sock, recvBuff, sizeof(recvBuff)-1);
        execute(recvBuff,sock);
        memset(&recvBuff[0], 0, sizeof(recvBuff));
        if (fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }
    //free(socket_desc);
    return  NULL;
}
int pthread_mode(int listenfd)
{
    printf("server is run pthread\n");

    int client_sock, *new_sock;

    while( (client_sock = accept(listenfd, (struct sockaddr *)NULL, NULL)))
    {
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}
int main(int argc, char *argv[])
{
    //argv[1]="server-process"; //server - pthreads  server-process
	
    pipe(fd);
	
    int sock = make_server_socket();

    if(sock == -1)
        exit(1);
    
    if (strcmp(argv[1],"server-process")==0)
    {
        processmode(sock);
    }else
    {
        pthread_mode(sock);
    }
}


