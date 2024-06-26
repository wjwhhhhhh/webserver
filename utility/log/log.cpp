#include "log.h"
#include <chrono>
#include <cstdarg>
#include <ctime>
#include <dirent.h>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
WebServer::Log::log &WebServer::Log::log::GetLog()
{
    static log lg;
    return lg;
}
using WebServer::Log::log;
void WebServer::Log::log::Write(int level, const char *name, int line, const char *format, ...)
{
    std::string te;
    {
        std::lock_guard<std::mutex> lock_(read_mutex_);
        te += GetLevel[level];
        te += " ";
        te += name;
        te += " ";
        te += move(std::to_string(line));
        te += " ";
        te += move(log::GetTime());
        te += " ";
        char temp[1024];
        va_list args;
        va_start(args, format);
        int m = vsnprintf(temp, 1024, format, args); //
        va_end(args);
        te += temp;
    }
    if (is_async_)
    {

        while (!deque.push_back(te))
        {
        }
    }
    else
    {
        file << te;
        flush();
    }
    return;
}

WebServer::Log::log::log()
{
    start = clock();
    GetLevel[0] = "DEBUG";
    GetLevel[1] = "INFO";
    GetLevel[2] = "WARN";
    GetLevel[3] = "ERROR";
    if (opendir("./logfile/") == nullptr)
    {
        int status = mkdir("./logfile/", S_IRWXO);

        if (status != 0)
        {
            exit(0);
        }
    }
    FileNameId = 0;
    line = 0;
    file.open(FileName[FileNameId].c_str(), std::ios_base::out | std::ios_base::trunc);
    assert(file.is_open());
    if (is_async_)
    {
        std::unique_ptr<std::thread> NewThread(new std::thread(FlushLogThread));
        WriterThread = move(NewThread);
    }
}

void WebServer::Log::log::AsyncWrite()
{
    std::string tem;
    while (this->deque.pop_front(tem))
    {
        {
            std::lock_guard<std::mutex> lock(Mutex);
            file << tem;
            flush();
        }
    }
    return;
}

void WebServer::Log::log::flush()
{
    file << '\n';
    file.flush();
    line++;
    if (line == MaxLine)
    {
        line = 0;
        FileNameId = (FileNameId + 1) % 2;
        file.close();
        file.open(FileName[FileNameId].c_str(), std::ios_base::out | std::ios_base::trunc);
        assert(file.is_open());
    }
    return;
}

std::string WebServer::Log::log::GetTime()
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();
    // 将时间转换为 time_t 对象
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    // 将 time_t 对象转换为本地时间的字符串表示
    std::string tem(std::ctime(&now_c));
    tem.pop_back();
    return tem;
}

WebServer::Log::log::~log()
{

    if (is_async_)
    {
        deque.close();
        WriterThread->join();
    }
    cout << clock() - start << endl;
}

void WebServer::Log::log::FlushLogThread()
{
    GetLog().AsyncWrite();
}

void WebServer::Log::log::SetAsync(bool temp)
{
    is_async_ = temp;
}
bool WebServer::Log::log::is_async_ = true;