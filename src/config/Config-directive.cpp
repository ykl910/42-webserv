#include "../../include/Config.hpp"

bool    Config::directiveFormatValid(const std::string& line, int indentSize)
{
    int lineLength = line.length();

    if (lineLength == 0)
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "line empty when expecting a directive.", _lineNbr);

    else if (lineLength < indentSize
          || lineLength < indentSize + static_cast<int>(DIRECTIVE_NAME_LENGTH))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "config not well formatted.", _lineNbr);

    else if (!rightIndentation(line, indentSize))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "wrong indentation.", _lineNbr);

    // is directive name format valid
    else if (_configFormat[_contextIndex][_directiveIndex][NAME]
            != line.substr(indentSize, DIRECTIVE_NAME_LENGTH))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "directive name not valid.", _lineNbr);

    std::string tmp = line.substr(indentSize + DIRECTIVE_NAME_LENGTH);
    if (!std::isspace(tmp[0]))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "no space between directive name and directive value.", _lineNbr);

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
                                 directiveValue& newDirective, int indentSize)
{
    newDirective.push_back(line.substr(indentSize + DIRECTIVE_NAME_LENGTH + 1,
        line.length() - indentSize - DIRECTIVE_NAME_LENGTH - 1));
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
