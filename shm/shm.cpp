#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

key_t key = 0x6666;

struct Message
{
	int len;
	char buf[1024];
};


int main()
{
	struct Message* p = NULL;
	int mlen = sizeof(struct Message);

	/*
	int shmget(key_t key, size_t size, int shmflg)
	创建共享内存, 或者获取共享内存
	key: 需要指定key32位 
	size: 共享内存长度 
	shmflg: 类似文件的权限|(IPC_CREAT //如果不存在就创建 IPC_EXCL //如果存在则返回失败 IPC_NOWAIT //如不等待直接返回)
	返回值成功：返回共享内存的标识符 出错：-1
	*/
	int shmid = shmget(key, mlen, 0640|IPC_CREAT);

	/*
	void *shmat(int shmid, const void *shmaddr, int shmflg)
	将共享内存连接到进程中
	shmid: 共享内存的id 
	shmaddr: 指定的连接地址或者NULL由操作系统自己分配(自己分配的为返回值)  
	shmflg: SHM_RDONLY：为只读模式，其他为读写模式
	返回值出错是-1
	*/
	p = (struct Message*)shmat(shmid, NULL, 0);

	printf("%d %s\n", p->len, p->buf);

	sprintf(p->buf, "hello world");
	p->len = strlen(p->buf);
	printf("共享内存写入\n");

	/*
	int shmdt(const void *shmaddr)
	将共享内存断开到进程中
	shmaddr：连接的共享内存的起始地址
	返回值成功0 出错-1
	*/
	shmdt((void*)p);

	/*
	int shmctl(int shmid, int cmd, struct shmid_ds *buf)
	shmid: 共享内存标识符
	cmd: 
		IPC_STAT：得到共享内存的状态，把共享内存的shmid_ds结构复制到buf中
		IPC_SET：改变共享内存的状态，把buf所指的shmid_ds结构中的uid、gid、mode复制到共享内存的shmid_ds结构内
		IPC_RMID：删除这片共享内存
	buf:共享内存管理结构体。具体说明参见共享内存内核结构定义部分
	返回值 成功0 出错-1
	*/
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}