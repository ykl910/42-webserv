#include "../include/WebServ.hpp"
#include "../include/Signal.hpp"

#define DEFAULT_PATH "config/test.conf"

static bool    hadChoosenMultiplexer(const std::string& input)
{
    return !input.empty() && (input == "select" || input == "poll"
                                                || input == "epoll");
}

static bool    gotRightSuffix(const std::string& config_file)
{
    std::string suffix(".conf");
    return !config_file.compare(config_file.length() - suffix.length(),
            suffix.length(), suffix);
}

static bool    gotConfigFilePath(const int argc, const char* argv)
{
    if ((argc == 2 || argc == 3) && !hadChoosenMultiplexer(argv)) {
        try {
            std::string config_file(argv);
            if (config_file.empty())
                throw std::runtime_error("Error: no config file name.");
            else if (!gotRightSuffix(config_file))
                throw std::runtime_error("Error: wrong config file name.");
            else if (access(config_file.c_str(), R_OK) != 0)
                throw std::runtime_error("Error: can't read config file.");
            return true;
        } catch (std::exception& e) {
            std::cerr << BOLD RED << e.what() << DEFAULT << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return false;
}

static void    runSpecificMultiplexer(const std::string& multiplexer,
                               const char* configFilePath)
{
    if (multiplexer == "select")
        WebServ<Select> server(configFilePath, "select");
    else if (multiplexer == "poll")
        WebServ<Poll>   server(configFilePath, "poll");
    else if (multiplexer == "epoll")
        WebServ<Epoll>  server(configFilePath, "epoll");
}

int main(int argc, char **argv)
{
    std::string configFilePath(DEFAULT_PATH);

    if (gotConfigFilePath(argc, argv[1]))
        configFilePath = std::string(argv[1]);
    try {
        initSignalHandler();
        if ((argc == 2 || argc == 3) && hadChoosenMultiplexer(argv[argc - 1]))
            runSpecificMultiplexer(argv[argc - 1], configFilePath.c_str());
        else
            WebServ<Epoll> server(configFilePath.c_str(), "epoll");
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
