// src/core/message.hpp
#pragma once
#include <optional>
#include <string>

class Message {
public:
    enum class Type { System, User, Assistant, ToolCall, ToolResult };
    
    Message(Type type);
    virtual ~Message() = default;
    
    Type getType() const;
    
    std::string content;
    std::optional<std::string> created;
    std::optional<std::string> finish_reason;
    std::optional<int> prompt_tokens;
    std::optional<int> completion_tokens;
    std::optional<int> total_tokens;
    std::optional<std::string> model;
    std::optional<double> temperature;
    std::optional<double> top_p;
    std::optional<int> max_tokens;
    std::optional<int> logprobs;
    std::optional<bool> stream;
    std::optional<int> random_seed;
    std::optional<std::string> response_format_type;

protected:
    Type type_;
};
