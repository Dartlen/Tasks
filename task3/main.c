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

#define handle_error_en(en, msg) do {errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


enum { STATE_A, STATE_B, STATE_C } state = STATE_A;
pthread_cond_t      condA  = PTHREAD_COND_INITIALIZER;
pthread_cond_t      condB  = PTHREAD_COND_INITIALIZER;
pthread_cond_t      condC  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex  = PTHREAD_MUTEX_INITIALIZER;

int flag=1;
int fd[2];


struct Memory {
	int  value;
	int  flag;
	};

int square(int number)
{
	return number*number;
}
void *threadA()
{
	printf("start thread A\n");
	printf("hello, input number:\n");
	int number;
		
	while(1)
    {
		
        pthread_mutex_lock(&mutex);
        while (state != STATE_A)
            pthread_cond_wait(&condA, &mutex);
        pthread_mutex_unlock(&mutex);

		if (flag==0)
			break;
		scanf("%d", &number);

		int result;
		result = write (fd[1], &number,sizeof(number));
		if (result == -1){
			perror ("write");
			exit (1);
		}

        state = STATE_B;
        pthread_cond_signal(&condB);
        pthread_mutex_unlock(&mutex);
    }
	    pthread_mutex_lock(&mutex);
        state = STATE_B;
        pthread_cond_signal(&condB);
        pthread_mutex_unlock(&mutex);
	printf("end thread A\n");
	return NULL;
}
void synch_signal (int sig)
{
	/* Изменение значения флага ведущее к завершению A,B,C*/ 
	flag=0;
}
void *threadB()
{
	
	printf("start thread B\n");

	int tmpsquare;
	struct sigaction usr_action;
	sigset_t block_mask;

	sigfillset (&block_mask);
	usr_action.sa_handler = synch_signal;
	usr_action.sa_mask = block_mask;
	usr_action.sa_flags = 0;
	sigaction (SIGUSR1, &usr_action, NULL);


	int            ShmID;
	struct Memory  *ShmPTR;
	key_t ShmKEY;
    /* Создание разделенной памяти в потоке B */ 
	ShmKEY = ftok("*",100);
	ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
	if (ShmID < 0) {
	  printf("*** shmget error (server) ***\n");
	  exit(1);
	}
	ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
	
		
	while(1)
    {
        pthread_mutex_lock(&mutex);
        
        while (state != STATE_B)
        {
            pthread_cond_wait(&condB, &mutex);
		}
        pthread_mutex_unlock(&mutex);
		
		if (flag==0)
			break;        

		read (fd[0],&tmpsquare,sizeof(tmpsquare));
		
		tmpsquare = square(tmpsquare);

		ShmPTR->value=tmpsquare;
		ShmPTR->flag=1;

        pthread_mutex_lock(&mutex);
        state = STATE_C;
        pthread_cond_signal(&condC);
        pthread_mutex_unlock(&mutex);        
    }	

	pthread_mutex_lock(&mutex);
	state = STATE_C;
	pthread_cond_signal(&condC);
	pthread_mutex_unlock(&mutex);
	
	shmctl(ShmID ,IPC_RMID,NULL);  
        
	printf("end thread B\n");
	return NULL;
}


void *threadC()
{	
	printf("start thread С\n");
	
	int shm_id;
	key_t ShmKEY;
	struct Memory * shm_buf;
	/* Создание разделенной памяти в потоке C */ 
	ShmKEY = ftok("*",100);
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

	int fork_rv;
	fork_rv = fork();
	
	if(fork_rv==-1)
	{
		perror("fork");
    }
	else if(fork_rv == 0)
	{

		printf("start thread С child\n");
		
		while (1) {

			if(shm_buf->flag==1)
			{
				shm_buf->flag=0;
				printf("#############\nSquare=%d\n#############\n",shm_buf->value);
			}
			else
			{
				printf("I am live\n");
		    }
			sleep(1);
		}
		
	}
	else
	{
		printf("start thread C parrent\n");

		while(1)
		{
			pthread_mutex_lock(&mutex);
			while (state != STATE_C)
			{
				pthread_cond_wait(&condC, &mutex);
			}
			pthread_mutex_unlock(&mutex);
			if(flag==0)
			{
				kill(fork_rv,SIGKILL);
				break;
			}
				if (shm_buf->value==100)
				{
					raise(SIGUSR1);
					
				}
			pthread_mutex_lock(&mutex);
			state = STATE_A;
			pthread_cond_signal(&condA);
			pthread_mutex_unlock(&mutex);
			
		}
		pthread_mutex_lock(&mutex);
		state = STATE_A;
		pthread_cond_signal(&condA);
		pthread_mutex_unlock(&mutex);
    }
	shmdt(shm_buf);
	shmctl(shm_id, IPC_RMID, NULL);
	printf("end thread C\n");

	return NULL;
}

void startThread(pthread_t *t, void* (*p)())
{
	int s;
	s = pthread_create(t,NULL,p,NULL);
    if (s != 0)
		handle_error_en(s, "pthread_create");

}

void createPipe()
{
	int result;
	result = pipe (fd);
	if (result < 0){
		perror("pipe ");
		exit(1);
	}
}

int main()
{
	
	pthread_t A,B,C;
	
	void* (*pA)();
	void* (*pB)();
	void* (*pC)();
	
	pA = threadA;
	pB = threadB;
	pC = threadC;
	
	createPipe();
	
	startThread(&A, pA);
	startThread(&B, pB);
	startThread(&C, pC);

	pthread_join(A,NULL);
	pthread_join(B,NULL);
	pthread_join(C,NULL);

	return 0;
}
