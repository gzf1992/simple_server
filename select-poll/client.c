#include<sys/select.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>

#define MAXLINE (8096)
#define err_quit perror

inline max(int a, int b){if (a>b) return a; else return b;}

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
    size_t n;
    const char *ptr;

    n = num;
    ptr = buf;
    // 循环写
    while (n > 0) {
    /* 开始写*/
        if ((res = write(fd, ptr, n)) == -1) {
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

void
str_cli(FILE *fp, int sockfd)
{
    int maxfdp1, stdineof;
    fd_set rset;
    char buf[MAXLINE];
    int n;

    stdineof = 0;
    FD_ZERO(&rset);
    for(;;){
        if (stdineof == 0){
            FD_SET(fileno(fp), &rset);
        }
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        if(select(maxfdp1, &rset, NULL, NULL, NULL) < 0){
            perror("select");
            return;
        }
        // 套接字响应
        if (FD_ISSET(sockfd, &rset)){
            if ((n = Read(sockfd, buf, MAXLINE)) == 0){
                if (stdineof == 1)
                    return;
                else
                    err_quit("str_cli, server terminated prematurely");
            }
            Write(fileno(stdout), buf, n);
        }

        // 标准输入响应
        if (FD_ISSET(fileno(fp), &rset)){
            if (n = Read(fileno(fp), buf, MAXLINE) == 0){
                stdineof = 1;                   // 结束标准输入
                if(shutdown(sockfd, SHUT_WR) < 0){
                    perror("shutdown");
                    return;
                }
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            Write(sockfd, buf, n);
        }
    }
}

int main(){
    int client_sockfd;
    struct sockaddr_in remote_addr;

    if((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return 1;
    }

    bzero(&remote_addr, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(18888);
    if(connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
    {
        perror("connect");
        return 1;
    }
    printf("connected to server\n");
    str_cli(stdin, client_sockfd);

    return 0;
}
