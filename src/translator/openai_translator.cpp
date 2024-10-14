// src/translators/openai_translator.cpp
#include "openai_translator.hpp"
#include <nlohmann/json.hpp>
#include <vector>


std::string OpenAITranslator::messageToJSON(const Message& message) const {
    return messageToJSONObject(message).dump();
}

std::string OpenAITranslator::toolToJSON(const Tool& tool) const {
    return toolToJSONObject(tool).dump();
}

std::string OpenAITranslator::createRequest(
    const std::vector<std::unique_ptr<Message>>& messages,
    const std::vector<Tool>& tools) const {
    nlohmann::json request;
    
    auto it = std::find_if(messages.rbegin(), messages.rend(), 
        [](const auto& msg) { return msg->getType() == Message::Type::User; });

    if (it == messages.rend()) {
        throw std::runtime_error("No user message found in conversation");
    }
    
    const Message* userMessage = it->get();

    // Add optional parameters
    if (userMessage->model.has_value()) { request["model"] = *userMessage->model; }
    if (userMessage->temperature.has_value()) { request["temperature"] = *userMessage->temperature; }
    if (userMessage->top_p.has_value()) { request["top_p"] = *userMessage->top_p; }
    if (userMessage->max_tokens.has_value()) { request["max_tokens"] = *userMessage->max_tokens; }
    if (userMessage->logprobs.has_value()) { request["logprobs"] = *userMessage->logprobs; }
    if (userMessage->stream.has_value()) { request["stream"] = *userMessage->stream; }
    if (userMessage->random_seed.has_value()) { request["seed"] = *userMessage->random_seed; }
    if (userMessage->response_format_type.has_value()) { 
        request["response_format"]["type"] = *userMessage->response_format_type; 
    }

    // Add messages
    nlohmann::json messageArray = nlohmann::json::array();
    for (const auto& message : messages) {
        messageArray.push_back(messageToJSONObject(*message));
    }
    request["messages"] = messageArray;
    
    // Add tools if present
    if (!tools.empty()) {
        nlohmann::json toolArray = nlohmann::json::array();
        for (const auto& tool : tools) {
            toolArray.push_back(toolToJSONObject(tool));
        }
        request["tools"] = toolArray;
    }
    
    return request.dump();
}
std::string OpenAITranslator::parameterToJSON(const Parameter& param) const {
    return parameterToJSONObject(param).dump();
}

nlohmann::json OpenAITranslator::parameterToJSONObject(const Parameter& param) const {
    nlohmann::json j;
    j["name"] = param.name;
    j["type"] = param.type;
    j["description"] = param.description;
    if (!param.enum_values.empty()) {
        j["enum"] = param.enum_values;
    }
    return j;
}

nlohmann::json OpenAITranslator::toolToJSONObject(const Tool& tool) const {
    nlohmann::json j;
    j["type"] = "function";
    j["function"]["name"] = tool.name;
    j["function"]["description"] = tool.description;
    j["function"]["parameters"]["type"] = "object";
    j["function"]["parameters"]["properties"] = nlohmann::json::object();
    for (const auto& param : tool.parameters) {
        j["function"]["parameters"]["properties"][param.name] = parameterToJSONObject(param);
    }

    // Add required parameters
    std::vector<std::string> requiredParams;
    for (const auto& param : tool.parameters) {
        if (param.required) {
            requiredParams.push_back(param.name);
        }
    }
    j["function"]["parameters"]["required"] = requiredParams;

    return j;
}

nlohmann::json OpenAITranslator::messageToJSONObject(const Message& message) const {
    nlohmann::json j;
    j["role"] = [&]() {
        switch (message.getType()) {
            case Message::Type::System: return "system";
            case Message::Type::User: return "user";
            case Message::Type::Assistant: return "assistant";
            case Message::Type::ToolCall: return "tool_call";
            case Message::Type::ToolResult: return "tool_result";
            default: throw std::runtime_error("Unknown message type");
        }
    }();
    j["content"] = message.content;

    // Add optional fields if they are present
    if (message.created) j["created"] = *message.created;
    if (message.finish_reason) j["finish_reason"] = *message.finish_reason;
    if (message.prompt_tokens) j["prompt_tokens"] = *message.prompt_tokens;
    if (message.completion_tokens) j["completion_tokens"] = *message.completion_tokens;
    if (message.total_tokens) j["total_tokens"] = *message.total_tokens;

    return j;
}
