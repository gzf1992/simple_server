#include<sys/select.h>
#include<sys/time.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>

#define MAXLINE 8096

ssize_t Read(int fd, void *buf, size_t num)
{
    ssize_t res;
    size_t n;
    char *ptr;

    n = num;
    ptr = buf;
    while (n > 0) {
        if ((res = read (fd, ptr, n)) == -1) {
            if (errno == EINTR)
                res = 0;
            else
                return (-1);
        }
        else if (res == 0)
            break;
        ptr += res;
        n -= res;
    }
    return (num - n);
}

ssize_t Write(int fd, const void *buf, size_t num){
    ssize_t res;
    size_t n = num;
    const char *ptr = buf;

    while (n > 0) {
        //阻塞
        if ((res = write(fd, ptr, n)) <= 0) {
            if (errno == EINTR)
                res = 0;
            else
                return (-1);
        }
        ptr += res;/* 从剩下的地方继续写     */
        n -= res;
    }

    return (num);
}

void err_quit(char *buf){
    perror(buf);
    exit(-1);
}

int main(){
    // setup
    struct sockaddr_in my_addr, cliaddr;
    int maxfd, connfd, sockfd, listenfd;
    int i, maxi, clilen, nready;
    int client[FD_SETSIZE];
    char buf[MAXLINE];
    size_t n;
    fd_set allset;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return 1;
    }
    bzero(&my_addr, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;   //all
    my_addr.sin_port = htons(18888);
    if (bind(listenfd, (struct sockaddr *)&my_addr, sizeof(my_addr))<0){
        perror("bind");
        return 1;
    }
    listen(listenfd, 10);

    // Initialize connections
    maxfd = listenfd;
    maxi = -1;
    for(i = 0; i < FD_SETSIZE; i++){
        client[i] = -1;
    }
    // initialize fd_set
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    for(;;){
        // blocking here
        if((nready = select(maxfd + 1, &allset, NULL, NULL, NULL) < 0){
            perror("select");
            return 1;
        };

        if(FD_ISSET(listenfd, &allset)){ /* new connection */
            clilen = sizeof(cliaddr);
            if (connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen) < 0){
                perror("accept");
                return 1;
            }
            // update client array
            for (i=0; i<FD_SETSIZE; i++){
                if (client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE)
                err_quit("too mant clients");
            // set fd set
            FD_SET(connfd, &allset);
            // update max descriptor for select
            if (connfd > maxfd)
                maxfd = connfd;
            if(i > maxi) 
                maxi = i;
            // if no ready
            if (--nready <= 0)
                continue;
        }
        for (i=0; i <= maxi; i++){    /*check all for data*/
            // closed connection
            if ((sockfd = client[i])<0)
                continue;
            if (FD_ISSET(sockfd, &allset)){
                if ((n = Read(sockfd, buf, MAXLINE)) == 0){ /*connection closed by client*/
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else {
                    Write(sockfd, buf, n);
                }
                // if no ready
                if (--nready <= 0)
                    break;
            }
        }
    }
}

