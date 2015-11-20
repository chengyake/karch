#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ipc.h>
#include <semaphore.h>
int lock_var;
time_t end_time;
sem_t sem1, sem2;

void pthread1(void *arg);
void pthread2(void *arg);
void pthread3(void *arg);

int main(int argc, char *argv[])
{
	pthread_t id1,id2, id3;
	pthread_t mon_th_id;
	int ret;
	end_time = time(NULL)+30;
	ret=sem_init(&sem1,0,1);
	ret=sem_init(&sem2,0,1);
	if(ret!=0)
	{
		perror("sem_init");
	}
	ret=pthread_create(&id1,NULL,(void *)pthread1, NULL);
	if(ret!=0)
		perror("pthread cread1");
	ret=pthread_create(&id2,NULL,(void *)pthread2, NULL);
	if(ret!=0)
		perror("pthread cread2");
	ret=pthread_create(&id3,NULL,(void *)pthread3, NULL);
	if(ret!=0)
		perror("pthread cread3");
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	pthread_join(id3,NULL);

	exit(0);
}

void pthread1(void *arg)
{
	int i;
	while(time(NULL) < end_time) {
		sem_wait(&sem1);
		printf("pthread1 wait ok\n");
		sleep(7);
	}
}

void pthread2(void *arg)
{
	int i;
	while(time(NULL) < end_time) {
		sem_wait(&sem2);
		printf("pthread2 wait ok\n");
	}
}

void pthread3(void *arg)
{
	int nolock=0;
	int ret;
	while(time(NULL) < end_time) {
		sleep(3);
		sem_post(&sem1);
		sem_post(&sem2);
	}
    sem_destroy(&sem1);
    sem_destroy(&sem2);
}

