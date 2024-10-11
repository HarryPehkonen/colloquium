#ifndef HTTP_CLIENT_HTTP_CLIENT_EXCEPTION_HPP
#define HTTP_CLIENT_HTTP_CLIENT_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace http_client {

class HTTPClientException : public std::runtime_error {
public:
    explicit HTTPClientException(const std::string& message) : std::runtime_error(message) {}
};

} // namespace http_client

#endif // HTTP_CLIENT_HTTP_CLIENT_EXCEPTION_HPP
