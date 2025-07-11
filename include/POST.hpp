#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
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

class POST : public HttpResponse {
public:
    POST() {};
    ~POST() {};
    void postHandler(HttpRequest &request);


private:
    
};
