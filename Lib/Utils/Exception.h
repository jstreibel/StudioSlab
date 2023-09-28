//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_EXCEPTION_H
#define STUDIOSLAB_EXCEPTION_H

#include <exception>
#include <utility>
#include "String.h"

#define NOT_IMPLEMENTED throw NotImplementedException(Str(__PRETTY_FUNCTION__) + " @ line " + ToStr(__LINE__));

class Exception : public std::exception {
private:
    std::string message;
public:
    explicit Exception(std::string  msg);

    const char* what() const noexcept override;
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
