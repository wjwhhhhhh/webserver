#include"epoll.h"
using namespace WebServer::Epoll;
myepoll::myepoll(int size)
{
    epollfd_=epoll_create(size);
}

bool myepoll::AddFd(int fd, uint32_t events) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
}

bool myepoll::ModFd(int fd, uint32_t events) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
}

bool myepoll::DelFd(int fd) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0 == epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev);
}

int myepoll::Wait(int timeoutMs) {
    
    return epoll_wait(epollfd_, &events_[0], 1024, timeoutMs);
}
myepoll::~myepoll()
{
    ::close(epollfd_);
}