
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
using namespace std;
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

    std::string data = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.9\r\nConnection:keep-alive\r\n\r\n";
    cout<<data.size()<<endl;
    int si=::send(sockfd, data.data(), data.size(), 0);
    cout<<si<<endl;
    char buf[1024] = {0};
    ::recv(sockfd, buf, sizeof buf, 0);
    printf("%s", buf);
    ::close(sockfd);
    cout<<"jieshu"<<endl;
}