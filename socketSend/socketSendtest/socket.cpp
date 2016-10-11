# include <winsock2.h>
#include "StdAfx.h"


int ConnectWithTimeout(SOCKET socket,/* char * host, int port, */  struct sockaddr *address, int timeout)
{
	/*
	1.���Ƚ���־λ��ΪNon-blockingģʽ��׼���ڷ�����ģʽ�µ���connect����
	2.����connect����������£���ΪTCP����������ҪһЩʱ�䣻������������ֻҪ����������ɾͻ᷵�ش�����������᷵��EINPROGRESS����ʾ�ڽ������ӵ���û����ɡ�
	3.�ڶ��׽ӿ���������(fd_set rset)��д�׽ӿ���������(fd_set 
	wset)�н���ǰ�׽ӿ���λ����FD_ZERO()��FD_SET()�꣩�������úó�ʱʱ��(struct 
	timeval *timeout)
	4.����select( socket, &rset, &wset, NULL, timeout )
	����0��ʾconnect��ʱ
	��������õĳ�ʱʱ�����75���û�б�Ҫ�������ˣ���Ϊ�ں��ж�connect�г�ʱ���ƾ���75�롣
	*/
	

	/*
	1������socket
	2������socket����Ϊ������ģʽ
	3������connect()
	4��ʹ��select()����socket�������Ƿ��д��ע�⣬�ǿ�д��
	5������select()���صĽ���ж�connect()���
	6����socket����Ϊ����ģʽ�������ĳ�����Ҫ������ģʽ�ģ��ⲽ��ʡ�ˣ�����һ������¶���������ģʽ�ģ�����Ҳ���׹���
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
	ioctlsocket(socket, FIONBIO, &ul); //����Ϊ����ģʽ
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
	//���÷��ͳ�ʱ
	setsockopt( *s,SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout));
	//���ý��ճ�ʱ
	setsockopt(*s, SOL_SOCKET, SO_RCVTIMEO ,(char *)&timeout,sizeof(timeout));

	// ��д�ͻ��˵�ַ��Ϣ
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




