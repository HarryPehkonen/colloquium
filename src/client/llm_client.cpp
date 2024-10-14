// src/client/llm_client.cpp
#include "llm_client.hpp"

LLMClient::LLMClient(std::unique_ptr<ITranslator> translator)
    : translator_(std::move(translator)) {}

void LLMClient::addMessage(std::unique_ptr<Message> message) {
    conversation_.push_back(std::move(message));
}

void LLMClient::addTool(Tool tool) {
    tools_.push_back(std::move(tool));
}

std::unique_ptr<Message> LLMClient::sendRequest() {
    // Empty implementation for now
    return nullptr;
}
