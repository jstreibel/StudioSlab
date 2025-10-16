//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_EXCEPTION_H
#define STUDIOSLAB_EXCEPTION_H

#include <exception>
#include <utility>
#include "String.h"
#include "Utils.h"

#define NOT_IMPLEMENTED_CLASS_METHOD throw NotImplementedException( \
    Slab::Common::getClassName(this) +                   \
    " (" + Str(__PRETTY_FUNCTION__) + " FROM FILE " + (__FILE__) + ":" + Slab::ToStr(__LINE__) + ")");

#define NOT_IMPLEMENTED throw NotImplementedException( \
        Str(__FILE__) + ":" + ToStr(__LINE__));

class Exception : public std::exception {
private:
    std::string message;
public:
    explicit Exception(std::string  msg);

    [[nodiscard]] const char* what() const noexcept override;
};

class NotImplementedException : public Exception {
public:
    explicit NotImplementedException(std::string who);
};

class StudioSlabRuntimeBackendInconsistency : public Exception {
private:
public:
    explicit StudioSlabRuntimeBackendInconsistency(std::string why);
};

#endif //STUDIOSLAB_EXCEPTION_H
