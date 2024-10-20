#include <gtest/gtest.h>
#include "translator/openai_translator.hpp"
#include <nlohmann/json.hpp>

class OpenAITranslatorTest : public ::testing::Test {
protected:
    OpenAITranslator translator;
};

TEST_F(OpenAITranslatorTest, MessageToJSON) {
    Message message(Message::Type::User);
    message.content = "Hello, AI!";
    message.created = 1729376080;

    std::string json = translator.messageToJSON(message);
    auto parsed = nlohmann::json::parse(json);

    EXPECT_EQ(parsed["role"], "user");
    EXPECT_EQ(parsed["content"], "Hello, AI!");
    EXPECT_EQ(parsed["created"], 1729376080);
}

TEST_F(OpenAITranslatorTest, ToolToJSON) {
    Tool tool;
    tool.name = "calculator";
    tool.description = "Perform basic arithmetic operations";

    Parameter num1;
    num1.name = "num1";
    num1.type = "number";
    num1.description = "First number";

    Parameter num2;
    num2.name = "num2";
    num2.type = "number";
    num2.description = "Second number";

    Parameter operation;
    operation.name = "operation";
    operation.type = "string";
    operation.description = "Arithmetic operation to perform";
    operation.enum_values = {"+", "-", "*", "/"};
    operation.required = true;

    tool.parameters = {num1, num2, operation};

    std::string json = translator.toolToJSON(tool);
    auto parsed = nlohmann::json::parse(json);

    EXPECT_EQ(parsed["type"], "function");
    EXPECT_EQ(parsed["function"]["name"], "calculator");
    EXPECT_EQ(parsed["function"]["description"], "Perform basic arithmetic operations");
    EXPECT_EQ(parsed["function"]["parameters"]["type"], "object");
    EXPECT_EQ(parsed["function"]["parameters"]["properties"]["num1"]["type"], "number");
    EXPECT_EQ(parsed["function"]["parameters"]["properties"]["num2"]["type"], "number");
    EXPECT_EQ(parsed["function"]["parameters"]["properties"]["operation"]["type"], "string");

    std::vector<std::string> expected_operations = {"+", "-", "*", "/"};
    EXPECT_EQ(parsed["function"]["parameters"]["properties"]["operation"]["enum"].size(), expected_operations.size());
    std::vector<std::string> expected_requireds = {"operation"};
    EXPECT_EQ(parsed["function"]["parameters"]["required"], expected_requireds);
}

TEST_F(OpenAITranslatorTest, CreateRequest) {
    std::vector<std::unique_ptr<Message>> messages;
    messages.push_back(std::make_unique<Message>(Message::Type::System));
    messages.back()->content = "You are a helpful assistant.";
    messages.push_back(std::make_unique<Message>(Message::Type::User));
    messages.back()->content = "What's the weather like?";
    messages.back()->model = "gpt-3.5-turbo";
    messages.back()->temperature = 0.7;

    Tool weatherTool;
    weatherTool.name = "get_weather";
    weatherTool.description = "Get the current weather for a location";
    Parameter location;
    location.name = "location";
    location.type = "string";
    location.description = "The city and state, e.g. San Francisco, CA";
    weatherTool.parameters = {location};

    std::vector<Tool> tools = {weatherTool};

    std::string json = translator.createRequest(messages, tools);
    auto parsed = nlohmann::json::parse(json);

    EXPECT_EQ(parsed["model"], "gpt-3.5-turbo");
    EXPECT_DOUBLE_EQ(parsed["temperature"], 0.7);
    ASSERT_EQ(parsed["messages"].size(), 2);
    EXPECT_EQ(parsed["messages"][0]["role"], "system");
    EXPECT_EQ(parsed["messages"][0]["content"], "You are a helpful assistant.");
    EXPECT_EQ(parsed["messages"][1]["role"], "user");
    EXPECT_EQ(parsed["messages"][1]["content"], "What's the weather like?");
    ASSERT_EQ(parsed["tools"].size(), 1);
    EXPECT_EQ(parsed["tools"][0]["type"], "function");
    EXPECT_EQ(parsed["tools"][0]["function"]["name"], "get_weather");
}

