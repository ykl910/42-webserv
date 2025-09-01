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

static void    runSpecificMultiplexer(Config& config, std::string multiplexer)
{
    if (multiplexer == "select")
        WebServ<Select> server(config, "select");
    else if (multiplexer == "poll")
        WebServ<Poll>   server(config, "poll");
    else if (multiplexer == "epoll")
        WebServ<Epoll>  server(config, "epoll");
}

int main(int argc, char **argv)
{
    const char* configFilePath(DEFAULT_PATH);

    if (gotConfigFilePath(argc, argv[1]))
        configFilePath = argv[1];
    try {
        Config  config(configFilePath);

        initSignalHandler();
        if ((argc == 2 || argc == 3) && hadChoosenMultiplexer(argv[argc - 1]))
            runSpecificMultiplexer(config, argv[argc - 1]);
        else
            WebServ<Epoll> server(config, "epoll");
    } catch (std::exception& e) {
        std::cerr << BOLD RED << e.what() << DEFAULT << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
