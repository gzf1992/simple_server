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
#define LISTEN_PORT 30099
#define MAXLINE 8096
#define MAXCONN 8096
inline int max(int a, int b){if (a>b) return a;else return b;}

int main(){
    int sockfd, connfd, maxfdsize;
    struct sockaddr_in servaddr, cliaddr;
    char buf[MAXLINE];
    int client[MAXCONN] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int n, optval, len, nready, i;
    fd_set rset;
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
    listen(sockfd, 5);
    //select
    FD_ZERO(&rset);
    for(;;){
        FD_SET(sockfd, &rset);
        maxfdsize = max(sockfd, connfd);
        if ((nready = select(maxfdsize + 1, &rset, NULL, NULL, NULL)) < 0 ){
            perror("select");
        }
        if (FD_ISSET(sockfd, &rset)){ // new connection
            len = sizeof(cliaddr);
            connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
            for (i = 0; i < MAXCONN; i++){
                if(client[i] < 0) break;
            }
            client[i] = connfd;
        }
        for (i = 0; i < MAXCONN; i++){
            connfd = client[i];
            if(FD_ISSET(connfd, &rset)){
                if ((n = recvfrom(connfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len)) < 0){
                    perror("receive");
                    close(connfd);
                    continue;
                }
                printf("receive %s %d\n", buf, n);
                sendto(connfd, (void *)buf, n, 0, (struct sockaddr *)&cliaddr, len);
            }
        }
    }
    return 0;
}
