#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

/*
匿名管道适用于父子进程间通信
*/
int main()
{
	int fd[2];
	/*
	pipe函数创建管道, 产生两个文件描述符0为读 1为写
	父子进程在同时写一个文件时是安全的
	*/
	if(pipe(fd) == -1)
	{
		printf("管道创建失败\n");
		return -1;
	}
	char buf[1024];
	pid_t pid = fork();
	if(pid > 0)
	{
		// 关闭读头
		close(fd[0]);
		for(int i = 0; i < 10; i++)
		{
			printf("父进程向管道写入%d\n", i);
			sprintf(buf, "this is %d", i);
			write(fd[1], buf, sizeof(buf));
		}
	}
	else if(pid == 0)
	{
		// 关闭写头
		close(fd[1]);
		for(int i = 0; i < 10; i++)
		{
			read(fd[0], buf, sizeof(buf));
			printf("子进程从管道读取 %s\n", buf);
		}
	}
	else if(pid == -1)
	{
		printf("管道创建失败\n");
		return -1;
	}

	if(pid > 0)
		waitpid(-1, NULL, 0);
	return 0;
}