#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>

key_t key = 0x666;
struct Message
{
	long msg_type;
	char buf[1024];
};

int main()
{
	/*
	int msgget(key_t key, int msgflg);
	key: 某个消息队列的名字
	msgflg:由九个权限标志构成
		IPC_CREAT
			如果消息队列对象不存在，则创建之，否则则进行打开操作
		IPC_EXCL
			如果消息对象不存在则创建之，否则产生一个错误并返回
	返回值：成功返回标识码 失败-1
	*/
	int msqid = msgget(key, 0666|IPC_CREAT);
	pid_t pid = fork();
	if(pid > 0)
	{
		struct Message q;
		q.msg_type = 1;// 不能为0
		sprintf(q.buf, "hello world");
		/*
		int  msgsnd(int msgid, const void *msg_ptr, size_t msg_sz, int msgflg);
		msgid: 由msgget函数返回的消息队列标识码
		msg_ptr:是一个指针，指针指向准备发送的消息，
		msg_sz:是msg_ptr指向的消息长度，消息缓冲区结构体中mtext的大小,不包括数据的类型
		msgflg:控制着当前消息队列满或到达系统上限时将要发生的事情
		如：
		msgflg = IPC_NOWAIT 表示队列满不等待，返回EAGAIN错误
		返回值：成功返回0 失败则返回-1
		*/
		if(msgsnd(msqid, &q, sizeof(q.buf), 0) == -1)
		{
			printf("发送消息失败\n");
			return -1;
		}
		waitpid(-1, NULL, 0);
		/*
		int  msgctl(int msqid, int command, strcut msqid_ds *buf);
		msqid: 由msgget函数返回的消息队列标识码
		command:是将要采取的动作,（有三个可取值）分别如下
			IPC_STAT: 把msqid ds结构中的数据设置为消息队列的当前关联值
			IPC_SET: 在进程有足够权限的前提下，把消息队列的当前关联值设置为msqid ds数据结构中给出的值
			IPC_RMID: 删除消息队列
		*/
		msgctl(msqid, IPC_RMID, 0);
	}
	else if(pid == 0)
	{
		struct Message q;
		/*
		int  msgrcv(int msgid, void *msg_ptr, size_t msgsz, long int msgtype, int msgflg);
		msgid: 由msgget函数返回的消息队列标识码
		msg_ptr:是一个指针，指针指向准备接收的消息，
		msgsz:是msg_ptr指向的消息长度，消息缓冲区结构体中mtext的大小,不包括数据的类型
		msgtype:它可以实现接收优先级的简单形式
		msgtype=0返回队列第一条信息
		msgtype>0返回队列第一条类型等于msgtype的消息　
		msgtype<0返回队列第一条类型小于等于msgtype绝对值的消息
		msgflg:控制着队列中没有相应类型的消息可供接收时将要发生的事
		msgflg=IPC_NOWAIT，队列没有可读消息不等待，返回ENOMSG错误。
		msgflg=MSG_NOERROR，消息大小超过msgsz时被截断
		返回值：成功返回实际放到接收缓冲区里去的字符个数 失败，则返回-1
		*/
		if(msgrcv(msqid, &q, sizeof(q.buf), 0, 0) == -1)
		{
			printf("接受消息失败\n");
			return -1;
		}
		printf("接受了消息%s\n", q.buf);
	}
	else if(pid == -1)
	{
		printf("子进程创建失败\n");
		return -1;
	}
	return 0;
}