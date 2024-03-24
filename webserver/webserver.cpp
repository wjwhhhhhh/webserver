#include "webserver.h"
using namespace WebServer;

void webserver::start()
{
    int timeMS = -1;
    LOG_INFO("========== Server start ==========");
    while (1)
    {
        if (timeoutMS_ > 0)
        {
            timeMS = timer_.GetNextTick();
        }
        int size = epl.Wait(timeMS);
        for (int i = 0; i < size; i++)
        {
            int sockfd = epl.events_[i].data.fd;
            uint32_t events = epl.events_[i].events;
            if (sockfd == listenfd.get())
            {
                DealListen();
            }
            else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                Closefd(sockfd);
            }
            else if (events & EPOLLIN)
            {
                DealRead(sockfd);
            }
            else if (events & EPOLLOUT)
            {

                DealWrite(sockfd);
            }
            else
            {
                LOG_ERROR("Unexpected event");
            }
        }
    }
}
int webserver::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
void webserver::DealListen()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = listenfd.accept();
    HTTP::httpconn temp(fd);
    value_.insert({fd, HTTP::httpconn(fd)});
    LOG_INFO("socket号为%d想要连接",fd);
    if (timeoutMS_ > 0)
    {
        timer_.add(fd, timeoutMS_, std::bind(&webserver::Closefd, this, fd));
    }
    epl.AddFd(fd, EPOLLIN | conn_event);
    setnonblocking(fd);
}
void webserver::OnRead(int &fd)
{

    char temp[1024];
    memset(temp, 0, sizeof temp);
    auto &connfd = value_[fd];
        connfd.buf.clear();
    if (conn_event & EPOLLET)
    {
        LOG_ERROR("在读了 %d",fd);
        while (1)
        {
            memset(temp, 0, sizeof temp);
            auto ret = ::recv(fd, temp, sizeof temp, 0);
            LOG_ERROR("读的大小为%zd",ret);
            if (ret == 0)
            {
                LOG_ERROR("read error");
                Closefd(fd);
                return ;
            }
            else if (ret <0)
            {
                if(errno==EAGAIN)
                {
                    break;
                }
            }
            else
            {
                LOG_INFO("%s被插入",temp);
                for (int i = 0; i < static_cast<int>(ret); i++)
                    connfd.buf.append(temp[i]);
            }
        }
    }
    if(connfd.buf.empty())
    {
        epl.ModFd(fd, EPOLLIN | conn_event);
        return ;
    }
    if (connfd.request_.parse(connfd.buf))
    {
        connfd.buf.clear();
        string path = "/root/webserver/build/WWW";
        path += connfd.request_.GetPath();
        connfd.response_.SetType("text/html");
        std::ifstream file(path);
        if (!file.is_open())
        {

            connfd.buf.append(connfd.response_.error_html());
                epl.ModFd(fd, EPOLLOUT | conn_event);
            return;
        }
        std::stringstream response;
        response << file.rdbuf();
        connfd.response_.SetBody(response.str());
        connfd.buf.append(connfd.response_.work());
        epl.ModFd(fd, EPOLLOUT | conn_event);
    }
    else
    {

        connfd.buf.append(connfd.response_.error_html());
         epl.ModFd(fd, EPOLLOUT | conn_event);
    }
}
void webserver::OnWrite(int &fd)
{
    char temp[1024];
    LOG_ERROR("%d 正在写了",fd)
    auto &connfd = value_[fd];
    int size = 0;
    auto tem=connfd.buf;
    LOG_INFO("%s",tem.GetString().c_str());
    while (!connfd.buf.empty())
    {
        while (!connfd.buf.empty() && size < 1024)
        {
            temp[size++] = connfd.buf.Get();
            connfd.buf.Peek();
        }
        int fr = 0;
        LOG_INFO("正在发送");
        while (fr < size)
        {
            int tr = ::send(fd, temp + fr, size - fr, 0);
            fr += tr;
            if (tr == 0)
            {
                LOG_ERROR("send error");
                exit(1);
            }
        }
    }
    epl.ModFd(fd, EPOLLIN | conn_event);
}
void webserver::DealRead(int fd)
{
    LOG_INFO("%d 可以读了",fd)
    if (timeoutMS_ > 0)
    {
        timer_.adjust(fd, timeoutMS_);
    }
    auto memberFunction = std::bind(&WebServer::webserver::OnRead, this, std::placeholders::_1);
    thread_pool_.enqueue(memberFunction, fd);

}
void webserver::DealWrite(int fd)
{
    LOG_INFO("%d 可以写了",fd)
    if (timeoutMS_ > 0)
    {
        timer_.adjust(fd, timeoutMS_);
    }
    auto memberFunction = std::bind(&WebServer::webserver::OnWrite, this, std::placeholders::_1);
    thread_pool_.enqueue(memberFunction, fd);

}
void webserver::Closefd(int fd)
{
    value_[fd].close();
    epl.DelFd(fd);
    value_.erase(fd);
    return;
}
