//
// Created by wjw on 2024/3/2.
//
#include "buffer.h"
#include <algorithm>
#include <cassert>
#include <iostream>
using std::string;
WebServer::Buffer::Buffer::Buffer()
{
}

WebServer::Buffer::Buffer::~Buffer()
{
}

void WebServer::Buffer::Buffer::append(char &val)
{
    buffer_.push_back(val);
    return;
}

void WebServer::Buffer::Buffer::append(const char *val)
{
    std::string tem(val);
    append(tem);
    return;
}

void WebServer::Buffer::Buffer::append(std::string &val)
{
    for (auto &c : val)
        append(c);
}

std::string WebServer::Buffer::Buffer::GetString()
{
    string ans = "";
    while (buffer_.size())
    {
        ans += buffer_.back();
        buffer_.pop_back();
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}

void WebServer::Buffer::Buffer::append(string &&val)
{
    for (auto &c : val)
        append(c);
    return;
}
void WebServer::Buffer::Buffer::Peek()
{
    buffer_.pop_front();
}
char& WebServer::Buffer::Buffer::Get()
{
    return buffer_.front();
}
char& WebServer::Buffer::Buffer::operator[](int x)
{
    if(buffer_.size()<x)exit(1);
    else return buffer_[x];
}
bool WebServer::Buffer::Buffer::empty()
{
    return buffer_.empty();
}
size_t WebServer::Buffer::Buffer::size()
{
    return buffer_.size();
}
