#ifndef MYEPOLL_H
#define MYEPOLL_H
#include "../log/log.h"
#include <sys/epoll.h>
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h> 
namespace WebServer::Epoll
{
    using std::string;
    class myepoll
    {
        public:
        myepoll(int sized=1);
        ~myepoll();
        bool AddFd(int fd, uint32_t events);
        bool ModFd(int fd, uint32_t events);
        bool DelFd(int fd);
        int Wait(int timeoutMs = -1);
        epoll_event events_[1024];
        private:
        int epollfd_;
    };
};
#endif 
