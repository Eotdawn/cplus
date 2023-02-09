#include <stdio.h>
#include <fcntl.h>	
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

struct Message
{
	int len;
	char buf[1024];
};

int main()
{
	int mlen = sizeof(Message);
	int fd = open("file.txt", O_RDWR | O_CREAT, 0644);
	/*
	设置文件偏移量
	SEEK_SET: 
 　　从文件头部开始偏移offset个字节。 
	SEEK_CUR： 
	　　从文件当前读写的指针位置开始，增加offset个字节的偏移量。 
	SEEK_END： 
	　　文件偏移量设置为文件的大小加上偏移量字节。
	*/
	lseek(fd, mlen, SEEK_SET);
	if(fd == -1)
	{
		printf("打开文件失败\n");
		return -1;
	}
	/*
	void* mmap(void* start,size_t length,int prot,int flags,int fd,off_t offset);
	参数start：指向欲映射的内存起始地址，通常设为 NULL，代表让系统自动选定地址，映射成功后返回该地址。

	参数length：代表将文件中多大的部分映射到内存。

	参数prot：映射区域的保护方式。可以为以下几种方式的组合：
	PROT_EXEC 映射区域可被执行
	PROT_READ 映射区域可被读取
	PROT_WRITE 映射区域可被写入
	PROT_NONE 映射区域不能存取

	参数flags：影响映射区域的各种特性。在调用mmap()时必须要指定MAP_SHARED 或MAP_PRIVATE。
	MAP_FIXED 如果参数start所指的地址无法成功建立映射时，则放弃映射，不对地址做修正。通常不鼓励用此旗标。
	MAP_SHARED对映射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享。
	MAP_PRIVATE 对映射区域的写入操作会产生一个映射文件的复制，即私人的“写入时复制”（copy on write）对此区域作的任何修改都不会写回原来的文件内容。
	MAP_ANONYMOUS建立匿名映射。此时会忽略参数fd，不涉及文件，而且映射区域无法和其他进程共享。
	MAP_DENYWRITE只允许对映射区域的写入操作，其他对文件直接写入的操作将会被拒绝。
	MAP_LOCKED 将映射区域锁定住，这表示该区域不会被置换（swap）。

	参数fd：要映射到内存中的文件描述符。如果使用匿名内存映射时，即flags中设置了MAP_ANONYMOUS，fd设为-1。有些系统不支持匿名内存映射，则可以使用fopen打开/dev/zero文件，然后对该文件进行映射，可以同样达到匿名内存映射的效果。

	参数offset：文件映射的偏移量，通常设置为0，代表从文件最前方开始对应，offset必须是分页大小的整数倍。
	*/
	void *p = mmap(NULL, mlen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED)
	{
		printf("内存映射失败\n");
		return -1;
	}
	struct Message* m = (struct Message *)p;
	sprintf(m->buf, "hello world\n");
	m->len = strlen(m->buf);

	/*
	int munmap(void* start,size_t length);
	*/
	int ret = munmap(p, mlen);
	if(ret == -1)
	{
		printf("内存解映射失败\n");
		return -1;
	}
	close(fd);
	return 0;
}