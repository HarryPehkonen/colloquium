// src/core/tool.hpp
#pragma once
#include "parameter.hpp"
#include <string>
#include <vector>

class Tool {
public:
    std::string name;
    std::string description;
    std::vector<Parameter> parameters;
};
