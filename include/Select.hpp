#pragma once

#include <sys/select.h>

class WebServ;

class Select {
public:
    void run(WebServ& server);

    Select();
    ~Select();

private:

};