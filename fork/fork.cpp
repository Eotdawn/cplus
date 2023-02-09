#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>  
#include <sys/types.h>

int main()
{
	/*
	孤儿进程,父进程比子进程先运行完, 子进程后续会有操作系统进行资源释放
	僵尸进程,子进程比父进程先运行完, 如果父进程不进行子进程的资源释放就会造成资源浪费
	*/

	// 主进程忽略子进程处理信号后, 子进程结束将由操作系统回收资源
	// signal(SIGCHLD, SIG_IGN);
	
	printf("主进程id=%d\n", getpid());
	int pid = fork();
	if(pid > 0)
		printf("创建了子进程id=%d\n", pid);
	else if(pid == 0)
		printf("创建子进程id=%d, 父进程id=%d\n", getpid(), getppid());
	else if(pid == -1)
		printf("创建子进程失败\n");

	if(pid > 0)
	{
		/*
		pid_ t waitpid(pid_t pid, int *status, int options);
		pid:
			pid==-1 等待任一子进程。这个waitpid与wait等效； wait(xx) == waitpid(-1, xx, 0);
			pid>0 等待进程ID与pid相等的子进程；
			pid==0 等待其组ID等于调用进程ID的任一子进程；
			pid < -1 等待期组ID等于pid绝对值的任一子进程；
		status:
			传入NULL不关系子进程的退出状态 
			WIFEXITED(status): 若为正常终止子进程返回的状态，则为真。（查看进程是否是正常退出）
			WEXITSTATUS(status): 若WIFEXITED非零，提取子进程退出码。（查看进程的退出码）
		options:
			0阻塞等待
			WNOHANG: 这是一个宏，表示调用wait为非阻塞的版本，非阻塞也就以为执行带waitpid函数会立即返回；
		返回值:
			若pid指定的子进程没有结束，则waitpid()函数返回0，父进程不予以等待；
			若正常结束，则返回该子进程的ID；
			若等待失败，即返回小于0；
		*/
		int id = waitpid(-1, NULL, 0);
		printf("子进程id=%d已经退出\n", id);
	}
	return 0;
}