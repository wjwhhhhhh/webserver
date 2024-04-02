#include "httpresponse.h"
#include "../utility/log/log.h"
#include <fstream>
#include <sstream>
#include <string>
using std::string;
using namespace WebServer::HTTP;
const std::unordered_map<string, string> httpresponse::SUFFIX_TYPE = {
    {"html", "text/html"},          {"xml", "text/xml"},          {"xhtml", "application/xhtml+xml"},
    {"txt", "text/plain"},          {"rtf", "application/rtf"},   {"pdf", "application/pdf"},
    {"word", "application/nsword"}, {"png", "image/png"},         {"gif", "image/gif"},
    {"jpg", "image/jpeg"},          {"jpeg", "image/jpeg"},       {"au", "audio/basic"},
    {"mpeg", "video/mpeg"},         {"mpg", "video/mpeg"},        {"avi", "video/x-msvideo"},
    {"gz", "application/x-gzip"},   {"tar", "application/x-tar"}, {"css", "text/css "},
    {"js", "text/javascript "},     {"ico", "image/x-icon"},{"json","application/json"}};
void httpresponse::SetBody(const string &val)
{
    body_ = val;
}
void httpresponse::SetType(const string &val)
{
    type_ = val;
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
    response << type_ << "\t\n";
    response << "Content-Length: ";
    response << body_.size() << "\r\n\r\n";
    response << body_;
    return response.str();
}
string httpresponse::error_html()
{
    std::ifstream file("./WWW/error/403.html");
    if (!file.is_open())
    {
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