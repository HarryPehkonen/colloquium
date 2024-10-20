// src/core/message.cpp
#include "message.hpp"
#include <numeric>
#include <execution>
#include <iostream>
#include <chrono>
#include <ctime>


Message::Message(Type type) : type_(type) {
    auto now = std::chrono::system_clock::now();
    auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    created = seconds_since_epoch;
}

Message::Type Message::getType() const {
    return type_;
}

std::string Message::to_string() {
    return std::string("") +
        "Type: " + [this]() {
            switch (type_) {
                case Type::System: return "system";
                case Type::User: return "user";
                case Type::Assistant: return "assistant";
                case Type::ToolCall: return "tool call";
                case Type::ToolResult: return "tool";
                default: return "unknown";
            }
        }() + "\n" +
        "Content: " + content + "\n" +
        "Created: " + std::to_string(created) + "\n" +
        (completion_tokens.has_value() ? "Completion Tokens: " + std::to_string(completion_tokens.value()) + "\n" : "") +
        (finish_reason.has_value() ? std::string("Finish Reason: ") + finish_reason.value() + "\n" : "") +
        (logprobs.has_value() ? "Logprobs: " + std::to_string(logprobs.value()) + "\n" : "") +
        (max_tokens.has_value() ? "Max Tokens: " + std::to_string(max_tokens.value()) + "\n" : "") +
        (model.has_value() ? "Model: " + model.value() + "\n" : "") +
        (name.has_value() ? "Name: " + name.value() + "\n" : "") +
        (prompt_tokens.has_value() ? "Prompt Tokens: " + std::to_string(prompt_tokens.value()) + "\n" : "") +
        (random_seed.has_value() ? "Random Seed: " + std::to_string(random_seed.value()) + "\n" : "") +
        (response_format_type.has_value() ? "Response Format Type: " + response_format_type.value() + "\n" : "") +
        (stream.has_value() ? "Stream: " + std::to_string(stream.value()) + "\n" : "") +
        (temperature.has_value() ? "Temperature: " + std::to_string(temperature.value_or(-1)) + "\n" : "") +
        (tool_call_id.has_value() ? "Tool Call ID: " + tool_call_id.value() + "\n" : "") +
        (tool_choice.has_value() ? "Tool Choice: " + tool_choice.value() + "\n" : "") +
        (top_p.has_value() ? "Top P: " + std::to_string(top_p.value_or(-1)) + "\n" : "") +
        (total_tokens.has_value() ? "Total Tokens: " + std::to_string(total_tokens.value()) + "\n" : "") +
        (tool_calls.size() != 0 ? "Tool Calls:\n" + std::accumulate(
            tool_calls.begin(),
            tool_calls.end(),
            std::string(""),
            [](std::string acc, const std::map<std::string, std::string>& tool_call) {
                for (const auto& [key, value] : tool_call) {
                    acc += "\t" + key + ": " + value + "\n";
                }
                return acc;
            }
        ) + "\n" +
        "Count: " + std::to_string(tool_calls.size()) + "\n" : "")
    ;
}
