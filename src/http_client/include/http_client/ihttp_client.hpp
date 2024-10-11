#ifndef HTTP_CLIENT_IHTTP_CLIENT_HPP
#define HTTP_CLIENT_IHTTP_CLIENT_HPP

#include <string>
#include <vector>
#include <future>
#include <chrono>
#include "http_response.hpp"

namespace http_client {

class IHTTPClient {
public:
    virtual ~IHTTPClient() = default;

    virtual std::future<HTTPResponse> Get(const std::string& uri, const std::vector<std::string>& headers = {}) = 0;
    virtual std::future<HTTPResponse> Put(const std::string& uri, const std::string& body, const std::vector<std::string>& headers = {}) = 0;
    virtual std::future<HTTPResponse> Post(const std::string& uri, const std::string& body, const std::vector<std::string>& headers = {}) = 0;
    virtual std::future<HTTPResponse> Patch(const std::string& uri, const std::string& body, const std::vector<std::string>& headers = {}) = 0;
    virtual std::future<HTTPResponse> Delete(const std::string& uri, const std::vector<std::string>& headers = {}) = 0;

    virtual void SetTimeout(std::chrono::milliseconds timeout) = 0;
    virtual std::chrono::milliseconds GetTimeout() const = 0;
};

} // namespace http_client

#endif // HTTP_CLIENT_IHTTP_CLIENT_HPP
