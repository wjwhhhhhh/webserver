#ifndef SELECT_HPP
#define SELECT_HPP
#include "../log/log.h"
#include "../pool/rallsqlconnpool.hpp"
#include <string>
namespace WebServer::SQL
{
class selecter
{
  public:
    bool FindOk(MYSQL *tem, const std::string &a, const std::string &b)
    {
        std::string query = "SELECT password FROM user where username=";
        query += a;
        if (mysql_query(tem, query.c_str()))
        {
            LOG_ERROR("查询信息失败,查询的语句是%s", query.c_str());
            return false;
        }
        MYSQL_RES *res;
        MYSQL_ROW row;
        res = mysql_use_result(tem);

        if (res == NULL)
        {
            LOG_ERROR("mysql_use_result() failed\n");
            return false;
        }
        bool fl=false;
        while ((row = mysql_fetch_row(res)) != NULL)
        {
            std::string tem=std::string(row[0]);
            tem ="\""+tem+"\"";
            if(tem==b)fl=true;
        }
        LOG_INFO("查询结果为%d,语句为%s",fl,query.c_str());
        return fl;
    }
    static selecter &Instance()
    {
        static selecter a;
        return a;
    }
  private:
    selecter()
    {

    }
    ~selecter()
    {

    }
    
};
}; // namespace WebServer::SQL
#endif