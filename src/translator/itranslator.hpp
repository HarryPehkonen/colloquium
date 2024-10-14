#pragma once
#include "core/message.hpp"
#include "core/tool.hpp"
#include <memory>
#include <string>
#include <vector>

class ITranslator {
public:
    virtual ~ITranslator() = default;
    virtual std::string messageToJSON(const Message& message) const = 0;
    virtual std::string toolToJSON(const Tool& tool) const = 0;
    virtual std::string createRequest(const std::vector<std::unique_ptr<Message>>& messages,
                                      const std::vector<Tool>& tools) const = 0;
};
