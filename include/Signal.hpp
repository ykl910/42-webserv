#pragma once

#include "textFormatting.hpp"
#include <stdexcept>
#include <iostream>
#include <csignal>

void initSignalHandler(void);
void signalHandler(int signum);
