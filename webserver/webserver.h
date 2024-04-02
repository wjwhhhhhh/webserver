#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <vector>
#include "../utility/epoll/epoll.h"
#include "../http/httpconn.h"
#include "../utility/timer/heaptimer.h"
#include "../utility/pool/rallsqlconnpool.hpp"
#include "../utility/pool/threadpool.hpp"
#include "../utility/api/select.hpp"
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
        int on=1;
        setsockopt(listenfd.get(),SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
        listenfd.bind(ip,port);
        listenfd.listen(1024);
        epl.AddFd(listenfd.get(),listen_event);
        LOG_INFO("成功启动");
       SQL::SqlConnPool::Instance()->Init("localhost",3306,"root","qwer123456A","yourdb",10);
    }
    void DealPath();
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
    const int timeoutMS_=1000;
    const uint32_t listen_event = EPOLLRDHUP | EPOLLIN;
    const uint32_t conn_event = EPOLLRDHUP |EPOLLET | EPOLLONESHOT;
    HeapTimer timer_;
    std::map<int,HTTP::httpconn> value_;
};
}; // namespace WebServer
#endif