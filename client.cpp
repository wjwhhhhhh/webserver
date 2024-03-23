
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
std::string ip = "0.0.0.0";
int port = 1316;
int backprintf = 1024;
int main()
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("fail to open socket,%s", strerror(errno));
        return 1;
    }

    struct sockaddr_in address;
    ::memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    int id = ::connect(sockfd, (sockaddr *)&address, sizeof address);
    if (id == -1)
    {
        printf("fail to open socket,%s", strerror(errno));
    }

    std::string data = "hello world";
    ::send(sockfd, data.c_str(), data.size(), 0);

    char buf[1024] = {0};
    ::recv(sockfd, buf, sizeof buf, 0);
    printf("%s", buf);
    ::close(sockfd);
}