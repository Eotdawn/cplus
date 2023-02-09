#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int filectx = 0;
int readCnt = 0;
sem_t file;
sem_t rSignal;

void *th_write(void *arg)
{
	while(true)
	{
		sem_wait(&file);
		filectx = (long long)arg;
		// printf("子线程写入%d\n", filectx);
		sem_post(&file);
	}
	return NULL;
}

void *th_read(void *arg)
{
	while(true)
	{
		sem_wait(&rSignal);
		if(readCnt == 0)
			sem_wait(&file);
		readCnt++;
		sem_post(&rSignal);

		printf("子线程读取%d\n", filectx);

		sem_wait(&rSignal);
		readCnt--;
		if(readCnt == 0)
			sem_post(&file);
		sem_post(&rSignal);
		sleep(2);
	}
	return NULL;
}

int main()
{
	sem_init(&file, 0, 1);
	sem_init(&rSignal, 0, 1);

	pthread_t rth[5], wth[3];

	for(int i = 0; i < 5; i++)
		pthread_create(&rth[i], NULL, th_read, NULL);

	for(int i = 0; i < 3; i++)
		pthread_create(&wth[i], NULL, th_write, (void *)(long)i);

	printf("join...\n");
	for(int i = 0; i < 5; i++)
		pthread_join(rth[i], NULL);
	for(int i = 0; i < 3; i++)
		pthread_join(wth[i], NULL);
	printf("join ok\n");

	sem_destroy(&file);
	sem_destroy(&rSignal);
	return 0;
}