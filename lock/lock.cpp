#include <stdio.h>
#include <pthread.h>

// 创建互斥
// pthread_mutex_t mutex;

// 创建自旋锁 自旋锁不会阻塞等待
pthread_spinlock_t mutex;

int res = 0;
void *threadMain(void * arg)
{
	for(int i = 1; i <= 10000; i++)
	{
		// 加锁
		// pthread_mutex_lock(&mutex);
		pthread_spin_lock(&mutex);
		res++;
		// 解锁
		// pthread_mutex_unlock(&mutex);
		pthread_spin_unlock(&mutex);
	}
	return NULL;
}

int main()
{
	// 初始化互斥锁
	// pthread_mutex_init(&mutex, NULL);
	pthread_spin_init(&mutex, PTHREAD_PROCESS_PRIVATE);
	
	pthread_t pthid[2];
	if(pthread_create(&pthid[0], NULL, threadMain, NULL) != 0)
	{
		printf("线程创建失败\n");
		return -1;
	}
	if(pthread_create(&pthid[1], NULL, threadMain, NULL) != 0)
	{
		printf("线程创建失败\n");
		return -1;
	}

	printf("join..\n");
	pthread_join(pthid[0], NULL);
	pthread_join(pthid[1], NULL);
	printf("join ok\n");

	// 销毁锁
	// pthread_mutex_destroy(&mutex);
	pthread_spin_destroy(&mutex);
	printf("%d\n", res);
	return 0;
}