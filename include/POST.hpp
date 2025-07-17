#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <map>

void handlePost(HttpRequest& request, HttpResponse& response);
