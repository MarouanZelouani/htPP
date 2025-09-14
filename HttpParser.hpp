#ifndef _HTTP_PARSER__
#define _HTTP_PARSER__

#include "HttpRequest.hpp"
#include <algorithm>

enum ParserState {
    PARSING_REQUEST_LINE,
    PARSING_HEADERS,
    PARSING_BODY,
    COMPLETE,
    ERROR
};

class HttpParser {
    private:
    std::string buffer_;
    ParserState state_;
    HttpRequest httpRequest_;
    size_t bytes_read_;
    
    // parsing functions
    bool parseRequestLine();
    bool parseHeaders();
    bool parseBody();
    bool hasEnoughData();
    
    public:
    HttpParser();
    ~HttpParser();
    
    // main functions
    int parseHttpRequest(const std::string& RequestData);
    void launch(const std::string& buffer);
};

#endif