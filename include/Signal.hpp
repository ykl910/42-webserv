#pragma once

#include "textFormatting.hpp"
#include <stdexcept>
#include <iostream>
#include <csignal>

extern int g_signal;

void initSignalHandler(void);
void signalHandler(int signum);
