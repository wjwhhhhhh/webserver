//
// Created by wjw on 2023/12/13.
//
#include "json.h"
#include <stdexcept>
#include <iostream>
#include <functional>
#include <queue>
#include <stack>
#include <sstream>
#include <vector>
#include <functional>
using namespace Eula::json;
using namespace std;
Json::Json():m_type(json_null)
{

}

Json::Json(const bool& value):m_type(json_bool)
{
    m_value.m_bool = value;
}

Json::Json (const int &value):m_type(json_int)
{
    m_value.m_int = value;
}
Json::Json(const double &value):m_type(json_double)
{
    m_value.m_double = value;
}
Json::Json(const char*value):m_type(json_string)
{
    m_value.m_string = new std::string(value);
}
Json::Json(const std::string &value):m_type(json_string)
{
    m_value.m_string = new std::string(value);
}

Json::Json (const Type type):m_type(type)
{
    switch (type)
    {
    case json_null:
        break;
    case json_bool:
        m_value.m_bool = false;
        break;
    case json_int:
        m_value.m_int = 0;
        break;
    case json_double:
        m_value.m_double = 0.0;
        break;
    case json_string:
        m_value.m_string = new std::string("");
        break;
        case json_object:
            m_value.m_object=new std::map<std::string ,Json>;
            break;
        case json_array:
            m_value.m_array=new std::vector<Json> ;
            break;
        default:break;
    }
}
Json::Json(const Json &other)
{
    *this = other;
}

Json::operator bool() const
{
    if(m_type!=json_bool)
    {
        throw std::logic_error("not bool");
    }
    return m_value.m_bool;
}

Json::operator std::string() const
{
    if(m_type!=json_string)
    {
        throw std::logic_error("not string");
    }
    return *(m_value.m_string);
}

Json::operator int() const
{
    if(m_type!=json_int)
    {
        throw std::logic_error("not int");
    }
    return m_value.m_int;
}
Json::operator double() const
{
    if(m_type!=json_double)
    {
        throw std::logic_error("not double");
    }
    return m_value.m_double;
}


Json &Json:: operator[](int index)
{
    if(m_type!=json_array)
    {
        m_type = json_array;
        m_value.m_array = new std::vector<Json>();
    }
    if(index<0)
    {
        throw std::logic_error("bad index");
    }
    while(index>=(m_value.m_array)->size())
    {
        (m_value.m_array)->push_back(Json());
    }
    return (m_value.m_array)->at(index);
}

Json& Json:: operator = (const Json& other)
{
    clear();
    m_type =other.m_type;
    switch (m_type)
    {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool = other.m_value.m_bool;
            break;
        case json_int:
            m_value.m_int = other.m_value.m_int;
            break;
        case json_double:
            m_value.m_double = other.m_value.m_double;
            break;
        case json_string:
            m_value.m_string = new std::string(*(other.m_value.m_string));
            break;
        case json_object:
            m_value.m_object=new std::map<std::string,Json>(*(other.m_value.m_object));
            break;
        case json_array:
            m_value.m_array=new std::vector<Json>(*(other.m_value.m_array));
            break;
        default:break;
    }

}

void Json::append(const Json&other)
{
    if(m_type!=json_array)
    {
        m_type = json_array;
        m_value.m_array = new std::vector<Json>();
    }
    (m_value.m_array)->push_back(other);
}

std::string Json::str()const
{
    std::string ans;
    switch (m_type)
    {
        case json_null:
            ans+="null";
            break;
        case json_bool:
            ans+=(m_value.m_bool?"true":"false");
            break;
        case json_int:
            ans+= std::to_string(m_value.m_int);
            break;
        case json_double:
            ans+=std::to_string(m_value.m_double);
            break;
        case json_string:
            ans+='\"'+*m_value.m_string+'\"';
            break;
        case json_object:
        {
            ans+="{";
            for(auto &[x,cnt]:*m_value.m_object)
            {
                ans+='\"'+x+'\"'+":";
                ans+=cnt.str();
                ans+=",";
            }
            if(ans.back()==',')ans.pop_back();
            ans.push_back('}');
        }
            break;
        case json_array:
            ans+="[";
            for(auto &x:*m_value.m_array)
            {
                ans+=x.str();
                ans+=",";
            }
            if(ans.back()==',')ans.pop_back();
            ans.push_back(']');
            break;
        default:break;
    }
    return ans;
}

Json &Json:: operator[](const char * key)
{
    return (*this)[std::string(key)];
}
Json & Json::operator[](const std::string &key)
{
    if(m_type!=json_object)
    {
        m_type = json_object;
        m_value.m_object = new std::map<std::string,Json>();
    }
    if((m_value.m_object)->find(key)==(m_value.m_object)->end())
    {
        (m_value.m_object)->insert({key,Json()});
    }
    return (*m_value.m_object)[key];
}
void Json::clear()
{
    switch (m_type)
    {
        case json_null:
            break;
        case json_bool:
            break;
        case json_int:
            break;
        case json_double:
            break;
        case json_string:
            delete(m_value.m_string);
            break;
        case json_object:
            m_value.m_object->clear();
            delete(m_value.m_object);
            break;
        case json_array:
            m_value.m_array->clear();
            delete(m_value.m_array);
            break;
        default:break;
    }
    m_type=json_null;
}
Json::~Json() {
    clear();
}

bool Json::IsObject() const {
    return m_type==json_object;
}

Json Eula::json::parse(const char *value)
{
    std::string temp(value);
    return parse(temp);
}
Json Eula::json::parse(const std::string &value) {
    int id=0;
    return parse(value,id);
}
Json Eula::json::parse(const std::string &value,int &id) {
    Json ans;
        while(id<value.size()&&value[id]==' '||value[id]=='\t'||value[id]=='\n'||value[id]==',')
        {
            id++;
        }
        if(id>=value.size())exit(1);
        if(value[id]=='n')ans=parse_null(value,id);
        else if(value[id]=='t'||value[id]=='f')ans=parse_bool(value,id);
        else if(value[id]=='{')ans=parse_object(value,id);
        else if(value[id]=='[')ans=parse_array(value,id);
        else if(value[id]=='"')ans=parse_string(value,id);
        else ans=parse_number(value,id);
        return ans;
}
Json Eula::json::parse_null(const std::string &value,int &id)
{
    std::string tem;
    Json ans;
    int size=0;
    while(id<value.size()&&tem.size()!=4)
    {
        if(value[id]==' '||value[id]=='\t'||value[id]=='\n')
        {
        }
        else tem+=value[id];
            id++;
    }
    if(tem!="null")exit(5);
    return ans;
}
Json Eula::json::parse_bool(const std::string &value,int &id)
{
    if(value[id]=='t')
    {
        std::string tem;

        int size=0;
        while(id<value.size()&&tem.size()!=4)
        {
            if(value[id]==' '||value[id]=='\t'||value[id]=='\n')
            {
            }
            else tem+=value[id];
            id++;
        }
        if(tem!="true")exit(3);
        return Json(true);
    }
    else
    {
        std::string tem;
        int size=0;
        while(id<value.size()&&tem.size()!=4)
        {
            if(value[id]==' '||value[id]=='\t'||value[id]=='\n')
            {
            }
            else tem+=value[id];
            id++;
        }
        if(tem!="false")exit(4);
        return Json(false);
    }
}
Json Eula::json::parse_number(const std::string &value,int &id)
{
    std::string tem;
    std::function<bool(char)> is_num=[](char c)
    {
        if(c>='0'&&c<='9')return true;
        else return false;
    };
    while(id<value.size()&&value[id]!=','&&value[id]!='}'&&value[id]!=']')
    {
        if(value[id]==' '||value[id]=='\t'||value[id]=='\n')id++;
        else if(value[id]=='+'||value[id]=='e'||value[id]=='.'||value[id]=='-'||is_num(value[id]))tem+=value[id++];
        else exit(2);
    }
    if(count(tem.begin(),tem.end(),'.')||count(tem.begin(),tem.end(),'e'))
    {
        return Json(static_cast<double>(stold(tem)));
    }
    else return Json(stoi(tem));
}
Json Eula::json::parse_array(const std::string &value,int &id)
{
    Json ans(Json::Type::json_array);
    id++;
    while(id<value.size())
    {
        if(value[id]==']')
        {
            id++;
            break;
        }
        else if(value[id]==','||value[id]==' '||value[id]=='\t',value[id]=='\n')id++;
        else ans.append(parse(value,id));
    }
    return ans;
}
Json Eula::json::parse_string(const std::string &value,int &id)
{
    std::string tem;
    id++;
    while(id<value.size())
    {
        if(value[id]=='"')
        {
            id++;
            break;
        }
        else
        {
            tem+=value[id];
            id++;
        }
    }
    return Json(tem);
}
Json Eula::json::parse_object(const std::string &value,int &id)
{
    Json ans(Json::Type::json_object);
    id++;
    while(id<value.size()&&(value[id]==','||value[id]==' '||value[id]=='\t'||value[id]=='\n'))id++;
    if(value[id]=='}')
    {
        id++;
        return ans;
    }
    while(id<value.size())
    {
        Json temp=parse_string(value,id);
        while(id<value.size()&&(value[id]==':'||value[id]==' '||value[id]=='\t'||value[id]=='\n'))id++;
        ans[std::string(temp)]=parse(value,id);
        while(id<value.size()&&(value[id]==','||value[id]==' '||value[id]=='\t'||value[id]=='\n'))id++;
        if(value[id]=='}')
        {
            id++;
            break;
        }
    }
    return ans;
}
