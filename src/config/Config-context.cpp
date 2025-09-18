#include "../../include/Config.hpp"

const char* _contextNameList[5] = {
    "server:", "location:", "error_page:", "redirection:", "cgi:"};

bool    Config::contextFormatValid(const std::string& line)
{
    size_t indentSize = 0;

    if (_contextIndex > 0)
        indentSize = 4;
    if (_contextIndex == LOCATION) {
        if (line.length() <= LOCATION_STRING_LENGTH + indentSize)
            manageConfigError(line, EXPECTED_CONTEXT,
                "location context format not valid.", _lineNbr);
        else
            return true;
    } else if (line.empty()
        || std::strlen(_contextNameList[_contextIndex]) + indentSize
                != line.length()
        || _contextNameList[_contextIndex] != line.substr(indentSize,
                                                  line.length() - indentSize))
        manageConfigError(line, EXPECTED_CONTEXT,
            "context format not valid.", _lineNbr);
    return true;
}

void    Config::extractContext(std::ifstream& file, std::string& line,
                               server& server)
{
    _locationNbr = 0;
    _contextIndex = SERVER;
    while (_contextIndex < _configFormat.size()) {
        std::getline(file, line);
        _lineNbr++;
        if (file.eof())
            break;

        if (_contextIndex == LOCATION && contextFormatValid(line)) {
            std::ifstream::pos_type streamPos;
            while (contextFormatValid(line)) {
                if (line.substr(4, 9) != "location:") {
                    file.seekg(streamPos);
                    break;
                }
                extractDirective(file, line, server);
                streamPos = file.tellg();
                std::getline(file, line);
                _locationNbr++;
            }

        } else if (contextFormatValid(line))
            extractDirective(file, line, server);
        _contextIndex++;
    }
    _locationNbrMap[_serverIndex] = _locationNbr;
    _cgiNbrMap[_serverIndex] = _cgiNbr;
    std::getline(file, line);
}
