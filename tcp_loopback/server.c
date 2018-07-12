#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>

//#define LISTEN_IP "192.168.1.14"
#define LISTEN_IP "127.0.0.1"
#define LISTEN_PORT 30099
#define MAXLINE 8096
//#define NONBLOCKING

static int connections[FD_SETSIZE];

int create_socket(int *sockfd, const int type)
{
    if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    return 0;
}

int bind_socket(const int sockfd)
{
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, LISTEN_IP, &servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(LISTEN_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("bind");
        return -1;
    }
    return 0;
}

int wait_loop_forever(const int connfd, fd_set *rset, int *max_fd){
    int nready, i, size, flags, len;
    int sockfd;
    struct sockaddr_in cliaddr;
    char buf[1024], ipstring[32];
#ifdef NONBLOCKING
    flags = MSG_DONTWAIT;
#else
    flags = 0;
#endif
    FD_SET(connfd, rset);
    // blocking
    if((nready = select(*max_fd + 1, rset, NULL, NULL, NULL)) < 0){
        perror("select");
        return -1;
    }
    // process connfd
    if(FD_ISSET(connfd, rset)){
        len = sizeof(cliaddr);
        if ((sockfd = accept(connfd, (struct sockaddr *)&cliaddr, &len)) < 0){
            perror("accept");
            return -1;
        }
        for (i=0; i<FD_SETSIZE; i++){
            if(connections[i] < 0){
                connections[i] = sockfd;
                FD_SET(sockfd, rset);
            }
            if (sockfd > *max_fd)
                *max_fd = sockfd;
        }

        printf("Accepted connection: %s %d at fd:%d\n",
                inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), sockfd);
        nready--;
    }
    // process sockfds
    for (i = 0;i < FD_SETSIZE; i++){
        if ((sockfd = connections[i]) < 0)
            continue;
        if(FD_ISSET(sockfd, rset)){
            if ((size = recvfrom(sockfd, buf, sizeof(buf), flags, NULL, NULL)) < 0){
                perror("recvfrom");
                return -1;
            }
            printf("recv %s size:%d fd:%d\n", buf, size, sockfd);
            if ((size = sendto(sockfd, buf, sizeof(buf), flags, NULL, 0)) < 0){
                perror("sendto");
                return -1;
            }
            printf("send %s size:%d fd:%d\n", buf, size, sockfd);
            nready--;
            
        }
    }
    return nready;
}

int main(){
    int listenfd, i;
    fd_set rset;
    //socket
    create_socket(&listenfd, SOCK_STREAM);
    //bind
    bind_socket(listenfd);
    //listen
    listen(listenfd, 5);
    //reset
    FD_ZERO(&rset);
    for (i=0; i<FD_SETSIZE; i++){
        connections[i] = -1;
    }
    for(;;){
        if (wait_loop_forever(listenfd, &rset, &listenfd) > 0){
            perror("wait_loop_forever");
            return -1;
        }
    }
    return 0;
}
