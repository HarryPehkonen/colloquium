#pragma once

#include <stdexcept>
#include <string>

namespace llm {

class LLMException : public std::runtime_error {
public:
    explicit LLMException(const std::string& message) : std::runtime_error(message) {}
};

class HTTPException : public LLMException {
public:
    explicit HTTPException(const std::string& message) : LLMException(message) {}
};

class TranslationException : public LLMException {
public:
    explicit TranslationException(const std::string& message) : LLMException(message) {}
};

// Add more specific exceptions as needed

} // namespace llm
