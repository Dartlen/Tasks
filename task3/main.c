#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdint.h>

#define READ  0
#define WRITE 1

#define handle_error_en(en, msg) do {errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)



struct Memory {
	int  value;
	int  flag;
	int pidB;
	
	};
void startThread(pthread_t *t, void* (*p)())
{
	int s;
	s = pthread_create(t,NULL,p,NULL);
    if (s != 0)
	handle_error_en(s, "pthread_create");

}
int square(int number)
{
	return number*number;
}

void synch_signal (int sig)
{
	printf("Received SIGUSR1!\n");
	
	//kill(0,SIGUSR2);
	
	int            ShmID;
	struct Memory  *ShmPTR;
	key_t ShmKEY;
	ShmKEY = ftok(".key",100);
	ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
	if (ShmID < 0) {
	  printf("*** shmget error (server) ***\n");
	  exit(1);
	}
	ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);	
	
	ShmPTR->pidB=0;
	
	shmdt(ShmPTR);
	shmctl(ShmID ,IPC_RMID,NULL); 
	
	
	
	
}

int processA(int *mypipefd, int *mypipefd2)
{
	printf("start thread A\n");
	printf("hello, input number:\n");
	int writenumber = 0;

	close(mypipefd[WRITE]);
	close(mypipefd2[READ]); 
	int            ShmID;
	struct Memory  *ShmPTR;
	key_t ShmKEY;
	ShmKEY = ftok(".key",100);
	ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
	if (ShmID < 0) {
	  printf("*** shmget error (server) ***\n");
	  exit(1);
	}
	ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);	

	while(1)
    {

			if(ShmPTR->pidB!=0)
			{
				scanf("%d",&writenumber);
				write(mypipefd2[WRITE],&writenumber,sizeof(writenumber));
			}
			else
			{
				break;
			}
		
    }
	close(mypipefd[READ]);
	close(mypipefd2[WRITE]);
	shmdt(ShmPTR);
	shmctl(ShmID ,IPC_RMID,NULL); 
	printf("end thread A\n");
	return 0;
}

int processB(int *mypipefd, int *mypipefd2)
{
	printf("start thread B\n");

	int ch;

	signal(SIGUSR1, synch_signal);
	
	int            ShmID;
	struct Memory  *ShmPTR;
	key_t ShmKEY;
	ShmKEY = ftok(".key",100);
	ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
	if (ShmID < 0) {
	  printf("*** shmget error (server) ***\n");
	  exit(1);
	}
	ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);	
	
	ShmPTR->pidB = getpid(); 
	
	int readnumber = 0;
	close(mypipefd[READ]);      
	close(mypipefd2[WRITE]);  
	
	

	while(ShmPTR->pidB!=0)
    {
 
		read(mypipefd2[READ],&readnumber,sizeof(readnumber));

		ch= square(readnumber);

		ShmPTR->value=ch;
		ShmPTR->flag=1;
	

    }	
    close(mypipefd[WRITE]);    
	close(mypipefd2[READ]);
	
	shmdt(ShmPTR);
	shmctl(ShmID ,IPC_RMID,NULL); 
        
        	
	printf("end thread B\n");
	return 0;
}

void * threadC1()
{
	int shm_id;
	key_t ShmKEY;
	struct Memory * shm_buf;
	ShmKEY = ftok(".key",100);
	shm_id = shmget ( ShmKEY, sizeof(struct Memory ), 0666 );
	
	if ( shm_id == -1 ) {
		printf ( "shmget error \n" );
		shmctl( shm_id, IPC_RMID, NULL );
		return NULL;
	}
	shm_buf = shmat(shm_id,NULL,0);
	if ( shm_buf == ( struct Memory * ) -1 ){
		fprintf ( stderr, " shmat() error\n");
		return NULL;
	}
	while(shm_buf->pidB!=0)
	{
        if(shm_buf->flag==1)
			{
				shm_buf->flag=2;	
			} 
	}
	shmdt(shm_buf);
	shmctl(shm_id, IPC_RMID, NULL);
	return 0;
}
void *threadC2()
{
	int shm_id;
	key_t ShmKEY;
	struct Memory * shm_buf;
	ShmKEY = ftok(".key",100);
	shm_id = shmget ( ShmKEY, sizeof(struct Memory ), 0666 );
	
	if ( shm_id == -1 ) {
		printf ( "shmget error \n" );
		shmctl( shm_id, IPC_RMID, NULL );
		return NULL;
	}
	shm_buf = shmat(shm_id,NULL,0);
	if ( shm_buf == ( struct Memory * ) -1 ){
		fprintf ( stderr, " shmat() error\n");
		return NULL;
	}

	
	while (shm_buf->pidB!=0)
	{
		if(shm_buf->flag==2)
		{
			shm_buf->flag=0;
			printf("#############\nSquare=%d\n#############\n",shm_buf->value);
			
			if(shm_buf->value==100)
				kill(shm_buf->pidB, SIGUSR1);
		}
		else
		{
			printf("I am live\n");
			sleep(2);
		}
		
	}
	shmdt(shm_buf);
	shmctl(shm_id, IPC_RMID, NULL);
	return NULL;
}
int processC()
{	
	printf("start thread С\n");
	
	pthread_t C1,C2;
	void* (*pC1)();
	void* (*pC2)();

	pC1 = threadC1;
	pC2 = threadC2;
	
	startThread(&C1, pC1);
	startThread(&C2, pC2);
	
	pthread_join(C1,NULL);
	pthread_join(C2,NULL);

	printf("end thread C\n");

	return 0;
}
int main()
{
  int mypipefd[2];
  int mypipefd2[2];
  pid_t pid,pid2;
  
  if (pipe(mypipefd) == -1 || pipe(mypipefd2) == -1) {
    fprintf(stderr,"Pipe failed");
    return 1;
  }

  pid = fork();

  if (pid < 0) {
    fprintf(stderr, "Fork failed");
    return 1;
  }
  if (pid > 0) 
  {

        processA(mypipefd, mypipefd2);   
  }
  else 
  { 
	pid2 = fork();

	if (pid2 < 0) 
	{
		fprintf(stderr, "Fork failed");
		return 1;
	}
	if(pid2>0)
	{
		
		processB(mypipefd, mypipefd2);
	}
	else
	{
	 processC();
	}
   }
	return 1;
}
