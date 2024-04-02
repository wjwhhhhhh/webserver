#include "socket.h"
using namespace WebServer::Socket;
using std::string;
mysocket::mysocket()
{
    sockfd_ = ::socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd_==-1)
    {
        LOG_ERROR("生成mysocket失败,%d,%s",strerror(errno));
        exit(1);
    }
}

mysocket::mysocket(const int& val)
{
    this->sockfd_=val;
}

void mysocket::bind(const string& ip,const int& port)
{
    struct sockaddr_in address;
    ::memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    if (::bind(this->sockfd_, (struct sockaddr *)&address, sizeof address) == -1)
    {
        LOG_ERROR("fail%d, %s", errno, strerror(errno));
        exit(1);
    }
}
void mysocket::listen(const int &backlog)
{
    if (::listen(this->sockfd_, backlog)==-1)
    {
        LOG_ERROR("listen%d,because %s", errno, strerror(errno));
        exit(1);
    }
}
void mysocket::conn(const string &ip,const int& port)
{
    struct sockaddr_in address;
    ::memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    int id = ::connect(this->sockfd_, (sockaddr *)&address, sizeof address);
    if (id == -1)
    {
        LOG_ERROR("fail to open mysocket,%s", strerror(errno));
        exit(1);
    }
}

string mysocket::rev()
{
    char buf[1024]={0};
    size_t len = ::recv(sockfd_, buf, sizeof buf, 0);
    if(len==-1)
    {
        LOG_ERROR("fail to rev,%s", strerror(errno));
        exit(1);
    }
    return string(buf);
}
int mysocket::accept()
{
    int id=::accept(sockfd_,nullptr,nullptr);
    if(id==-1)
    {
        LOG_ERROR("开启accept失败,%s", strerror(errno));
        exit(1);
    }
    return id;
}
size_t mysocket::send(const string & val)
{
    size_t a = ::send(sockfd_, val.c_str(), val.size(), 0);
        if (a < 0)
        {
            LOG_ERROR("send error");
            exit(1);
        }
        return a;
}
void mysocket::close()
{
    LOG_INFO("socket号%d 已经被关闭了", sockfd_);
    ::close(sockfd_);
}

int  mysocket::get()
{
    return this->sockfd_;
}