#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define LISTEN_IP "192.168.1.14" 
#define LISTEN_PORT 30088
#define MAXLINE 8096

int main(){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buf[MAXLINE];
    int n, optval, len, nready;
    fd_set rset;
    //socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    optval = 1;
    //socket option
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR|SO_BROADCAST, &optval, sizeof(int));
    //bind 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, LISTEN_IP, &servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(LISTEN_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("bind");
        return 1;
    }
    //select
    FD_ZERO(&rset);
    while(1){
        FD_SET(sockfd, &rset);
        if ((nready = select(sockfd+1, &rset, NULL, NULL, NULL)) < 0 ){
            perror("select");
        }
        if (FD_ISSET(sockfd, &rset)){
            len = sizeof(cliaddr);
            if ((n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len)) < 0){
                perror("receive");
                return 1;
            }
            printf("receive %s %d\n", buf, n);
            sendto(sockfd, (void *)buf, n, 0, (struct sockaddr *)&cliaddr, len);
        }
    }
    return 0;
}
