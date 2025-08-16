/*
 * scatterParams.h
 *
 *  Created on: 21 de set de 2017
 *      Author: johnny
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include "../../../../Core/Controller/InterfaceOwner.h"
#include "../../../../Core/Controller/CommonParameters.h"
#include "Utils/Types.h"
#include "Utils/NativeTypes.h"
#include "Utils/List.h"

/* DEFAULTS */


namespace Slab::Math {

    class FNumericConfig : public Core::FInterfaceOwner {
    public:
        explicit FNumericConfig(bool do_register = true);

        FNumericConfig(const FNumericConfig &) = delete;

        virtual
        auto getn() const -> UInt = 0;

        virtual
        auto to_string() const -> Str = 0;
    };


}

#endif /* PARAMS_H_ */

