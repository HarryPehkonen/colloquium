// src/core/tool.hpp
#pragma once
#include "parameter.hpp"
#include <string>
#include <vector>
#include <functional>

class Tool {
public:
    std::string name;
    std::string description;
    std::vector<Parameter> parameters;
    std::function<std::string(const std::string&)> function;
};
