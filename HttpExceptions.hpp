#ifndef _HTTP_EXCEPTIONS__
#define _HTTP_EXCEPTIONS__

#include <exception>
#include <string>

enum HttpErrorCode {
    // TODO : check for codes
    MALFORMED_REQUEST_LINE = 1000,
    INVALID_METHOD_NAME = 1001,
    MALFORMED_HEADER = 2000,
    INVALID_FIELD_NAME = 2001,
    MISSING_CONTENT_LENGTH = 2100,
    INVALID_CONTENT_LENGTH = 2101,
    DUPLICATE_HEADER = 2102,
    INVALID_MESSAGE_STRUCTURE = 3000
};

class HttpRequestException : public std::exception {
private:
    std::string message_;
    HttpErrorCode error_code_;

public:
    HttpRequestException(const std::string& message, HttpErrorCode error_code) 
        : message_(message), error_code_(error_code) {}
    const char* what() const throw() { return message_.c_str(); }
    HttpErrorCode error_code() const { return error_code_; }
};

class MalformedRequestLineException : public HttpRequestException {
public:
    MalformedRequestLineException(const std::string& message) 
        : HttpRequestException(message, MALFORMED_REQUEST_LINE) {}
};

class MalformedHeaderException : public HttpRequestException {    
public:
    MalformedHeaderException(const std::string& message) 
        : HttpRequestException(message, MALFORMED_HEADER) {}
};

class InvalidMethodName : public HttpRequestException {    
public:
    InvalidMethodName(const std::string& message) 
        : HttpRequestException(message, INVALID_METHOD_NAME) {}
};

class InvalidFieldNameException : public HttpRequestException {
public:
    InvalidFieldNameException(const std::string& message)
        : HttpRequestException(message, INVALID_FIELD_NAME) {}
};

class MissingContentLengthException : public HttpRequestException {
public:
    MissingContentLengthException(const std::string& message) 
        : HttpRequestException(message, MISSING_CONTENT_LENGTH) {}
};

class InvalidContentLengthException : public HttpRequestException {
public:
    InvalidContentLengthException(const std::string& message)
        : HttpRequestException(message, INVALID_CONTENT_LENGTH) {}
};

class DuplicateHeaderException : public HttpRequestException {
public:
    DuplicateHeaderException(const std::string& message)
        : HttpRequestException(message, DUPLICATE_HEADER) {}
};

class MessageStructureException : public HttpRequestException {
public:
    MessageStructureException(const std::string& message) 
        : HttpRequestException(message, INVALID_MESSAGE_STRUCTURE) {}
};

#endif