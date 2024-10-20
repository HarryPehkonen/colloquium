#include "curl_http_client.hpp"
#include "http_client/http_client_exception.hpp"
#include <sstream>
#include <thread>
#include <iostream>
#include <iterator>
#include <algorithm>

namespace http_client {

CurlHTTPClient::CurlHTTPClient() : m_timeout(30000) {
    m_curl = curl_easy_init();
    if (!m_curl) {
        throw HTTPClientException("Failed to initialize libcurl");
    }
}

CurlHTTPClient::~CurlHTTPClient() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
}

std::future<HTTPResponse> CurlHTTPClient::Get(const std::string& uri, const std::vector<std::string>& headers) {
    return PerformRequest("GET", uri, "", headers);
}

std::future<HTTPResponse> CurlHTTPClient::Put(const std::string& uri, const std::string& body, const std::vector<std::string>& headers) {
    return PerformRequest("PUT", uri, body, headers);
}

std::future<HTTPResponse> CurlHTTPClient::Post(const std::string& uri, const std::string& body, const std::vector<std::string>& headers) {
    std::cout << "CurlHTTPClient::POST" << std::endl;
    for (const auto& header : headers) {
        std::cout << "\t" << header << "\n";
    };
    std::cout << "Body: " << body;
    std::cout << std::endl;
    return PerformRequest("POST", uri, body, headers);
}

std::future<HTTPResponse> CurlHTTPClient::Patch(const std::string& uri, const std::string& body, const std::vector<std::string>& headers) {
    return PerformRequest("PATCH", uri, body, headers);
}

std::future<HTTPResponse> CurlHTTPClient::Delete(const std::string& uri, const std::vector<std::string>& headers) {
    return PerformRequest("DELETE", uri, "", headers);
}

void CurlHTTPClient::SetTimeout(std::chrono::milliseconds timeout) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_timeout = timeout;
}

std::chrono::milliseconds CurlHTTPClient::GetTimeout() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_timeout;
}

std::future<HTTPResponse> CurlHTTPClient::PerformRequest(const std::string& method, const std::string& uri, const std::string& body, const std::vector<std::string>& headers) {
    return std::async(std::launch::async, [this, method, uri, body, headers]() {
        std::lock_guard<std::mutex> lock(m_mutex);

        curl_easy_reset(m_curl);
        curl_easy_setopt(m_curl, CURLOPT_URL, uri.c_str());
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT_MS, static_cast<long>(m_timeout.count()));

        struct curl_slist* curl_headers = nullptr;
        for (const auto& header : headers) {
            curl_headers = curl_slist_append(curl_headers, header.c_str());
        }
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, curl_headers);

        std::string response_body;
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_body);

        if (method != "GET") {
            curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, method.c_str());
            if (!body.empty()) {
                curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, body.c_str());
            }
        }

        CURLcode res = curl_easy_perform(m_curl);

        if (curl_headers) {
            curl_slist_free_all(curl_headers);
        }

        if (res != CURLE_OK) {
            throw HTTPClientException(curl_easy_strerror(res));
        }

        long status_code;
        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &status_code);

        HTTPResponse response;
        response.statusCode = static_cast<int>(status_code);
        response.body = response_body;

        return response;
    });
}

size_t CurlHTTPClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        return 0;
    }
    return newLength;
}

} // namespace http_client
