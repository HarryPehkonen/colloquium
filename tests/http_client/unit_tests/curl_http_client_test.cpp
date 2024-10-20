#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "http_client/ihttp_client.hpp"
#include "exceptions/llm_exceptions.hpp"
#include "http_client/curl_http_client.hpp"
#include <chrono>

using namespace http_client;
using namespace testing;

class CurlHTTPClientTest : public Test {
protected:
    void SetUp() override {
        client = std::make_unique<CurlHTTPClient>();
    }

    std::unique_ptr<IHTTPClient> client;
};

TEST_F(CurlHTTPClientTest, GetRequestReturnsValidResponse) {
    auto future = client->Get("http://example.com");
    auto response = future.get();

    EXPECT_EQ(response.statusCode, 200);
    EXPECT_FALSE(response.body.empty());
}

TEST_F(CurlHTTPClientTest, PostRequestSendsData) {
    std::string body = "test data";
    auto future = client->Post("http://httpbin.org/post", body);
    auto response = future.get();

    EXPECT_EQ(response.statusCode, 200);
    EXPECT_THAT(response.body, HasSubstr(body));
}

TEST_F(CurlHTTPClientTest, SetTimeoutChangesTimeout) {
    auto newTimeout = std::chrono::milliseconds(5000);
    client->SetTimeout(newTimeout);
    EXPECT_EQ(client->GetTimeout(), newTimeout);
}

TEST_F(CurlHTTPClientTest, RequestWithCustomHeadersSendsHeaders) {
    std::vector<std::string> headers = {"X-Test-Header: test_value"};
    auto future = client->Get("http://httpbin.org/headers", headers);
    auto response = future.get();

    EXPECT_EQ(response.statusCode, 200);
    EXPECT_THAT(response.body, HasSubstr("X-Test-Header"));
    EXPECT_THAT(response.body, HasSubstr("test_value"));
}

TEST_F(CurlHTTPClientTest, RequestToNonexistentUrlThrowsException) {
    EXPECT_THROW({
        auto future = client->Get("http://thisurldoesnotexist.test");
        future.get();
    }, llm::HTTPException);
}
