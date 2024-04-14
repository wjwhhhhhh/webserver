//
// Created by wjw on 2024/3/2.
//

#ifndef WEBSERVER_LOG_H
#define WEBSERVER_LOG_H

#include "blockqueue.hpp"
#include "AtomicQueue.h"
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#define ATOMIC_QUEUE_
namespace WebServer::Log
{
class log
{
  public:
    enum LogLevel
    {
        DEBUG = 0,
        INFO,
        WARN,
        ERROR
    };
    static log &GetLog();
    void Write(int level, const char *name, int line, const char *format, ...);
    static void FlushLogThread();
    static void SetAsync(bool temp);

  private:
#ifdef  ATOMIC_QUEUE_
WebServer::Atomic::AtomicQueue deque;
#else
    WebServer::BlockQueue::BlockQueue<std::string> deque;
#endif
    int start;
    void AsyncWrite();
    void flush();
    static std::string GetTime();
    log();
    virtual ~log();
    std::string FileName[2] = {"./logfile/0.log", "./logfile/1.log"};
    int FileNameId = 0;
    const int MaxLine = 5000;
    int line;
    static bool is_async_;
    std::fstream file;
    std::mutex Mutex, read_mutex_;

    std::unordered_map<int, std::string> GetLevel;
    std::unique_ptr<std::thread> WriterThread;
};

} // namespace WebServer::Log

#define LOG_BASE(level, __FILE__, __LINE__, format, ...)                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        WebServer::Log::log &Log = WebServer::Log::log::GetLog();                                                      \
        Log.Write(level, __FILE__, __LINE__, format, ##__VA_ARGS__);                                                   \
    } while (0);
#define LOG_DEBUG(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        LOG_BASE(0, __FILE__, __LINE__, format, ##__VA_ARGS__)                                                         \
    } while (0);
#define LOG_INFO(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        LOG_BASE(1, __FILE__, __LINE__, format, ##__VA_ARGS__)                                                         \
    } while (0);
#define LOG_WARN(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        LOG_BASE(2, __FILE__, __LINE__, format, ##__VA_ARGS__)                                                         \
    } while (0);
#define LOG_ERROR(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        LOG_BASE(3, __FILE__, __LINE__, format, ##__VA_ARGS__)                                                         \
    } while (0);
#endif // WEBSERVER_LOG_H
