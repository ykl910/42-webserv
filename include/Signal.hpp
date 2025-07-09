#pragma once

#include "textFormatting.hpp"
#include <stdexcept>
#include <iostream>
#include <csignal>

class Signal {
public:
    Signal();
    ~Signal();

private:
    static void sigHandler(int signum);
};
