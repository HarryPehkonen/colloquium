#include <gtest/gtest.h>
#include "core/tool.hpp"

TEST(ToolTest, FieldAssignment) {
    Tool tool;
    tool.name = "weather";
    tool.description = "Get weather information for a location";
    
    Parameter location;
    location.name = "location";
    location.type = "string";
    location.required = true;
    location.description = "The city and state, e.g. San Francisco, CA";
    
    Parameter units;
    units.name = "units";
    units.type = "string";
    units.description = "Temperature units";
    units.enum_values = {"celsius", "fahrenheit"};
    units.required = false;

    tool.parameters = {location, units};

    EXPECT_EQ(tool.name, "weather");
    EXPECT_EQ(tool.description, "Get weather information for a location");
    ASSERT_EQ(tool.parameters.size(), 2);
    EXPECT_EQ(tool.parameters[0].name, "location");
    EXPECT_EQ(tool.parameters[1].name, "units");
    EXPECT_EQ(tool.parameters[0].required, true);
    EXPECT_EQ(tool.parameters[1].required, false);
}

TEST(ToolTest, EmptyParameters) {
    Tool tool;
    tool.name = "current_time";
    tool.description = "Get the current time";

    EXPECT_EQ(tool.name, "current_time");
    EXPECT_EQ(tool.description, "Get the current time");
    EXPECT_TRUE(tool.parameters.empty());
}

