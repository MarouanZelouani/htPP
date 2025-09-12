#include "HttpRequest.hpp"

HttpRequest::HttpRequest() 
    : method_(""), path_(""), version_(""), query_string_(""),
         body_(""), content_lenght_found_(false){
    NoneDupHeaders.insert("content_lenght");
    NoneDupHeaders.insert("authorization");
    NoneDupHeaders.insert("form");
    NoneDupHeaders.insert("if-modified-since");
    NoneDupHeaders.insert("referer");
    NoneDupHeaders.insert("user_agent");
    NoneDupHeaders.insert("content_type");
}
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
    // TODO: check for allowed method in config
    if (method != "GET" && method != "POST" && method != "DELETE") {
        throw InvalidMethodName("");
    }
    method_ = method;
}

void HttpRequest::setVersion(const std::string& version) {
    if (version != "HTTP/1.0") {
        std::cout << version << "\n";
        throw MalformedRequestLineException("");
    }
    version_ = version;
}

void HttpRequest::handleURI(const std::string& uri) {
    if (uri.length() > MAX_URI_SIZE)
        throw ;
    if (uri.empty() || uri[0] != '/')
        throw MalformedRequestLineException("");
    std::string allowedChars = "$_-.!*'(),%:@&=+/;?";
    // if (uri.find_first_not_of(allowedChars) != std::string::npos)
    //     throw ;
    for (size_t i = 0; i < uri.length(); i++) {
        if (!std::isalnum(uri[i]))
        {
            if (allowedChars.find(uri[i]) == std::string::npos)
                throw MalformedRequestLineException("");
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
    // TODO: normalize path (example : /../../hdhddhd)
    //       check if the normilized path still exist in the root (cofnig)
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
                throw MalformedRequestLineException("");
            std::string hexPart = encoded.substr(i + 1, 2);
            if (!std::isxdigit(hexPart[0]) || !std::isxdigit(hexPart[1])) 
                throw MalformedRequestLineException("");
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
        throw MalformedHeaderException("");
    for (size_t i = 0; i < newValue.length(); i++) {
        if ((newValue[i] < 32 || newValue[i] > 126) && newValue[i] != 9)
        throw MalformedHeaderException("");
    }
    std::string validChars = "!#$%&'*+-.^_`|~";
    for (size_t i = 0; i < newKey.length(); i++) {
        if (!std::isalnum(newKey[i]))
        {
            if (validChars.find(newKey[i]) == std::string::npos)
                throw InvalidFieldNameException("");
            if (std::isalpha(newKey[i]))
                newKey[i] = std::tolower(newKey[i]);
        }
        // TODO: check duplicate headers : done
        //       check if the Method id POST and there is no content-lenght
        //       check for each METHOD and what heasers must exist for it
        if (newKey == "content-Length") {
            content_lenght_found_ = true;
            signed long contentLen = std::strtol(newValue.c_str(), &tolEnd, 10);
            if (tolEnd == NULL || *tolEnd == '\0' || contentLen < 0)
                throw InvalidContentLengthException("");
            content_lenght_ = contentLen;
        }
    }
    if (!content_lenght_found_ && method_ == "POST")
        throw MissingContentLengthException("");
    if (HttpRequest::isDublicate(newKey))
        throw DuplicateHeaderException("");
    headers_.insert({newKey, newValue});
}

bool HttpRequest::isDublicate(const std::string& name) {
    std::map<std::string, std::string>::const_iterator map_it;
    std::set<std::string>::iterator set_itr = NoneDupHeaders.find("name");
    for (map_it = headers_.begin(); map_it != headers_.end(); ++map_it) {
        if (name == map_it->first && set_itr == NoneDupHeaders.end())
            return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& request) {
    os << "---------------------REQUEST-------------------\n";
    os << "     --------------REQUEST LINE------------    \n";
    os << "Method [" << request.method_ << "]\n";
    os << "Path [" << request.path_ << "]\n";
    os << "Query String [" << request.query_string_ << "]\n";
    os << "Version: [" << request.version_ << "]\n";
    os << "     ----------------HEADERS---------------    \n";
    request.printHeaders();
    os << "-----------------------------------------------\n";
    return os;
}

void HttpRequest::printHeaders() const {
    if (!this->hasHeaders()) {
        std::cout << "No Headers!\n";
        return ;
    }
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers_.begin(); it != headers_.end(); ++it) {
        std::cout << it->first << ": " << it->second << "\n";
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