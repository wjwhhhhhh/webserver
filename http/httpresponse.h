#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include<string>
#include<unordered_map>
using std::string;
namespace WebServer::HTTP
{
    class httpresponse
    {
        public:
        httpresponse()
        {
            body_="";
        }
        ~httpresponse()=default;
        void SetBody(const string &val);
        void SetType(const string &val);
        string GetBody();
        string GetType();
        string work();
        string error_html();
        static const std::unordered_map<std::string, std::string>SUFFIX_TYPE;
        private:
        string type_;
        string body_;

    };
};
#endif 