# include <winsock2.h>
#include "StdAfx.h"


int ConnectWithTimeout(SOCKET socket,/* char * host, int port, */  struct sockaddr *address, int timeout)
{
	/*
	1.首先将标志位设为Non-blocking模式，准备在非阻塞模式下调用connect函数
	2.调用connect，正常情况下，因为TCP三次握手需要一些时间；而非阻塞调用只要不能立即完成就会返回错误，所以这里会返回EINPROGRESS，表示在建立连接但还没有完成。
	3.在读套接口描述符集(fd_set rset)和写套接口描述符集(fd_set 
	wset)中将当前套接口置位（用FD_ZERO()、FD_SET()宏），并设置好超时时间(struct 
	timeval *timeout)
	4.调用select( socket, &rset, &wset, NULL, timeout )
	返回0表示connect超时
	如果你设置的超时时间大于75秒就没有必要这样做了，因为内核中对connect有超时限制就是75秒。
	*/
	

	/*
	1．建立socket
	2．将该socket设置为非阻塞模式
	3．调用connect()
	4．使用select()检查该socket描述符是否可写（注意，是可写）
	5．根据select()返回的结果判断connect()结果
	6．将socket设置为阻塞模式（如果你的程序不需要用阻塞模式的，这步就省了，不过一般情况下都是用阻塞模式的，这样也容易管理）
	*/
	int ret = 0;
	TIMEVAL timeval = {0};
	timeval.tv_sec = timeout;
	timeval.tv_usec = 0;
	unsigned long mode = 1;
	ret = ioctlsocket(socket, FIONBIO, &mode);
	if (ret != NO_ERROR) 
		return ret;

	/////////// connect with timeout ///////////////////
	struct linger			Linger;
	fd_set 				set;
	unsigned long     ul;
	if (connect(socket,(struct sockaddr *)(address),sizeof(struct sockaddr_in)) < 0)
	{
		if (errno != 0 && errno != EINPROGRESS)
			return -2;

		FD_ZERO(&set);
		FD_SET(socket, &set);
		ret = select(socket+1, NULL, &set, NULL, &timeval);//writeSet
		if (ret == -1 || ret == 0 )
			return -3;

		int error = -1;
		if( FD_ISSET(socket,&set) )
		{
			int len = sizeof (int);
			if(getsockopt(socket, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0)
				return(-4);

			if (error == ETIMEDOUT)
				return(-5);

			if(error == ECONNREFUSED)
				return(-6);
			/* */
		}

	}

	Linger.l_onoff = 1;
	Linger.l_linger = 0;
	setsockopt(socket,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger));
	int on = 1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));	
	ul = 0;
	ioctlsocket(socket, FIONBIO, &ul); //设置为阻塞模式
	return(0);
}
	

void loadSockLib()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2) , &wsaData);
}


int initSocket(char* ip , int port , SOCKET *s , SOCKADDR_IN  *ServerAddr)
{

	// Create a SOCKET for connecting to server
	*s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct timeval timeout = {2,0};  //2s
	//setsockopt( *s,SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout));
	//设置发送超时
	setsockopt( *s,SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout));
	//设置接收超时
	setsockopt(*s, SOL_SOCKET, SO_RCVTIMEO ,(char *)&timeout,sizeof(timeout));

	// 填写客户端地址信息
	ServerAddr->sin_family = AF_INET;
	ServerAddr->sin_port = htons(port);    
	ServerAddr->sin_addr.s_addr = inet_addr(ip);	
	
	if ((  ConnectWithTimeout(*s, (struct sockaddr *)ServerAddr  , timeout.tv_sec ) ) < 0 )
	{
		closesocket (*s);
		return -1;
	}
	return 0;
}






int mysend(SOCKET *fd , SOCKADDR_IN  *ServerAddr ,unsigned char *msg ,int lenofMsg)
{
	int iResult = send(*fd , (char*)msg ,lenofMsg  , 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(*fd);
		return -1;
	}
	return iResult;
}


int myrecv(SOCKET *fd, unsigned char *buf ,int len)
{
	int iResult;
	int recLen = 0;
	do {
		iResult = recv(*fd, (char*)buf, len, MSG_WAITALL);
		recLen += iResult;
	} while( iResult > 0 );
	return recLen;
}




