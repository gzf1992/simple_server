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

#define LISTEN_IP "192.168.1.14" 
#define LISTEN_PORT 30099
#define MAXLINE 8096
#define MAXCONN FD_SETSIZE

inline int max(int a, int b){if (a>b) return a;else return b;}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    ssize_t n;
    if((n = recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) < 0){
        perror("recvfrom");
    }
    return n;
}

int main(){
    int sockfd, connfd, maxfd, maxfdsize;
    struct sockaddr_in servaddr, cliaddr;
    char buf[MAXLINE];
    int client[MAXCONN];
    int n, optval, len, nready, i;
    fd_set rset;
    for (i=0; i<MAXCONN; i++){
        client[i] = -1;
    }
    //socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    //bind 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, LISTEN_IP, &servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(LISTEN_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("bind");
        return 1;
    }
    //listen
    listen(sockfd, 5);

    //select
    FD_ZERO(&rset);
    maxfd = -1;
    for(;;){
        // accept connection
        FD_SET(sockfd, &rset);
        maxfdsize = max(sockfd, maxfd);
        if ((nready = select(maxfdsize + 1, &rset, NULL, NULL, NULL)) < 0 ){
            if (errno == EINTR)
                continue;
            else
                perror("select");
        }
        if (FD_ISSET(sockfd, &rset)){ // new connection
            len = sizeof(cliaddr);
            connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
            if(connfd > maxfd) maxfd = connfd;
            for (i = 0; i < MAXCONN; i++){
                if(client[i] < 0) break;
            }
            client[i] = connfd;
            FD_SET(connfd, &rset);
            printf("accepted connection! fd:%d\n", connfd);
        }
        // process request
        for (i = 0; i < MAXCONN; i++){
            if((connfd = client[i]) < 0){
                continue;
            }
            if(FD_ISSET(connfd, &rset)){
                n = Recvfrom(connfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
                printf("receive %s %d %d\n", buf, n, connfd);
                sendto(connfd, (void *)buf, n, 0, (struct sockaddr *)&cliaddr, len);
                printf("send %s %d %d\n", buf, n, connfd);
            }
        }
    }
    return 0;
}
