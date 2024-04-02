#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include<unordered_map>
#include <unordered_set>
#include <string>
#include "../utility/buffer/buffer.h"
#include "../utility/log/log.h"
#include <regex>
using namespace WebServer::Buffer;
using std::string;
namespace WebServer::HTTP
{
    class httprequest
    {
    public:
    enum PARSE_STATE {//读取行的状态
        REQUEST_LINE,
        HEADERS,//
        BODY,
        FINISH,        
    };

        enum HTTP_CODE {//解析的结果
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };
        httprequest()
        {
            init();
        }
        ~httprequest()=default;
    void init();

    bool parse(WebServer::Buffer::Buffer& bur);
    bool ParseRequestLine(const string & line);
    bool ParseHeader(const string & line);
    string&GetBody();
    string&GetMethod();
    string&GetPath();
    void clear();
    private:
    PARSE_STATE state_;
    std::string method_, path_, version_, body_;
    std::unordered_map<std::string, std::string> header_;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;
    static int ConverHex(char ch);
    };

    
}
#endif