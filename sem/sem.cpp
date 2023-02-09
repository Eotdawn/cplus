// 本程序演示线程同步-信号量。
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int var;

sem_t sem;      // 声明信号量。

void *thmain(void *arg);    // 线程主函数。

int main(int argc,char *argv[])
{
    /*
       该函数初始化由sem指向的信号对象，设置它的共享选项，并给它一个初始的整数值。
       pshared控制信号量的类型，如果其值为0，就表示这个信号量是当前进程的局部信号量，否则信号量就可以在多个进程之间共享，
       value为sem的初始值。
       调用成功时返回0，失败返回-1.
       */
    sem_init(&sem, 0, 1);   // 初始化信号量。

    pthread_t thid1,thid2;

    // 创建线程。
    if (pthread_create(&thid1,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
    if (pthread_create(&thid2,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

    // 等待子线程退出。
    printf("join...\n");
    pthread_join(thid1,NULL);  
    pthread_join(thid2,NULL);  
    printf("join ok.\n");

    printf("var=%d\n",var);

    sem_destroy(&sem);  // 销毁信号量。
}

void *thmain(void *arg)    // 线程主函数。
{
    for (int ii=0;ii<1000000;ii++)
    {
        sem_wait(&sem);    // p
        var++;
        sem_post(&sem);  // v
    }
	return NULL;
}
