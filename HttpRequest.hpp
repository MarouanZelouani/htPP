#ifndef _HTTP_REQUEST__
#define _HTTP_REQUEST__

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

class HttpRequest {
    private:
    // request line componenet
    std::string method_;
    std::string path_;
    std::string version_;
    std::string query_string_;

    // headers
    std::map<std::string, std::string> headers_;

    // body
    std::string body_;
    size_t content_lenght_;

    static const int MAX_URI_SIZE = 1024;

    // getters only accessible by the Parser
    void setMethod(const std::string& method);
    void handleURI(const std::string& uri);
    void setPath(const std::string& path);
    void setVersion(const std::string& version);
    void setQueryString(const std::string& query);
    void addHeader(const std::string& key, const std::string& value);

    // helper functions
    std::string percentDecode(const std::string& encoded);
    std::string trim(const std::string& str);
    
    public:
    // constructor and Deconstructor
    HttpRequest();
    ~HttpRequest();
    
    // add copy and assignment op later
    
    // only HttpParser can access the setters
    friend class HttpParser;

    // getters
    std::string getMethod() const;
    std::string getPath() const;
    std::string getVersion() const;
    std::string getQueryString() const;

    // utility methods
    bool hasHeaders() const;
    std::string getHeader(const std::string& headerName) const;
    void ptintHeaders() const;

    friend std::ostream& operator<<(std::ostream& os, const HttpRequest& request);
};

#endif