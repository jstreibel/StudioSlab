//
// Created by joao on 22/09/23.
//

#include "Exception.h"

Exception::Exception(std::string msg) : message(std::move(msg)) {}

const char *Exception::what() const noexcept {
    return message.c_str();
}
