// src/core/parameter.hpp

#pragma once
#include <string>
#include <vector>

class Parameter {
public:
    std::string name;
    std::string type;
    std::string description;
    bool required = false;
    std::vector<std::string> enum_values;
};
