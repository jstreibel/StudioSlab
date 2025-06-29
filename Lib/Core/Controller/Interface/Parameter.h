//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_PARAMETER_H
#define STUDIOSLAB_PARAMETER_H

#include "Utils/String.h"

namespace Slab::Core {

    using Id = Str;

    class FParameter {
    public:
        virtual ~FParameter() = default;

        virtual auto GetValueAsString()            const -> Str          = 0;

        virtual auto SetValueFromString(Str value)       -> void         = 0;

        virtual auto GetValueVoid()                const -> const void * = 0;

        template<typename T>
        const T& GetValueAs() const { return *static_cast<const T*>(GetValueVoid()); }

        virtual bool operator==(const Str& Str) const = 0;
    };

} // Slab::Core

#endif //STUDIOSLAB_PARAMETER_H
