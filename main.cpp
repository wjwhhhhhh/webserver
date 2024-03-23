#include"socket/socker.h"
#include "epoll/epoll.h"
using  namespace WebServer::Socket;
using  namespace WebServer::Epoll;
using std::cout;
 string buf= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 46\r\n\r\n<html><body><h1>Hello,World !</h1></ body> </html>";
int main()
{
    mysocket  listen_fd;
    listen_fd.bind("0.0.0.0",1316);
    listen_fd.listen(1024);
    myepoll epl;
    uint32_t ev=0;
    ev|=EPOLLIN;
    epl.AddFd(listen_fd.get(),ev);
    while(1)
    {
        int si=epl.Wait();
        if(si<0)
        {
            LOG_ERROR("epoll 出现错误");
            exit(1);
        }
        for(int i=0;i<si;i++)
        {
            auto sock_fd=epl.events_[i].data.fd;
            if(sock_fd==listen_fd.get())
            {
                mysocket conn_fd=listen_fd.accept();
            }
        }        
    }
    return 0;
}
