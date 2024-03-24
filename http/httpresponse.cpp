#include "httpresponse.h"
#include <fstream>
#include <string>
#include <sstream>
#include "../log/log.h"
using std::string;
using namespace WebServer::HTTP;
void httpresponse::SetBody(const string &val)
{
    body_=val;
}
void httpresponse::SetType(const string &val)
{
    type_=val;
}
string httpresponse::GetBody()
{
    return body_;
}
string httpresponse::GetType()
{
    return type_;
}
string httpresponse::work()
{
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: ";
    response <<type_<<"\t\n";
    response << "Content-Length: ";
    response << body_.size()<< "\r\n\r\n";
    response <<body_;
    return response.str();
}
string httpresponse::error_html()
{
    std::ifstream file("./WWW/error/403.html");
    if (!file.is_open()) {
        LOG_ERROR("403.html not found");
        exit(1);
    }
    std::stringstream response;
    response << "HTTP/1.1 403 Forbidden\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: ";

    file.seekg(0, std::ios::end);
    int fileLength = file.tellg();
    file.seekg(0, std::ios::beg);

    response << fileLength << "\r\n\r\n";
    response << file.rdbuf();

    return response.str();
}