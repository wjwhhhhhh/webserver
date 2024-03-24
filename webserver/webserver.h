#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <vector>
#include "../epoll/epoll.h"
#include "../http/httpconn.h"
#include "../timer/heaptimer.h"
#include "../pool/rallsqlconnpool.hpp"
#include "../pool/threadpool.hpp"
namespace WebServer
{
using namespace Socket;
using namespace Epoll;
using namespace time;
using namespace ThreadPool;
class webserver
{
  public:
    webserver():thread_pool_(512)
    {
        listenfd.bind(ip,port);
        listenfd.listen(1024);
        epl.AddFd(listenfd.get(),listen_event);
        LOG_INFO("success");
    }
    ~webserver()
    {
        for(auto &[x,y]:value_)
        {
            y.close();
        }
    }
    void start();
    void DealListen();
    void DealRead(int fd);
    void DealWrite(int fd);
    int setnonblocking(int fd);
    void Closefd(int fd);
    void OnRead(int &fd);
    void OnWrite(int &fd);
    private:
    threadpool thread_pool_;
    mysocket listenfd;//建立监听的socket;
    myepoll epl;
    const string ip = "0.0.0.0";
    const int port = 1316;
    const int timeoutMS_=300;
    const uint32_t listen_event = EPOLLRDHUP | EPOLLIN;
    const uint32_t conn_event = EPOLLRDHUP |EPOLLET | EPOLLONESHOT;
    HeapTimer timer_;
    std::map<int,HTTP::httpconn> value_;
};
}; // namespace WebServer
#endif