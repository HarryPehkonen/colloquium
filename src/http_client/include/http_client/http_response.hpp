#ifndef HTTP_CLIENT_HTTP_RESPONSE_HPP
#define HTTP_CLIENT_HTTP_RESPONSE_HPP

#include <string>
#include <vector>

namespace http_client {

struct HTTPResponse {
    int statusCode;
    std::vector<std::string> headers;
    std::string body;
};

} // namespace http_client

#endif // HTTP_CLIENT_HTTP_RESPONSE_HPP
