#include "../../include/Config.hpp"

bool    Config::directiveFormatValid(const std::string& line, size_t indentSize)
{
    size_t lineLength = line.length();

    if (lineLength == 0)
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "line empty when expecting a directive.", _lineNbr);

    else if (lineLength < indentSize
          || lineLength < indentSize + DIRECTIVE_NAME_LENGTH)
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "config not well formatted.", _lineNbr);

    else if (!correctlyIndended(line, indentSize))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "wrong indentation.", _lineNbr);

    else if (_configFormat[_contextIndex][_directiveIndex][NAME]
            != line.substr(indentSize, DIRECTIVE_NAME_LENGTH))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "directive name not valid.", _lineNbr);

    std::string tmp = line.substr(indentSize + DIRECTIVE_NAME_LENGTH);
    if (!std::isspace(tmp[0]))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "missing space between directive name and directive value.", _lineNbr);

    else if (_contextIndex == LOCATION && tmp.length() < 2
        && (!tmp[1] || tmp[1] != '/'))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "location directive invalid.", _lineNbr);

    else if (!tmp[1] || std::isspace(tmp[1]))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "directive value not well formatted.", _lineNbr);

    return true;
}

void    Config::getDirective(std::string& line,
                                 directiveValue& newDirective, size_t indentSize)
{
    std::string directiveValue(
        line.substr(indentSize + DIRECTIVE_NAME_LENGTH + 1,
        line.length() - indentSize - DIRECTIVE_NAME_LENGTH - 1));

    if (directiveValue.empty()) {
        manageConfigError(line, "",
            "directive value cannot be empty.", _lineNbr);

    } else if (_contextIndex == SERVER
        && _directiveIndex == CLIENT_MAX_BODY_SIZE) {

        size_t length(directiveValue.length());

        for (size_t i = 0; directiveValue[i]; ++i) {
            if (!std::isdigit(directiveValue[i])) {
                if (length - i > 1
                    || directiveValue.substr(i, length).
                        find_first_not_of("kKmMgG") != std::string::npos)
                    manageConfigError(line,
                        "client max body size format must be:\n"
                        + std::string("[number][k,K,m,M,g,G]"),
                        "max body size not well formatted.", _lineNbr);
                break;
            }
        }

    } else if (_contextIndex == SERVER && _directiveIndex == LISTEN) {
        if (directiveValue.find_first_not_of("0123456789") != std::string::npos)
            manageConfigError(line, "a port range between 0-65535",
                "port number not well formatted.", _lineNbr);

      else if (directiveValue.length() > MAX_PORT_VALUE_LENGTH
        || std::atol(directiveValue.c_str()) > MAX_PORT_VALUE)
        manageConfigError(line, "a port range between 0-65535",
            "port number not valid.", _lineNbr);
    }

    newDirective.push_back(directiveValue);
}

void    Config::extractDirective(std::ifstream& file, std::string& line,
                                 server& server)
{
    int indentSize;

    if (_contextIndex == 0)
        indentSize = 4;
    else
        indentSize = 8;

    _directiveIndex = 0;
    while (_directiveIndex < _configFormat[_contextIndex].size()
        || _contextIndex == CGI) {
        directiveValue   newDirective;

        if (_contextIndex == LOCATION
            && _directiveIndex == PATH) {
            newDirective.push_back(line.substr(4 + 10,
                                    line.length() - 4 - 10));

        } else if (_contextIndex == CGI) {
            _cgiNbr = 0;
            while (true) {
                directiveValue newCgi;
                std::ifstream::pos_type streamPos;

                streamPos = file.tellg();
                std::getline(file, line);
                if (file.eof())
                    break;
                else if (line.length() == 0) {
                    file.seekg(streamPos);
                    break;
                }
                newCgi.push_back(line.substr(indentSize, line.length() - indentSize));
                server[_contextIndex + _locationNbr + _cgiTotal][_directiveIndex] = newCgi;
                _directiveIndex++;
                _lineNbr++;
                _cgiNbr++;
            }
            _cgiTotal += _cgiNbr;
            return;

        } else {
            std::getline(file, line);
            if (directiveFormatValid(line, indentSize))
                getDirective(line, newDirective, indentSize);
        }
        _lineNbr++;
        server[_contextIndex + _locationNbr][_directiveIndex] = newDirective;
        _directiveIndex++;
    }
}
