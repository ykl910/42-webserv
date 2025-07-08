#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "../include/HttpRequest.hpp"

class HttpResponse {

	public:
        HttpResponse(HttpRequest &request);
        void setCode(int code);
        void setHeaders(std::string &key, std::string &value);
        void setBody(std::string &body);
        std::string getResponse() const;   //TODO
        size_t getResponseLength() const; //TODO

    private:
        void    build();
        std::string statusLine;
        std::map<std::string, std::string> headers;
        std::string body;

};
