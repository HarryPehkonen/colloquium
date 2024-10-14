#pragma once
#include "translator/itranslator.hpp"
#include "core/message.hpp"
#include "core/tool.hpp"
#include <memory>
#include <vector>

class LLMClient {
public:
    LLMClient(std::unique_ptr<ITranslator> translator);
    
    void addMessage(std::unique_ptr<Message> message);
    void addTool(Tool tool);
    std::unique_ptr<Message> sendRequest();

private:
    std::vector<std::unique_ptr<Message>> conversation_;
    std::vector<Tool> tools_;
    std::unique_ptr<ITranslator> translator_;
};
