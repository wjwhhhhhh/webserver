#include <iostream>
#include "httprequest.h"

using namespace std;
using namespace WebServer::HTTP;
using namespace WebServer::Buffer;

const unordered_set<string> httprequest::DEFAULT_HTML{
    "/index", "/register", "/login", "/welcome", "/video", "/picture",
};

const unordered_map<string, int> httprequest::DEFAULT_HTML_TAG{
    {"/register.html", 0},
    {"/login.html", 1},
};

void httprequest::init()
{
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
}
bool httprequest::parse(WebServer::Buffer::Buffer &bur)
{
    string line;
    while (!bur.empty())
    {
        if (bur.size() > 1 && bur[0] == '\r' && bur[1] == '\n')
        {
            switch (state_)
            {
            case REQUEST_LINE:
                if (!ParseRequestLine(line))
                {
                    return false;
                }
                break;
            case HEADERS:
                if (!ParseHeader(line))
                {
                    return false;
                }
                break;
            default:
                break;
            }
            bur.Peek();
            bur.Peek();
            line="";
        }
        else
        {
            line += bur.Get();
            bur.Peek();
        }
    }
    
    if (state_ == FINISH)
    {

        if (method_ == "Post" && header_.find("Content-Length") != header_.end())
        {
            body_ = line.substr(0, stoi(header_["Content-Length"]));
        }
        return true;
    }
    return false;
}
bool httprequest::ParseRequestLine(const string &line)
{
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if (regex_match(line, subMatch, patten))
    {
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    LOG_ERROR("RequestLine Error");
    return false;
}
bool httprequest::ParseHeader(const string &line)
{
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if (regex_match(line, subMatch, patten))
    {
        header_[subMatch[1]] = subMatch[2];
    }
    else
    {
        state_ = FINISH;
    }
    return true;
}
string &httprequest::GetBody()
{
    return body_;
}
string &httprequest::GetMethod()
{
    return method_;
}
string&httprequest::GetPath()
{
    return path_;
}