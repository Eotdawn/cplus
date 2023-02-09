#include <stdio.h>
#include <pthread.h>

void threadClean(void *arg)
{
	printf("子线程%lld调用了线程清理函数\n", (long long)arg);
}

void *threadMain(void *arg)
{
	//线程函数入栈
	pthread_cleanup_push(threadClean, arg);

	pthread_t pthid = pthread_self();
	/*
	线程分离，注意线程分离后，会导致join立即返回直接失败
	*/
	// pthread_detach(pthid);
	printf("子线程%lld运行中...\n", (long long)arg);

	//线程函数出栈
	pthread_cleanup_pop(1);

	// 线程退出函数, 可以用来传递一些返回值啥的
	if(arg == (void *)1)
		pthread_exit((void *)1);
	return (void *)2;
}

int main()
{
	pthread_t tid[2];
	/*
	线程也有类似于僵尸的状态, 僵尸线程只有join后才能释放资源
	也可以使用类似信号的方法将子线程分离, 来让操作系统来直接释放线程的资源
	*/
	/*
	int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
	thread
		属于结果参数。函数结束时，返回线程ID并存储到thread中。
		pthread_t被定义成unsigned long int类型。
	attr
		设置线程的属性，主要是栈相关的属性。
	start_routine
		start_routine是一个回调函数（函数指针实现）。指明了线程要执行的函数。
	arg
		回调函数start_routine()执行时的参数（实参）。
	*/
	int ret = pthread_create(&tid[0], NULL, threadMain, (void *)1);
	if(ret != 0)
	{
		printf("线程1创建失败\n");
	}
	ret = pthread_create(&tid[1], NULL, threadMain, (void *)2);
	if(ret != 0)
	{
		printf("线程2创建失败\n");
	}

	/*
	等待线程退出
	int pthread_join(pthread_t thread, void **retval);
	thread
		被等待线程的ID
	retval
		如果此值非NULL，pthread_join复制线程的退出状态
	返回值
		成功返回0，失败返回非0
	*/
	int res[2];
	long long tres[2];
	printf("join...\n");
	res[0] = pthread_join(tid[0], (void **)&tres[0]);
	res[1] = pthread_join(tid[1], (void **)&tres[1]);
	printf("join ok\n");
	
	printf("线程%d 返回状态%d 返回%lld\n", 0 + 1, res[0], tres[0]);
	printf("线程%d 返回状态%d 返回%lld\n", 1 + 1, res[1], tres[1]);
	return 0;
}