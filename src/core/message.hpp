// src/core/message.hpp
#pragma once
#include <optional>
#include <map>
#include <vector>
#include <string>

class Message {
public:
    enum class Type { System, User, Assistant, ToolCall, ToolResult };

    Message(Type type);
    virtual ~Message() = default;

    Type getType() const;

    std::string to_string();

    void copyTo(Message& other);

    std::string content;
    long long created;
    std::optional<std::string> finish_reason;
    std::optional<int> prompt_tokens;
    std::optional<int> completion_tokens;
    std::optional<int> total_tokens;
    std::optional<std::string> model;
    std::optional<std::string> uri;
    std::optional<std::string> api_key_name;
    std::optional<double> temperature;
    std::optional<double> top_p;
    std::optional<int> max_tokens;
    std::optional<int> logprobs;
    std::optional<bool> stream;
    std::optional<int> random_seed;
    std::optional<std::string> response_format_type;
    std::optional<std::string> tool_choice;
    std::optional<std::string> name;
    std::optional<std::string> tool_call_id;
    std::vector<std::map<std::string, std::string>> tool_calls;

protected:
    Type type_;
};
