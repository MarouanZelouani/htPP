#ifndef PARSER_HPP
#define PARSER_HPP

#include "Config.hpp"
#include <fstream>
#include <string>
#include <vector>

class Parser {
public:
    static std::vector<ServerConfig> parseConfigFile(const std::string& filename);
    static ServerConfig parseServer(std::ifstream& file);
    static LocationConfig parseLocation(std::ifstream& file, const std::string& path, bool braceOnSameLine = false);
    static void validateBraceLine(const std::string& line, char brace);
    static void validateDirectiveLine(const std::string& line, const std::string& directive);
};

#endif