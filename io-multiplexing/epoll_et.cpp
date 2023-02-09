#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int NUM = 1024;

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main()
{
	int serv_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(8888);

	bind(serv_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(serv_fd, SOMAXCONN);

	/*
	在内核里创建一个管理socket的事件表
	*/
	int epfd = epoll_create(1);

	/*
	epfd: epoll_create方法返回的文件描述符，指定要操作的内核事件表的文件描述符
	op: 指定操作类型，添加 EPOLL_CTL_ADD, 修改 EPOLL_CTL_MOD, 删除 EPOLL_CTL_DEL
	fd: 指定要操作的文件描述符
	event：指定事件，它是 epoll_event 结构指针类型， epoll_event 的定义如下：
	struct epoll_event
	{
		uint32_t events;  //事件的集合
		epoll_data_t data;//用户数据，data.fd 文件描述符
	};
	typedef union epoll_data
	{
		int fd;  //文件描述符
		void *ptr;//用户数据
		uint32_t u32;
		uint64_t u64;
	}epoll_data_t;
	*/
	struct epoll_event ev;
	ev.data.fd = serv_fd;
	ev.events = EPOLLIN | EPOLLET; // 设置边沿触发
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_fd, &ev);

	char buf[1024];
	while(true)
	{
		struct epoll_event evs[NUM];
		/*
		expfd：这次处理的内核事件表的文件描述符
		events：用户数组，这个用户数组所有的都是内核填充的，用来返回所有就绪的文件描述符以及事件，这个数组仅仅在 epoll_wait
		返回时保存内核检测到的所有就绪事件，而不像 select 和 poll
		的数组参数那样既用于传入用户注册的事件，又用于输出内核检测到的就绪事件。这就极大地提高了应用程序索引就绪文件描述符的效率
		maxevents：events数组的长度，即指定最多监听多少个事件
		timeout：定时时间，单位为毫秒，如果 timeout 为 0，则 epoll_wait 会立即返回，如果timeout 为-1，则 epoll_wait 会一直阻塞，直到有事件就绪。
		返回值：成功返回就绪的文件描述符的个数，出错返回-1，超时返回0
		*/
		int n = epoll_wait(epfd, evs, NUM, -1);
		if(n == 0) continue;
		if(n == -1)
		{
			printf("poll error\n");
			return -1;
		}
		for(int i = 0; i < n; i++)
		{
			if(evs[i].data.fd == serv_fd)
			{
				struct sockaddr_in client_addr;
				socklen_t len = sizeof(client_addr);
				int client_fd = accept(serv_fd, (sockaddr*)&client_addr, &len);
				if(client_fd == -1) continue;

				struct epoll_event ev;
				ev.data.fd = client_fd;
				ev.events = EPOLLIN | EPOLLET;
				setnonblocking(client_fd);
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
				printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			}
			else if(evs[i].data.fd != serv_fd)
			{
				if(evs[i].events & EPOLLIN)
				{
					// 在边沿触发中因为事件只能触发一次, 要一次读取所有数据
					while(true)
					{
						bzero(buf, sizeof(buf));
						ssize_t read_bytes = read(evs[i].data.fd, buf, sizeof(buf));
						if(read_bytes > 0) 
						{
							printf("%d : message is %s\n", evs[i].data.fd, buf);
							write(evs[i].data.fd, buf, sizeof(buf));
						}
						else if(read_bytes == 0)  // EOF, 客户端断开连接
						{
							printf("%d disconnected\n", evs[i].data.fd);
							epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
							close(evs[i].data.fd);
							break;
						}
						else if(read_bytes == -1 && errno == EINTR) // 客户端正常中断、继续读取
						{
							continue;
						}
						else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 非阻塞IO，这个条件表示数据全部读取完毕
						{
							printf("finish reading once, errno: %d\n", errno);
							break;
						}
						else if(read_bytes == -1) // 发生错误
						{
							printf("%d error\n", evs[i].data.fd);
							epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
							close(evs[i].data.fd);
							break;
						}
					}
				}
			}
		}
	}

	close(serv_fd);
	return 0;
}