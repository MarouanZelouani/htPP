#include "HttpRequest.hpp"

HttpRequest::HttpRequest() 
    : method_(""), path_(""), version_(""), query_string_(""), body_("") {}
HttpRequest::~HttpRequest(){}

std::string HttpRequest::getMethod() const { return method_; }
std::string HttpRequest::getPath() const { return path_; }
std::string HttpRequest::getVersion() const { return version_; }
std::string HttpRequest::getQueryString() const { return query_string_; }

bool HttpRequest::hasHeaders() const {
    if (!headers_.empty())
        return true; 
    return false;
};

std::string HttpRequest::getHeader(const std::string& headerName) const {
    if (!this->hasHeaders())
        return NULL;
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers_.begin(); it != headers_.end(); ++it) {
        if (it->first == headerName)
            return it->second;
    }
    return NULL;
}

void HttpRequest::setMethod(const std::string& method) {
    if (method != "GET" && method != "POST" && method != "DELETE") {
        throw "Error!23323";
    }
    method_ = method;
}

void HttpRequest::setVersion(const std::string& version) {
    if (version != "HTTP/1.0" && version != "HTTP/1.1") {
        std::cout << version << "\n";
        throw "Error!2332";
    }
    version_ = version;
}

void HttpRequest::handleURI(const std::string& uri) {
    if (uri.length() > MAX_URI_SIZE)
        throw ;
    if (uri.empty() || uri[0] != '/')
        throw "Error12!";
    std::string allowedChars = "$_-.!*'(),%:@&=+/;?";
    // if (uri.find_first_not_of(allowedChars) != std::string::npos)
    //     throw ;
    for (size_t i = 0; i < uri.length(); i++) {
        if (!std::isalnum(uri[i]))
        {
            if (allowedChars.find(uri[i]) == std::string::npos)
                throw "er34";
        }    
    }
    size_t queryPos = uri.find('?');
    if (queryPos == std::string::npos) {
        this->setPath(uri);
        return ;
    }
    this->setPath(uri.substr(0, queryPos));
    this->setQueryString(uri.substr(queryPos + 1));
}

void HttpRequest::setPath(const std::string& path) {
    path_ = HttpRequest::percentDecode(path);
}

void HttpRequest::setQueryString(const std::string& query) {
    query_string_ = HttpRequest::percentDecode(query);
}

std::string HttpRequest::percentDecode(const std::string& encoded) {
    std::string result = "";
    for (size_t i = 0; i < encoded.length(); i++) {
        if (encoded[i] == '%') {
            if (i + 2 > encoded.length())
                throw "error44";
            std::string hexPart = encoded.substr(i + 1, 2);
            if (!std::isxdigit(hexPart[0]) || !std::isxdigit(hexPart[1])) 
                throw "error55";
            int hexPartInt = std::strtol(hexPart.c_str(), NULL, 16);
            result += static_cast<char>(hexPartInt);
            i += 2;
        } else {
            result += encoded[i];
        }
    }
    return result;
}   

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
    std::string newKey = HttpRequest::trim(key);
    std::string newValue = HttpRequest::trim(value);
    char *tolEnd;
    if (newKey.empty() || value.empty())
        throw "error=";
    for (size_t i = 0; i < newValue.length(); i++) {
        if ((newValue[i] < 32 || newValue[i] > 126) && newValue[i] != 9)
        throw "ert34";
    }
    std::string validChars = "!#$%&'*+-.^_`|~";
    for (size_t i = 0; i < newKey.length(); i++) {
        if (!std::isalnum(newKey[i]))
        {
            if (validChars.find(newKey[i]) == std::string::npos)
                throw "er34+";
            if (std::isalpha(newKey[i]))
                std::tolower(newKey[i]);
        }
        if (newKey == "content-Length") {
            signed long contentLen = std::strtol(newValue.c_str(), &tolEnd, 10);
            if (tolEnd == NULL || *tolEnd == '\0' || contentLen <0)
                throw "stol";
            content_lenght_ = contentLen;
        }
    }
    headers_.insert({newKey, newValue});
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& request) {
    os << "----------------------------------------\n";
    os << "Method: " << request.method_ << '\n';
    os << "Path: " << request.path_ << '\n';
    os << "Query String: " << request.query_string_ << '\n';
    os << "Version: " << request.version_ << '\n';
    os << "----------------------------------------\n";
    return os;
}

void HttpRequest::ptintHeaders() const {
    if (!this->hasHeaders()) {
        std::cout << "no headers!\n";
        return ;
    }
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers_.begin(); it != headers_.end(); ++it) {
        std::cout << it->first << ":" << it->second << "\n";
    }
}

std::string HttpRequest::trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(str[start]))
        start++;
    if (start == str.size())
        return "";
    size_t end = str.size() - 1;
    while (end > start && std::isspace(str[end]))
        end--;
    return str.substr(start, end - start + 1);
}