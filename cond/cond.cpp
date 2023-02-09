#include <queue>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

std::queue<int> q;
pthread_cond_t cond;
pthread_mutex_t mutex;

void Producer(int sig)
{
	static int cnt = 0;
	
	pthread_mutex_lock(&mutex);
	
	for(int i = 1; i <= 5; i++)
		q.push(cnt++);
	
	pthread_mutex_unlock(&mutex);

	// pthread_cond_signal(&cond); // 唤醒一个
	pthread_cond_broadcast(&cond); // 唤醒全部
}

void clean(void *arg)
{
	pthread_mutex_unlock(&mutex);

	printf("子线程%lld清理资源\n", (long long)arg);
}

void *Consumer(void *arg)
{

	int mgs;
	pthread_cleanup_push(clean, arg);
	for(int i = 1; i <= 10; i++)
	{
		pthread_mutex_lock(&mutex);
		
		/*
		pthread_cond_wait 等待条件变量的流程是解锁 等待唤醒 加锁
		*/
		while(q.empty()) pthread_cond_wait(&cond, &mutex);

		mgs = q.front();
		q.pop();

		pthread_mutex_unlock(&mutex);

		printf("子线程%lld消费了%d\n", (long long)arg, mgs);
	}
	pthread_cleanup_pop(1);
	return NULL;
}

int main()
{
	signal(15, Producer);

	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	pthread_t pthid[10];
	for(int i = 0; i < 10; i++)
		if(pthread_create(&pthid[i], NULL, Consumer, (void *)(long)i) != 0)
		{
			printf("创建子线程失败\n");
			return -1;
		}

	printf("join..\n");
	for(int i = 0; i < 10; i++)
		if(pthread_join(pthid[i], NULL) != 0)
		{
			printf("回收子线程失败\n");
			return -1;
		}
	printf("join ok\n");

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
	return 0;
}