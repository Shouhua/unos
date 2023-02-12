
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/listener.h>

int createListenfd(char *addr, int port)
{
    int listenFd = -1;
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        return -1;
    }
    struct sockaddr_in localAddr = {0};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port);
    localAddr.sin_addr.s_addr = inet_addr(addr);

    if (bind(listenFd, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0)
    {
        close(listenFd);
        return - 1;
    }
    if(listen(listenFd, 64) < 0)
    {
        close(listenFd);
        return -1;
    }
    return listenFd;
}

void tcpEchoCb(evutil_socket_t fd, short event, void *arg)
{
    char buf[1024] = {0};
    int n = 0;

    n = read(fd, buf, 1024);
    if(n < 0)
    {
        return;
    }
    if(n == 0)
    {
        close(fd);
        event_del((struct event *)arg);
    }
    printf("Server receive msg is %s, len is %d\n", buf, n);
    write(fd, buf, n);
}

void tcpEchoConnCb(
    struct evconnlistener *evconn, 
    evutil_socket_t fd, 
    struct sockaddr *remoteAddr, 
    int socklen,
    void *arg
)
{
    struct sockaddr_in *clientAddr =(struct sockaddr_in *)remoteAddr;
    printf("New client is %s:%d connected...\n", inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port));
    struct event_base *base = NULL;
    struct event *evTcpEcho;
    base = arg;

    evTcpEcho = event_new(base, fd, EV_READ | EV_PERSIST, tcpEchoCb, event_self_cbarg());
    event_add(evTcpEcho, NULL);
}

void tcpConnReqCb(evutil_socket_t fd, short event, void *arg)
{
   printf("fd is %d\n", fd); 
   struct sockaddr_in clientAddr;
   int len = sizeof(clientAddr);
   struct event *evTcpEcho = NULL;
   struct event_base *base = NULL;

   int clientfd;
   clientfd = accept(fd, (struct sockaddr *)&clientAddr, &len);
   if(clientfd < 0)
   {
        return;
   }

   base = event_get_base((struct event *)arg);
   evTcpEcho = event_new(base, clientfd, EV_READ | EV_PERSIST, tcpEchoCb, event_self_cbarg());
   event_add(evTcpEcho, NULL);
   printf("New client is %s:%d connected...\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
}

int main(int argc, char *argv[])
{
    struct event_base *base = NULL;
    struct event *eventConnReq = NULL;
    // int listenFd = createListenfd("127.0.0.1", 12345);
    // if(listenFd < 0)
    // {
    //     return -1;
    // }

    // printf("Current listen fd is %d\n", listenFd);
    base = event_base_new();
    if(!base)
    {
        // close(listenFd);
        return -1;
    }

    struct sockaddr_in localAddr = {0};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(12345);
    localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    struct evconnlistener *evConnListen = evconnlistener_new_bind(
        base,
        tcpEchoConnCb,
        (void *)base,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        64,
        (struct sockaddr *)&localAddr,
        sizeof(localAddr)
    );

    if(!evConnListen)
    {
        event_base_free(base);
        return -1;
    }

    // eventConnReq = event_new(base, listenFd, EV_READ | EV_PERSIST, tcpConnReqCb, event_self_cbarg());
    // if(!eventConnReq)
    // {
    //     close(listenFd);
    //     event_base_free(base);
    //     return -1;
    // }

    // event_add(eventConnReq, NULL);
    event_base_dispatch(base);

    evconnlistener_free(evConnListen);
    event_base_free(base);
    return 0;
}