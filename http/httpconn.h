#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include <unordered_map>
#include "../utility/socket/socket.h"
#include "./httprequest.h"
#include "./httpresponse.h"
#include "../utility/buffer/buffer.h"
#include "../utility/log/log.h"
namespace WebServer::HTTP
{
    class httpconn: public WebServer::Socket::mysocket
    {
        public:
        httpconn()
        {
            
        }
        httpconn(int sockfd):WebServer::Socket::mysocket(sockfd)
        {
            buf.clear();
        }
        httprequest request_;
        httpresponse response_;
        WebServer::Buffer::Buffer buf;
    };
};
#endif 
