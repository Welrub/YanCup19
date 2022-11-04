#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.hpp"
using json = nlohmann::json;

int count_sum(std::vector<int>& arr){
    int res = 0;
    for(int i = 0; i < arr.size(); ++i){
        res += arr[i];
    }
    return res;
}

static size_t call_b(void* data, size_t size, size_t nmemb, void* userp){
    ((std::string*)userp)->append((char*)data, size * nmemb);
    return size * nmemb;    
}

std::vector<int> parser(std::string& buffer){
    std::vector<int> res;
    json j = json::parse(buffer);
    res = j.get<std::vector<int>>();
    return res;
}

int main()
{
    std::string url_var, port, a, b;
    std::cin >> url_var >> port >> a >> b;
    url_var += ":" + port + "?a=" + a + "&b=" + b;
    CURL *curl = curl_easy_init();
    CURLcode res;
    std::string buffer = "";
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url_var.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_b);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    std::vector<int> arr = parser(buffer);
    std::cout << count_sum(arr) << std::endl;
    return 0;
}
