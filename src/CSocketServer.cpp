#include <CSocketServer.h>

CSocketServer::CSocketServer(int port)
{
	PORT = port;
	ssock = createSocket();
    printf("Server listening on port %d\n", PORT);
    socklen_t clilen = sizeof(client);
    csock = accept(ssock, (struct sockaddr *)&client, &clilen);
    if (csock < 0)
    {
        printf("Error: accept() failed\n");
    }
    printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));
}

int CSocketServer::createSocket()
{
    int sock, err;
    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("ERROR: Socket creation failed\n");
        exit(1);
    }
    printf("Socket created\n");

    bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("ERROR: Bind failed\n");
        exit(1);
    }
    printf("Bind done\n");

    listen(sock , 3);

    return sock;
}

void CSocketServer::closeSocket()
{
    close(csock);
    return;
}

void CSocketServer::sendMsg(void* msg, uint32_t msgsize)
{
    if (write(csock, msg, msgsize) < 0)
    {
        printf("Can't send message.\n");
        closeSocket();
        exit(1);
    }
    return;
}
void CSocketServer::talkToclient(char* msg,int msgsize, char *buff,int buffsize)
{
	
	sendMsg(msg, msgsize);
    while ((nread=read(csock, buff, buffsize)) > 0)
    {
        break;
    }
}
