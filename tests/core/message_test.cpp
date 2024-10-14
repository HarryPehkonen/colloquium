#include <gtest/gtest.h>
#include "core/message.hpp"

TEST(MessageTest, ConstructorAndGetType) {
    Message systemMessage(Message::Type::System);
    EXPECT_EQ(systemMessage.getType(), Message::Type::System);

    Message userMessage(Message::Type::User);
    EXPECT_EQ(userMessage.getType(), Message::Type::User);

    Message assistantMessage(Message::Type::Assistant);
    EXPECT_EQ(assistantMessage.getType(), Message::Type::Assistant);

    Message toolCallMessage(Message::Type::ToolCall);
    EXPECT_EQ(toolCallMessage.getType(), Message::Type::ToolCall);

    Message toolResultMessage(Message::Type::ToolResult);
    EXPECT_EQ(toolResultMessage.getType(), Message::Type::ToolResult);
}

TEST(MessageTest, ContentAndOptionalFields) {
    Message message(Message::Type::User);
    message.content = "Hello, World!";
    message.created = "2023-05-01T12:00:00Z";
    message.model = "gpt-3.5-turbo";
    message.temperature = 0.7;
    message.max_tokens = 100;

    EXPECT_EQ(message.content, "Hello, World!");
    EXPECT_EQ(message.created.value(), "2023-05-01T12:00:00Z");
    EXPECT_EQ(message.model.value(), "gpt-3.5-turbo");
    EXPECT_DOUBLE_EQ(message.temperature.value(), 0.7);
    EXPECT_EQ(message.max_tokens.value(), 100);

    EXPECT_FALSE(message.finish_reason.has_value());
    EXPECT_FALSE(message.prompt_tokens.has_value());
    EXPECT_FALSE(message.completion_tokens.has_value());
    EXPECT_FALSE(message.total_tokens.has_value());
    EXPECT_FALSE(message.top_p.has_value());
    EXPECT_FALSE(message.logprobs.has_value());
    EXPECT_FALSE(message.stream.has_value());
    EXPECT_FALSE(message.random_seed.has_value());
    EXPECT_FALSE(message.response_format_type.has_value());
}

