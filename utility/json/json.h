//
// Created by wjw on 2023/12/13.
//

#ifndef ORM_JSON_H
#pragma once
#define ORM_JSON_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
namespace Eula
{
    namespace json
    {

          class Json
          {
          public:
              enum Type
              {
                  json_null=0,
                  json_bool,
                  json_int,
                  json_double,
                  json_string,
                  json_array,
                  json_object
              };
                Json();
                ~Json();
                void clear();
                Json(const bool& value);
                Json (const int &value);
                Json(const double &value);
                Json(const char*value);
                Json(const std::string&value);
                Json (const Type type);
                Json(const Json & other);
                explicit operator bool() const;
                explicit operator std::string() const;
                explicit operator int() const;
                explicit operator double() const;

                Json& operator = (const Json& other);
                Json & operator[](int index);
              Json & operator[](const char * key);
              Json & operator[](const std::string &key);
                void append(const Json& other);
                 std::string str() const;
                bool IsObject() const;
              union Value
              {
                  bool m_bool;
                  int m_int;
                  double m_double;
                  std::string* m_string;
                  std::vector<Json> *m_array;
                  std::map<std::string,Json> *m_object;
              };
          private:


              Type m_type;
              Value m_value{};
          };
        Json parse(const char * value);
        Json parse(const std::string &value,int &id);
        Json parse(const std::string &value);
        Json parse_null(const std::string &value,int &id);
        Json parse_bool(const std::string &value,int &id);
        Json parse_number(const std::string &value,int &id);
        Json parse_array(const std::string &value,int &id);
        Json parse_string(const std::string &value,int &id);
        Json parse_object(const std::string &value,int &id);
    };
};
#endif //ORM_JSON_H
