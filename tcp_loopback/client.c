#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <strings.h>
#include <stdio.h>

#define TARGET_IP "192.168.1.14" 
#define TARGET_PORT 30099
//#define NONBLOCKING

int create_socket(int *sockfd, const int type)
{
    if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    return 0;
}

int connect_server(const int sockfd)
{
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, TARGET_IP, &servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(TARGET_PORT);
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))){
        perror("connect");
        return -1;
    }
}

int loop_forever(const int sockfd)
{
    int size, flags;
    struct sockaddr_in cliaddr;
    char buf[1024] = "HELLO";
#ifdef NONBLOCKING
    flags = MSG_DONTWAIT;
#else
    flags = 0;
#endif
    //size = send(sockfd, buf, sizeof(buf), flags);
    if ((size = sendto(sockfd, buf, sizeof(buf), flags, NULL, 0)) < 0){
        perror("sendto");
        return -1;
    }
    printf("send %s size:%d fd:%d\n", buf, size, sockfd);
    bzero(buf, sizeof(buf));
    sleep(1);
    //size = recv(sockfd, buf, sizeof(buf), flags);
    if ((size = recvfrom(sockfd, buf, sizeof(buf), flags, NULL, NULL)) < 0){
        perror("recvfrom");
        return -1;
    }
    printf("recv %s size:%d fd:%d\n", buf, size, sockfd);
    sleep(1);
    return 0;
}

int main(){
    int sockfd;
    // create socket
    create_socket(&sockfd, SOCK_STREAM);
    // connect to server
    connect_server(sockfd);
    // send to server
    for(;;){
        loop_forever(sockfd);
    }
    return 0;
}
