#pragma once
#include "core/message.hpp"
#include "core/tool.hpp"
#include "translator/translation_exception.hpp"
#include <memory>
#include <string>
#include <vector>

class ITranslator {
public:
    virtual ~ITranslator() = default;
    virtual std::string messageToJSON(const Message& message) const noexcept(false) = 0;
    virtual std::string toolToJSON(const Tool& tool) const noexcept(false) = 0;
    virtual std::string createRequest(const std::vector<std::unique_ptr<Message>>& messages,
                                      const std::vector<Tool>& tools) const noexcept(false) = 0;
    virtual std::unique_ptr<Message> responseToMessage(const std::string& json) const noexcept(false) = 0;
};
