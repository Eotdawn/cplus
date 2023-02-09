#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
	/*
	第一个参数：IP地址类型，AF_INET表示使用IPv4，如果使用IPv6请使用AF_INET6。
	第二个参数：数据传输方式，SOCK_STREAM表示流格式、面向连接，多用于TCP。SOCK_DGRAM表示数据报格式、无连接，多用于UDP。
	第三个参数：协议，0表示根据前面的两个参数自动推导协议类型。设置为IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP和UDP。
	成功会返回大于等于0 失败返回-1
	*/
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);

	// tcp专用地址结构
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = INADDR_ANY;// inet_addr("127.0.0.1")

	// 将地址结构绑定到文件描述符上 成功返回0 失败返回-1
    bind(serv_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));

	// 监听端口, 这个函数的第二个参数是listen函数的最大监听队列长度 成功返回0 失败返回-1
    listen(serv_fd, SOMAXCONN);
    
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

	// 接受一个客户端的连接 成功返回0 失败
    int clnt_fd = accept(serv_fd, (sockaddr*)&clnt_addr, &clnt_addr_len);

	char buf[1024];     //定义缓冲区
	while (true) 
	{
		bzero(buf, sizeof(buf));       //清空缓冲区
		ssize_t read_bytes = read(clnt_fd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
		if(read_bytes > 0) // 正常读取数据
		{
			printf("read %s from %s:%d\n", buf, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
			write(clnt_fd, buf, sizeof(buf));
		} 
		else if(read_bytes == 0) // 链接关闭
		{
			printf("client %s isconnected\n", inet_ntoa(serv_addr.sin_addr));
			break;
		} 
		else if(read_bytes == -1)// 发生错误
		{
			printf("client %s is error\n", inet_ntoa(serv_addr.sin_addr));
			break;
		}
	}
	close(clnt_fd);
	close(serv_fd);
    return 0;
}