#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <strings.h>
#include <stdio.h>

#define TARGET_IP "192.168.1.14" 
#define TARGET_PORT 30099

int main(){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buf[1024] = "HELLO";
    int len, n, optval;
    // create random stocket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    // prepare server addr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, TARGET_IP, &servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(TARGET_PORT);
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))){
        perror("connect");
        return -1;
    }
    // send to server
    for(;;){
        if (sendto(sockfd, (void *)buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
            perror("sendto");
            return 1;
        }
        printf("send %s\n", buf);
        bzero(buf, 1024);
        sleep(1);
        if ((n = recvfrom(sockfd, (void *)buf, sizeof(buf), 0, NULL, NULL)) < 0){
            perror("recvfrom");
            return 1;
        }
        printf("recv %s\n", buf);
        sleep(1);
    }
    return 0;
}
