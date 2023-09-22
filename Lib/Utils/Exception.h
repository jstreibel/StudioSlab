//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_EXCEPTION_H
#define STUDIOSLAB_EXCEPTION_H

#include <exception>
#include <utility>
#include <string>


class Exception : public std::exception {
private:
    std::string message;
public:
    explicit Exception(std::string  msg);

    const char* what() const noexcept override;
};

#endif //STUDIOSLAB_EXCEPTION_H
