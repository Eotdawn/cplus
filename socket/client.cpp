#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8888);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));

	char buf[1024];
	while(true)
	{
		bzero(buf, sizeof(buf));
		scanf("%s", buf);
		// 返回值为0代表写缓冲区满
		ssize_t write_bytes = write(client_fd, buf, sizeof(buf));
		if(write_bytes <= 0) 
		{
			printf("socket already disconnected, can't write any more!\n");
			break;
		}
		bzero(buf, sizeof(buf));
		ssize_t read_bytes = read(client_fd, buf, sizeof(buf));
		if(read_bytes > 0) // 正常读取数据
		{
			printf("read %s from %s:%d\n", buf, inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
		} 
		else if (read_bytes == 0) // 链接关闭
		{
			printf("client %s isconnected\n", inet_ntoa(serv_addr.sin_addr));
			break;
		} 
		else // 发生错误
		{
			printf("client %s is error\n", inet_ntoa(serv_addr.sin_addr));
			break;
		}
	}
	close(client_fd);
	return 0;
}