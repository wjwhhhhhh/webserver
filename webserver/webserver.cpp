#include "webserver.h"
#include "../utility/json/json.h"
#include "../utility/json/writer.h"
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
    LOG_INFO("socket号为%d想要进行连接", fd);
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
    LOG_INFO("socket号为%d正在进行读入", fd);
    if (conn_event & EPOLLET)
    {
        while (1)
        {
            memset(temp, 0, sizeof temp);
            auto ret = ::recv(fd, temp, sizeof temp, 0);
            if (ret == 0)
            {
                LOG_ERROR("read error");
                Closefd(fd);
                return;
            }
            else if (ret < 0)
            {
                if (errno == EAGAIN)
                {
                    break;
                }
            }
            else
            {
                for (int i = 0; i < static_cast<int>(ret); i++)
                    connfd.buf.append(temp[i]);
            }
        }
    }
    LOG_INFO("socket号为%d读入结束,读入了%d", fd, connfd.buf.size());
    if (connfd.buf.empty())
    {
        LOG_ERROR("socket号为%d读入为空", fd);
        epl.ModFd(fd, EPOLLIN | conn_event);
        return;
    }
    connfd.request_.clear();
    if (connfd.request_.parse(connfd.buf))
    {
        connfd.buf.clear();
        LOG_INFO("socket号为%d,解析成功,%s", fd,connfd.request_.GetMethod().c_str());
        if (connfd.request_.GetMethod() == "GET")
        {
            string path = "/root/webserver/build/WWW";
            path += connfd.request_.GetPath();
            std::regex pattern("\\.(\\w+)$");
            std::smatch match;
            if (std::regex_search(path, match, pattern))
            {
                std::string a = match[1];
                LOG_INFO("最后匹配的%s", a.c_str());
                auto it = HTTP::httpresponse::SUFFIX_TYPE.find(a);
                if (it == HTTP::httpresponse::SUFFIX_TYPE.end())
                {
                    LOG_ERROR("socket号为%d,请求路径的为%s,正则匹配的后缀失败", fd, path.c_str());
                    connfd.buf.append(connfd.response_.error_html());
                    epl.ModFd(fd, EPOLLOUT | conn_event);
                    return;
                }
                connfd.response_.SetType(it->second);
            }
            else
            {
                LOG_ERROR("socket号为%d,请求路径为%s不存在后缀名", fd, path.c_str());
                connfd.buf.append(connfd.response_.error_html());
                epl.ModFd(fd, EPOLLOUT | conn_event);
                return;
            }
            std::ifstream file(path);
            if (!file.is_open())
            {
                LOG_ERROR("socket号为%d,请求路径为%s不存在", fd, path.c_str());
                connfd.buf.append(connfd.response_.error_html());
                epl.ModFd(fd, EPOLLOUT | conn_event);
                return;
            }
            std::stringstream response;
            response << file.rdbuf();
            connfd.response_.SetBody(response.str());
            connfd.buf.append(connfd.response_.work());
            epl.ModFd(fd, EPOLLOUT | conn_event);
            LOG_ERROR("socket号为%d,请求路径为%s已经成功放到缓存区中", fd, path.c_str());
            return;
        }
        if (connfd.request_.GetPath() != "/api/login")
        {
            connfd.buf.clear();
            LOG_ERROR("socket号为%d路径错误", fd);
            connfd.buf.append(connfd.response_.error_html());
            epl.ModFd(fd, EPOLLOUT | conn_event);
            return;
        }
        {
            LOG_INFO("请求体中的body是%s", connfd.request_.GetBody().c_str());
            using namespace Eula::json;
            auto json_ = parse(connfd.request_.GetBody());
            MYSQL *raii;
            SQL::SqlConnRAII p(&raii, SQL::SqlConnPool::Instance());
            bool res = SQL::selecter::Instance().FindOk(raii, json_["username"].str(), json_["password"].str());
            if (res)
            {
                connfd.response_.SetType("application/json");
                connfd.response_.SetBody("{\"status\":200,\"username\":\"WjW\"}");
                connfd.buf.append(connfd.response_.work());
                epl.ModFd(fd, EPOLLOUT | conn_event);
                LOG_ERROR("socket号为%d,已经成功放到缓存区中", fd);
                return;
            }
            else
            {
                connfd.buf.clear();
                LOG_ERROR("socket号为%d解析失败", fd);
                connfd.buf.append(connfd.response_.error_html());
                epl.ModFd(fd, EPOLLOUT | conn_event);
                return;
            }
        }
    }
    else
    {
        connfd.buf.clear();
        LOG_ERROR("socket号为%d解析失败", fd);
        connfd.buf.append(connfd.response_.error_html());
        epl.ModFd(fd, EPOLLOUT | conn_event);
        return;
    }
}
void webserver::OnWrite(int &fd)
{
    LOG_ERROR("socket号%d 正在写了", fd);
    auto &connfd = value_[fd];

    string a = connfd.buf.GetString();
    int size = a.size();
    const char *ctr = a.data();
    long long sendBufferSize = 100ll * 1024 * 1024;
    if (size > 10000)
    {
        if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sendBufferSize, sizeof(sendBufferSize)) == -1)
        {
            LOG_ERROR("调整失败,%s", strerror(errno));
        }
    }
    LOG_ERROR("socket编号为%d,要发送的大小为", fd, size);
    while (size > 0)
    {
        int hassend = ::send(fd, ctr, size, 0);

        if (hassend < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 资源暂时不可用，稍等一段时间后重试发送
                usleep(10000); // 等待10毫秒
                continue;
            }
            LOG_ERROR("socket编号为%d,发送失败%s", fd, strerror(errno));
            return;
        }
        ctr += hassend;
        size -= hassend;
        LOG_ERROR("socket编号为%d,发送了%d,还要发送%d", fd, hassend, size);
    }
    epl.ModFd(fd, EPOLLIN | conn_event);
    LOG_INFO("socket号%d 已经写完了", fd);
}
void webserver::DealRead(int fd)
{
    LOG_INFO("%d 可以读了", fd)
    if (timeoutMS_ > 0)
    {
        timer_.adjust(fd, timeoutMS_);
    }
    auto memberFunction = std::bind(&WebServer::webserver::OnRead, this, std::placeholders::_1);
    thread_pool_.enqueue(memberFunction, fd);
}
void webserver::DealWrite(int fd)
{
    LOG_INFO("socket号为%d的可以写了", fd)
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
