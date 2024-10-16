#pragma once
#include <stdexcept>
#include <string>

class TranslationException : public std::runtime_error {
public:
    explicit TranslationException(const std::string& message) : std::runtime_error(message) {}
};
