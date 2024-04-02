//
// Created by wjw on 2024/3/23.
//
#include "writer.h"
using namespace Eula::json;
using std::string;
bool Writer::Key(const std::string &key) {
    ans+='\"';
    ans+=key;
    ans+="\":";
    return true;
}

bool Writer::Key(const char *key) {
    string tem(key);
    return Key(tem);
}

bool Writer::StartObject() {
    ans+="{";
    return true;
}

bool Writer::EndObject() {
    if(ans.back()==',')ans.pop_back();
    ans.push_back('}');
    ans+=",";
    return true;
}

bool Writer::Int(const int &val) {
    ans+=std::to_string(val);
    ans+=",";
    return true;
}

bool Writer::String(const string &val) {
    ans+='\"';
    ans+=val;
    ans+="\",";
    return true;
}

bool Writer::String(const char *val) {
    string tem(val);
    String(tem);
    return true;
}

bool Writer::Double(const double val) {
    ans+=std::to_string(val);
    ans+=",";
    return true;
}

bool Writer::Bool(bool val) {
    if(val==true)ans+="true";
    else ans+="false";
    ans+=",";
    return true;
}

bool Writer::Null() {
    ans+="null";
    ans+=",";
}

bool Writer::StartArray() {
    ans+="{";
    return true;
}

bool Writer::EndArray() {
    if(ans.back()==',')ans.pop_back();
    ans.push_back('}');
    ans+=",";
    return true;
}

std::string &Writer::GetString()  {
    if(ans.back()==',')ans.pop_back();
    return ans;
}
