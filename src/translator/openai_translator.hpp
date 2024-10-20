// src/translators/openai_translator.hpp
#pragma once
#include "itranslator.hpp"
#include <nlohmann/json.hpp>

class OpenAITranslator : public ITranslator {
public:
    std::unique_ptr<Message> responseToMessage(const std::string& json) const override;
    std::string messageToJSON(const Message& message) const override;
    std::string toolToJSON(const Tool& tool) const override;
    std::string createRequest(const std::vector<std::unique_ptr<Message>>& messages,
                                  const std::vector<Tool>& tools) const override;
    
    std::string parameterToJSON(const Parameter& param) const;

private:
    nlohmann::json parameterToJSONObject(const Parameter& param) const;
    nlohmann::json toolToJSONObject(const Tool& tool) const;
    nlohmann::json messageToJSONObject(const Message& message) const;
};
