#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <strings.h>
#include <stdio.h>

#define TARGET_IP "192.168.1.14" 
#define TARGET_PORT 30088

int main(){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buf[1024] = "HELLO";
    int len, n, optval;
    // create random stocket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval,sizeof(int));
    // prepare server addr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, TARGET_IP, &servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(TARGET_PORT);
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
    
    
    
    // recv
    
    
}
