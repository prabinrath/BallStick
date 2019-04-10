#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using namespace std;

class CSocketServer
{
	private:
		int PORT;
		int ssock, csock;
		int nread;
		struct sockaddr_in client;
		int createSocket();
	public:
		CSocketServer(int port);
		void sendMsg(void* msg, uint32_t msgsize);
		void talkToclient(char* msg,int msgsize, char *buff,int buffsize);
		void closeSocket();
};
