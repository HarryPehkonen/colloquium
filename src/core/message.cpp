// src/core/message.cpp
#include "message.hpp"

Message::Message(Type type) : type_(type) {}

Message::Type Message::getType() const {
    return type_;
}
