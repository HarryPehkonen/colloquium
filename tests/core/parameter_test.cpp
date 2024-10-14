#include <gtest/gtest.h>
#include "core/parameter.hpp"

TEST(ParameterTest, FieldAssignment) {
    Parameter param;
    param.name = "temperature";
    param.type = "number";
    param.description = "Controls randomness in the output";
    param.enum_values = {"low", "medium", "high"};

    EXPECT_EQ(param.name, "temperature");
    EXPECT_EQ(param.type, "number");
    EXPECT_EQ(param.description, "Controls randomness in the output");
    ASSERT_EQ(param.enum_values.size(), 3);
    EXPECT_EQ(param.enum_values[0], "low");
    EXPECT_EQ(param.enum_values[1], "medium");
    EXPECT_EQ(param.enum_values[2], "high");
}

TEST(ParameterTest, EmptyEnumValues) {
    Parameter param;
    param.name = "max_tokens";
    param.type = "integer";
    param.description = "Maximum number of tokens to generate";

    EXPECT_EQ(param.name, "max_tokens");
    EXPECT_EQ(param.type, "integer");
    EXPECT_EQ(param.description, "Maximum number of tokens to generate");
    EXPECT_TRUE(param.enum_values.empty());
}

TEST(ParameterTest, DefaultConstructor) {
    Parameter p;
    EXPECT_TRUE(p.name.empty());
    EXPECT_TRUE(p.type.empty());
    EXPECT_TRUE(p.description.empty());
    EXPECT_TRUE(p.enum_values.empty());
}

TEST(ParameterTest, FieldAssignment2) {
    Parameter p;
    p.name = "test_param";
    p.type = "string";
    p.description = "A test parameter";
    p.enum_values = {"value1", "value2"};

    EXPECT_EQ(p.name, "test_param");
    EXPECT_EQ(p.type, "string");
    EXPECT_EQ(p.description, "A test parameter");
    EXPECT_EQ(p.enum_values.size(), 2);
    EXPECT_EQ(p.enum_values[0], "value1");
    EXPECT_EQ(p.enum_values[1], "value2");
}

TEST(ParameterTest, CopyConstructor) {
    Parameter p1;
    p1.name = "original";
    p1.type = "int";
    p1.description = "An original parameter";
    p1.enum_values = {"1", "2", "3"};

    Parameter p2 = p1;

    EXPECT_EQ(p2.name, p1.name);
    EXPECT_EQ(p2.type, p1.type);
    EXPECT_EQ(p2.description, p1.description);
    EXPECT_EQ(p2.enum_values, p1.enum_values);
}

TEST(ParameterTest, MoveConstructor) {
    Parameter p1;
    p1.name = "movable";
    p1.type = "float";
    p1.description = "A movable parameter";
    p1.enum_values = {"0.1", "0.2", "0.3"};

    Parameter p2 = std::move(p1);

    EXPECT_EQ(p2.name, "movable");
    EXPECT_EQ(p2.type, "float");
    EXPECT_EQ(p2.description, "A movable parameter");
    EXPECT_EQ(p2.enum_values.size(), 3);
    EXPECT_EQ(p2.enum_values[0], "0.1");
    EXPECT_EQ(p2.enum_values[1], "0.2");
    EXPECT_EQ(p2.enum_values[2], "0.3");
}
