// src/session/llm_session.hpp
#pragma once

#include "translator/itranslator.hpp"
#include "http_client/ihttp_client.hpp"
#include "core/message.hpp"
#include "core/tool.hpp"
#include "core/source.hpp"
#include <async_deque/async_deque.hpp>
#include <memory>
#include <string>
#include <vector>

class LLMSession {
public:
    LLMSession(
        std::unique_ptr<http_client::IHTTPClient> httpClient
    );

    // Prevent copying
    LLMSession(const LLMSession&) = delete;
    LLMSession& operator=(const LLMSession&) = delete;

    // Allow moving
    LLMSession(LLMSession&&) = default;
    LLMSession& operator=(LLMSession&&) = default;

    // Main interface
    void addTool(Tool tool);
    void processMessages(Source& source);

    // Access to conversation history
    const std::vector<std::unique_ptr<Message>>& getConversation() const;

private:

    // Helper methods for message processing
    void handleSystemMessage(const std::string& content);
    void handleUserMessage(const std::string& content);
    void processToolCalls(
        const Message& response, 
        async_deque::AsyncDeque<std::unique_ptr<Message>>& cache
    );
    std::unique_ptr<Message> sendRequest();

    // API communication helpers
    std::string getApiKey(const Message& message) const;
    std::vector<std::string> createRequestHeaders(const Message& message) const;

    // Member variables
    std::vector<std::unique_ptr<Message>> conversation_;
    std::vector<Tool> tools_;
    std::unique_ptr<ITranslator> translator_;
    std::unique_ptr<http_client::IHTTPClient> httpClient_;

    Message defaultMessage;
};
