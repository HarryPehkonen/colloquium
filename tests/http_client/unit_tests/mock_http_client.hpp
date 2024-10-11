#ifndef HTTP_CLIENT_MOCK_HTTP_CLIENT_HPP
#define HTTP_CLIENT_MOCK_HTTP_CLIENT_HPP

#include <gmock/gmock.h>
#include "http_client/ihttp_client.hpp"

namespace http_client {

class MockHTTPClient : public IHTTPClient {
public:
    MOCK_METHOD(std::future<HTTPResponse>, Get, (const std::string&, const std::vector<std::string>&), (override));
    MOCK_METHOD(std::future<HTTPResponse>, Put, (const std::string&, const std::string&, const std::vector<std::string>&), (override));
    MOCK_METHOD(std::future<HTTPResponse>, Post, (const std::string&, const std::string&, const std::vector<std::string>&), (override));
    MOCK_METHOD(std::future<HTTPResponse>, Patch, (const std::string&, const std::string&, const std::vector<std::string>&), (override));
    MOCK_METHOD(std::future<HTTPResponse>, Delete, (const std::string&, const std::vector<std::string>&), (override));
    MOCK_METHOD(void, SetTimeout, (std::chrono::milliseconds), (override));
    MOCK_METHOD(std::chrono::milliseconds, GetTimeout, (), (const, override));
};

} // namespace http_client

#endif // HTTP_CLIENT_MOCK_HTTP_CLIENT_HPP
