// src/translators/openai_translator.cpp
#include "openai_translator.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <iostream>

/*

{
    "messages": [
        {
            "content": "You are a helpful assistant.",
            "role": "system"
        },
        {
            "content": "What's the weather like today?",
            "role": "user"
        }
    ],
    "model": "ministral-8b-latest",
    "tools": [
        {
            "function": {
                "description": "Get the current weather for a location",
                "name": "get_weather",
                "parameters": {
                    "properties": {
                        "location": {
                            "description": "The city and state, e.g. San Francisco, CA",
                            "type": "string"
                        }
                    },
                    "required": [
                        "location"
                    ],
                    "type": "object"
                }
            },
            "type": "function"
        }
    ]
}
*/


std::string OpenAITranslator::createRequest(
    const std::vector<std::unique_ptr<Message>>& messages,
    const std::vector<Tool>& tools) const {
    nlohmann::json request;
    
    auto lastUserMessageIterator = std::find_if(messages.rbegin(), messages.rend(), 
        [](const auto& msg) { return msg->getType() == Message::Type::User; });

    if (lastUserMessageIterator == messages.rend()) {
        throw llm::TranslationException("No user message found in conversation");
    }
    
    const Message* messageWithSpecs = lastUserMessageIterator->get();

    // Add message-specific parameters
    if (messageWithSpecs->model.has_value()) { request["model"] = *messageWithSpecs->model; }
    if (messageWithSpecs->temperature.has_value()) { request["temperature"] = *messageWithSpecs->temperature; }
    if (messageWithSpecs->top_p.has_value()) { request["top_p"] = *messageWithSpecs->top_p; }
    if (messageWithSpecs->max_tokens.has_value()) { request["max_tokens"] = *messageWithSpecs->max_tokens; }
    if (messageWithSpecs->logprobs.has_value()) { request["logprobs"] = *messageWithSpecs->logprobs; }
    if (messageWithSpecs->stream.has_value()) { request["stream"] = *messageWithSpecs->stream; }
    if (messageWithSpecs->random_seed.has_value()) { request["seed"] = *messageWithSpecs->random_seed; }
    if (messageWithSpecs->response_format_type.has_value()) { 
        request["response_format"]["type"] = *messageWithSpecs->response_format_type; 
    }

    // Add messages array
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
        if (messageWithSpecs->tool_choice.has_value()) {
            request["tool_choice"] = *messageWithSpecs->tool_choice;
        }
    }
    
    return request.dump();
}

std::unique_ptr<Message> OpenAITranslator::responseToMessage(const std::string& json) const {
    try {
        nlohmann::json j = nlohmann::json::parse(json);
        std::string role = j["choices"][0]["message"]["role"];
        std::unique_ptr<Message> message = role == "assistant"
            ? std::make_unique<Message>(Message::Type::Assistant)
            : std::make_unique<Message>(Message::Type::System);

        message->content = j["choices"][0]["message"]["content"];
        message->created = j["created"];
        message->finish_reason = j["choices"][0]["finish_reason"];
        message->model = j["model"];
        message->prompt_tokens = j["usage"]["prompt_tokens"];
        message->completion_tokens = j["usage"]["completion_tokens"];
        message->total_tokens = j["usage"]["total_tokens"];

        // Tool calls processing
        const auto& messageObj = j["choices"][0]["message"];
        if (messageObj.contains("tool_calls") && !messageObj["tool_calls"].is_null()) {
            for (const auto& toolCall : messageObj["tool_calls"]) {
                std::map<std::string, std::string> toolCallMap;
                toolCallMap["id"] = toolCall["id"];
                toolCallMap["name"] = toolCall["function"]["name"];
                toolCallMap["arguments"] = toolCall["function"]["arguments"];
                message->tool_calls.push_back(toolCallMap);
            }
        }

        return message;

    } catch (const nlohmann::json::exception& e) {
        throw llm::TranslationException(e.what());
    }
}

std::string OpenAITranslator::toJSON(const Message& message) const {
    return messageToJSONObject(message).dump();
}

std::string OpenAITranslator::toJSON(const Tool& tool) const {
    return toolToJSONObject(tool).dump();
}

std::string OpenAITranslator::toJSON(const Parameter& param) const {
    return parameterToJSONObject(param).dump();
}

nlohmann::json OpenAITranslator::parameterToJSONObject(const Parameter& param) const {
    nlohmann::json j;
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
    if (!requiredParams.empty()) {
        j["function"]["parameters"]["required"] = requiredParams;
    }

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
            case Message::Type::ToolResult: return "tool";
            default: throw llm::TranslationException("Unknown message type");
        }
    }();
    
    j["content"] = message.content;

    // Only include tool-specific fields for tool results
    if (message.getType() == Message::Type::ToolResult) {
        if (message.tool_call_id) j["tool_call_id"] = *message.tool_call_id;
        if (message.name) j["name"] = *message.name;
    }

    // Include tool calls if present
    if (!message.tool_calls.empty()) {
        j["tool_calls"] = nlohmann::json::array();
        for (const auto& toolCall : message.tool_calls) {
            nlohmann::json toolCallJSON;
            toolCallJSON["id"] = toolCall.at("id");
            toolCallJSON["type"] = "function";
            toolCallJSON["function"]["name"] = toolCall.at("name");
            toolCallJSON["function"]["arguments"] = toolCall.at("arguments");
            j["tool_calls"].push_back(toolCallJSON);
        }
    }

    return j;
}
