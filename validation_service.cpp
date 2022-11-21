#include <iostream>
#include <string>
#include "httplib.h"
#include <unordered_set>
using namespace httplib;

std::unordered_set<std::string> valid_codes({"982", "986", "912", "934"});
std::unordered_set<std::string> valid_number_form({" code ### ####",
                                                   " code ### ## ##",
                                                   " (code) ###-####",
                                                   "code#######"});

bool is_valid(std::string& number){
    std::string new_num = "";
    int i = 0; int n = number.size();
    if(number.size() < 11 && number.substr(0, 4) != "%2B7" && number[0] != '8'){
        return false;
    }
    i += (number[i] == '8') ? 1 : 4;
    if(number[i] == '+') { 
        new_num += ' '; 
        ++i;
    }
    if(number.substr(i, 3) == "%28"){
        new_num += '(';
        i += 3;
    }
    if(valid_codes.find(number.substr(i, 3)) != valid_codes.end()){
        new_num += "code";
        i += 3;
    }else{
        return false;
    }
    if(number.substr(i, 3) == "%29"){
        new_num += ')';
        i += 3;
    }
    while(i < n){
        if(number[i] == '+'){
            new_num += ' ';
            ++i;
            continue;
        }
        if(number[i] == '-'){
            new_num += '-';
        }else{
            new_num += '#';
        }
        ++i;
    }
    return (valid_number_form.find(new_num) != valid_number_form.end());
}

std::string normalization(std::string& number){
    std::string s = "+7-   -   -    ";
    int i = 0; int j = 3;
    i += (number[i] == '8') ? 1 : 4;
    i += (number[i] == '+') ? 1 : 0;
    i += (number.substr(i, 3) == "%28") ? 3 : 0;
    for(; j < 6; ++j, ++i){
        s[j] = number[i];
    }
    i += (number.substr(i, 3) == "%29") ? 3: 0;
    i += (number[i] == '+') ? 1 : 0;
    while(j < s.size()){
        j += (s[j] == '-') ? 1 : 0;
        if(number[i] == '+' || number[i] == '-'){
            ++i;
        } 
        s[j] = number[i];
        ++i; ++j;
    }
    return s;
}

void common_handler(const Request& req, Response& res){
    if(!req.has_param("phone_number")){
        res.status = 400;
        return;
    }
    std::string number = req.get_param_value("phone_number");
    std::string ans = "";
    if(is_valid(number)){
        ans += "normalized: " + normalization(number) + '\n';
        ans += "status: true";
    }else{
        ans += "status: false";
    }
    res.set_content(ans, "text/plain");
}

int main(){
    Server svr;
    svr.Get("/validatePhoneNumber", common_handler);
    svr.Get("/shutdown", [&](const Request& req, Response& res){
        svr.stop();
    });
    svr.Get("/ping", [&](const Request& req, Response& res){
        res.status = 200;
    });
    svr.listen("127.0.0.1", 7777);
    return 0;
}