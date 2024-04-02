#ifndef MYSOCKET_H
#define MYSOCKET_H
#include "../log/log.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
namespace WebServer::Socket
{
    using std::string;
    class mysocket
    {
        public:
        mysocket();
        mysocket(const int & val);
        void bind(const string& ip,const int& port);
        void listen(const int& backlog);
        void conn(const string& ip,const int& port);
        int accept();
        string rev();
        size_t send(const string& val);
        void close();
        int get();
        private:
        int sockfd_;
    };
}
#endif 
