//
// Created by joao on 10/14/24.
//

#include <iomanip>
#include "Interface.h"
#include "Core/Tools/Log.h"

namespace Slab::Core {

    Interface::Interface(Str name)
            : name(name) {
    }

    Interface::~Interface() {

    }

    auto Interface::getParameters() const -> Vector<CLParameter_constptr> {
        Vector<CLParameter_constptr> constParameters;

        std::copy(parameters.begin(), parameters.end(), std::back_inserter(constParameters));

        return constParameters;
    }

    void Interface::addParameter(CLParameter_ptr parameter) {
        auto insertionSuccessful = parameters.insert(parameter).second;

        if (!insertionSuccessful) {
            throw "Error while inserting parameter in interface.";
        }

        auto quotename = Str("\"") + parameter->getFullCommandLineName() + "\"";
    }
    void Interface::addParameters(std::initializer_list<CLParameter_ptr> parametersList) {
        for (auto param: parametersList)
            addParameter(param);
    }

    auto Interface::getParameter(Str key) const -> CLParameter_ptr {
        auto compareFunc = [key](CLParameter_ptr parameter) {
            return *parameter == key;
        };

        auto result = std::find_if(parameters.begin(), parameters.end(), compareFunc);

        return *result;
    }

    Message Interface::sendRequest(Request req) {
        return {"[unknown request]"};
    }


} // Slab::Core