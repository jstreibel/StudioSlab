//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_PARAMETER_H
#define STUDIOSLAB_PARAMETER_H

#include "Utils/String.h"

namespace Slab::Core {

    using Id = Str;

    class Parameter {
    public:
        virtual
        auto getValueAsString() const -> Str = 0;

        virtual
        auto setValueFromString(Str value) -> void = 0;

        virtual
        auto getValueVoid() const -> const void * = 0;

        template<typename T>
        const T& getValueAs() const { return *static_cast<const T*>(getValueVoid()); }
    };

} // Slab::Core

#endif //STUDIOSLAB_PARAMETER_H
