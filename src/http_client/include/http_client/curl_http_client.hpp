#ifndef HTTP_CLIENT_CURL_HTTP_CLIENT_HPP
#define HTTP_CLIENT_CURL_HTTP_CLIENT_HPP

#include "http_client/ihttp_client.hpp"
#include <curl/curl.h>
#include <mutex>

namespace http_client {

class CurlHTTPClient : public IHTTPClient {
public:
    CurlHTTPClient();
    ~CurlHTTPClient() override;

    std::future<HTTPResponse> Get(const std::string& uri, const std::vector<std::string>& headers = {}) override;
    std::future<HTTPResponse> Put(const std::string& uri, const std::string& body, const std::vector<std::string>& headers = {}) override;
    std::future<HTTPResponse> Post(const std::string& uri, const std::string& body, const std::vector<std::string>& headers = {}) override;
    std::future<HTTPResponse> Patch(const std::string& uri, const std::string& body, const std::vector<std::string>& headers = {}) override;
    std::future<HTTPResponse> Delete(const std::string& uri, const std::vector<std::string>& headers = {}) override;

    void SetTimeout(std::chrono::milliseconds timeout) override;
    std::chrono::milliseconds GetTimeout() const override;

private:
    std::future<HTTPResponse> PerformRequest(const std::string& method, const std::string& uri, const std::string& body, const std::vector<std::string>& headers);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);

    CURL* m_curl;
    std::chrono::milliseconds m_timeout;
    mutable std::mutex m_mutex;
};

} // namespace http_client

#endif // HTTP_CLIENT_CURL_HTTP_CLIENT_HPP
