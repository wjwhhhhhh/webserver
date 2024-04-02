//
// Created by wjw on 2024/3/23.
//

#ifndef ORM_JSONWRITER_H
#define ORM_JSONWRITER_H

#include "json.h"
namespace Eula::json
{
    class Writer
    {
        public:
        bool Key(const std::string& key);
        bool Key(const char* key);
        bool StartObject();
        bool EndObject();
        bool StartArray();
        bool EndArray();
        bool Int(const int & val);
        bool String(const std::string& val);
        bool String(const char * val);
        bool Double(const double val);
        bool Bool (bool val);
        bool Null();
        std::string& GetString();
        private:
        std::string ans;
    };
}

#endif //ORM_JSONWRITER_H
