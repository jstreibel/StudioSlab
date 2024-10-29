/*
 * scatterParams.h
 *
 *  Created on: 21 de set de 2017
 *      Author: johnny
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include "Core/Controller/CommandLine/CLInterfaceOwner.h"
#include "Core/Controller/CommandLine/CommonCLParameters.h"
#include "Utils/Types.h"
#include "Utils/NativeTypes.h"
#include "Utils/List.h"

/* DEFAULTS */


namespace Slab::Math {

    class NumericConfig : public Core::CLInterfaceOwner {
    public:
        explicit NumericConfig(bool do_register = true);

        NumericConfig(const NumericConfig &) = delete;

        virtual
        auto getn() const -> UInt = 0;

        virtual
        auto to_string() const -> Str = 0;
    };


}

#endif /* PARAMS_H_ */

