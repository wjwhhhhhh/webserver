//
// Created by wjw on 2024/3/2.
//

#ifndef MyWebServer_BUFFER_H
#define MyWebServer_BUFFER_H
#include <atomic>
#include <sstream>
#include <string>
#include <vector>

namespace WebServer
{
namespace Buffer
{
class Buffer
{
  public:
    explicit Buffer();
    ~Buffer();
    void append(char &val);
    void append(const char *val);
    void append(std::string &val);
    void append(std::string &&val);
    std::string GetString();

  private:
    std::vector<char> buffer_;
};
} // namespace Buffer
} // namespace WebServer
#endif // MyWebServer_BUFFER_H
