// src/session/llm_session.cpp
#include "llm_session.hpp"
#include "exceptions/llm_exceptions.hpp"
#include "translator/openai_translator.hpp"
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <iostream>

LLMSession::LLMSession(
    std::unique_ptr<http_client::IHTTPClient> httpClient
) : httpClient_(std::move(httpClient)),
    defaultMessage(Message(Message::Type::System)) {}

void LLMSession::addTool(Tool tool) {
    tools_.push_back(std::move(tool));
}

std::string LLMSession::getApiKey(const Message& message) const {
    spdlog::debug("getApiKey");
    if (!message.api_key_name) {
        throw llm::LLMException("No API_KEY_NAME specified in message");
    }
    
    const char* apiKey = std::getenv(message.api_key_name->c_str());
    if (!apiKey) {
        throw llm::LLMException(*message.api_key_name + " environment variable not set");
    }
    return apiKey;
}

std::vector<std::string> LLMSession::createRequestHeaders(const Message& message) const {
    std::string authHeader = "Authorization: Bearer " + getApiKey(message);
    return {
        "Content-Type: application/json",
        "Accept: application/json",
        authHeader
    };
}

void LLMSession::handleSystemMessage(const std::string& content) {
    spdlog::debug("handleSystemMessage");
    auto message = std::make_unique<Message>(Message::Type::System);
    message->content = content;
    conversation_.push_back(std::move(message));

    // sanity checks
    if (message.get() != nullptr) {
        std::cerr << "Dangling System Message: " << message->content << std::endl;
    }
}

void LLMSession::handleUserMessage(const std::string& content) {
    spdlog::debug("handleUserMessage");
    auto message = std::make_unique<Message>(Message::Type::User);
    defaultMessage.copyTo(*message);
    message->content = content;
    conversation_.push_back(std::move(message));

    // sanity checks
    if (message.get() != nullptr) {
        std::cerr << "Dangling User Message: " << message->content << std::endl;
    }
}

std::unique_ptr<Message> LLMSession::sendRequest() {
    spdlog::debug("sendRequest");
    // use the most recent message's configuration
    // the last message *must* have all the details to make a request
    std::unique_ptr<Message>& lastMessage = conversation_.back();
    
    if (!lastMessage->uri) {
        throw llm::LLMException("No URI specified for API request");
    }

    if (!translator_) {
        throw llm::LLMException("No translator specified");
    }

    std::string requestBody = translator_->createRequest(conversation_, tools_);
    auto headers = createRequestHeaders(*lastMessage);

    spdlog::debug("Sending request to {}", lastMessage->uri.value());
    auto response = httpClient_->Post(lastMessage->uri.value(), requestBody, headers).get();

    if (response.statusCode != 200) {
        throw llm::HTTPException("Received error status code: " + 
            std::to_string(response.statusCode) + "\nResponse body: " + response.body);
    }

    auto responseMessage = translator_->responseToMessage(response.body);
    if (!responseMessage) {
        throw llm::TranslationException("Failed to parse the response.");
    }

    return responseMessage;
}

void LLMSession::processToolCalls(const Message& response, 
    async_deque::AsyncDeque<std::unique_ptr<Message>>& cache) {
    
    auto firstToolCall = response.tool_calls[0];
    for (std::map<std::string, std::string> tool_call : response.tool_calls) {
        std::string toolName = tool_call["name"];
        std::cout << "Tool call: " << toolName << std::endl;
    }
    spdlog::debug("processToolCalls");
    for (const auto& tool_call : response.tool_calls) {
        auto toolNameIt = tool_call.find("name");
        auto toolCallIdIt = tool_call.find("id");
        auto toolArgsIt = tool_call.find("arguments");

        if (toolNameIt == tool_call.end() || toolCallIdIt == tool_call.end() || 
            toolArgsIt == tool_call.end()) {
            throw llm::LLMException("Invalid tool call format");
        }

        auto toolIt = std::find_if(tools_.begin(), tools_.end(),
            [&toolName = toolNameIt->second](const Tool& tool) { 
                return tool.name == toolName; 
            });

        if (toolIt == tools_.end()) {
            throw llm::LLMException("Tool not found: " + toolNameIt->second);
        }

        std::string toolResult = toolIt->function(toolArgsIt->second);

        auto resultMessage = std::make_unique<Message>(Message::Type::ToolResult);
        defaultMessage.copyTo(*resultMessage);
        resultMessage->content = toolResult;
        resultMessage->name = toolNameIt->second;
        resultMessage->tool_call_id = toolCallIdIt->second;
        
        cache.push_back(std::move(resultMessage));

        // sanity checks
        if (resultMessage.get() != nullptr) {
            std::cerr << "Dangling Result Message: " << resultMessage->content << std::endl;
        }
    }
}

void LLMSession::processMessages(Source& source) {
    spdlog::debug("processMessages");
    async_deque::AsyncDeque<std::unique_ptr<Message>> cache;

    while (true) {
        // Process any available messages
        if (!cache.empty()) {
            auto message = std::move(*(cache.pop_front()));
            
            // If it's not a tool result and we still have items in cache,
            // push it back and break - we need to process tool results first
            if (message->getType() != Message::Type::ToolResult && !cache.empty()) {
                cache.push_back(std::move(message));
                break;
            }
            
            conversation_.push_back(std::move(message));
        } else {
            auto prompt = source.get();
            if (!prompt) {
                spdlog::debug("No more prompts to process");
                break;
            }

            // Check for configuration commands
            if (prompt->find("#") == 0) {
                if (prompt->find("#system ") == 0) {
                    handleSystemMessage(prompt->substr(8));
                    continue;
                }
                // Update default configuration
                constexpr std::string_view URI_CMD = "#URI ";
                constexpr std::string_view API_KEY_NAME_CMD = "#API_KEY_NAME ";
                constexpr std::string_view MODEL_CMD = "#MODEL ";
                constexpr std::string_view TRANSLATOR_CMD = "#TRANSLATOR ";
                if (prompt->find(URI_CMD) == 0) {
                    defaultMessage.uri = prompt->substr(URI_CMD.length());
                    spdlog::debug("URI set to {}", defaultMessage.uri.value());
                } else if (prompt->find(API_KEY_NAME_CMD) == 0) {
                    defaultMessage.api_key_name = prompt->substr(API_KEY_NAME_CMD.length());
                    spdlog::debug("API_KEY_NAME set to {}", defaultMessage.api_key_name.value());
                } else if (prompt->find(MODEL_CMD) == 0) {
                    defaultMessage.model = prompt->substr(MODEL_CMD.length());
                    spdlog::debug("MODEL set to {}", defaultMessage.model.value());
                } else if (prompt->find(TRANSLATOR_CMD) == 0) {
                    std::string translatorName = prompt->substr(TRANSLATOR_CMD.length());
                    if (translatorName == "openai") {
                        translator_ = std::make_unique<OpenAITranslator>();
                    } else {
                        throw llm::LLMException("Unknown translator: " + translatorName);
                    }
                    spdlog::debug("Translator set to {}", translatorName);
                }
                continue;
            } else {
                handleUserMessage(*prompt);
            }
        }

        // Send the request with current conversation state
        std::cout << "Request:  " << conversation_.back()->content << std::endl;
        auto responseMessage = sendRequest();
        std::cout << "Response:  " << responseMessage->content << std::endl;
        
        if (responseMessage->tool_calls.empty()) {
            conversation_.push_back(std::move(responseMessage));
        } else {
            // make a copy for conversation
            std::unique_ptr<Message> responseMessageCopy = std::make_unique<Message>(*responseMessage);
            conversation_.push_back(std::move(responseMessageCopy));
            processToolCalls(*responseMessage, cache);

            // sanity checks
            if (responseMessageCopy.get() != nullptr) {
                std::cerr << "Dangling Response Copy: " << responseMessageCopy->content << std::endl;
            }
        }

        // sanity checks
        // no!  responseMessage is declared in the top scope of the loop.
        // if (responseMessage.get() != nullptr) {
        //     std::cerr << "Dangling Response: " << responseMessage->content << std::endl;
        // }
    }
}

const std::vector<std::unique_ptr<Message>>& LLMSession::getConversation() const {
    return conversation_;
}
