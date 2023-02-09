#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int NUM = 128;

bool insert_fds(struct pollfd fds[], int fd, int events)
{
	for(int i = 0; i < NUM; i++)
	{
		if(fds[i].fd != -1) continue;
		fds[i].fd = fd;
		fds[i].events = events;
		return true;
	}
	return false;
}

int main()
{
	int serv_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(8888);

	bind(serv_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(serv_fd, SOMAXCONN);

	/*
	struct pollfd
	{
		int fd;			        // poll监听的文件描述符
		short int events;		// 该文件描述符监听的事件
		short int revents;		// 由内核填充，poll返回时用来标注就绪的事件类型
	};
	*/
	struct pollfd fds[NUM];

	for(int i = 0; i < NUM; i++) 
		fds[i].fd = -1, fds[i].events = 0;

	insert_fds(fds, serv_fd, POLLIN);

	char buf[1024];
	while(true)
	{
		/*
		int poll(struct pollfd *fds, nfds_t nfds, int timeout);
		fds是要监听的文件描述符结构体数组
		ndfs是要监听的文件描述符的最大数量
		timeout 设置超时以毫秒为单位, -1表示一直阻塞到有事件就绪
		返回值为就绪事件的数量, 返回值： == 0 超时 < 0 出错  > 0 就绪的文件描述符的个数
		*/
		int n = poll(fds, NUM, -1);
		if(n == 0) continue;
		if(n == -1)
		{
			printf("poll error\n");
			return -1;
		}
		for(int i = 0; i < NUM; i++)
		{
			if(fds[i].fd == -1) continue;
			if(fds[i].fd == serv_fd && (fds[i].revents & POLLIN))
			{
				struct sockaddr_in client_addr;
				socklen_t len = sizeof(client_addr);
				int client_fd = accept(serv_fd, (sockaddr*)&client_addr, &len);
				if(client_fd == -1) continue;

				printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				insert_fds(fds, client_fd, POLLIN); 
			}
			else if(fds[i].revents & POLLIN)
			{
				bzero(buf, sizeof(buf));
				ssize_t read_bytes = read(fds[i].fd, buf, sizeof(buf));
				if(read_bytes > 0) 
				{
					printf("%d : message is %s\n", fds[i].fd, buf);
					write(fds[i].fd, buf, sizeof(buf));
				}
				else if(read_bytes == 0)
				{
					printf("%d is disconnected\n", fds[i].fd);
					close(fds[i].fd);
					fds[i].fd = -1;
					fds[i].events = 0;
				}
				else if(read_bytes == -1)
				{
					printf("%d is error\n", fds[i].fd);
					close(fds[i].fd);
					fds[i].fd = -1;
					fds[i].events = 0;
				}
			}
		}
	}

	close(serv_fd);
	return 0;
}