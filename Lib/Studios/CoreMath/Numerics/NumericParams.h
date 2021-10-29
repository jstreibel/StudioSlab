/*
 * scatterParams.h
 *
 *  Created on: 21 de set de 2017
 *      Author: johnny
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include <Studios/Controller/Interface/Interface.h>
#include <Studios/Controller/Interface/CommonParameters.h>
#include <Common/Typedefs.h>
#include <Studios/CoreMath/CoreNativeTypes.h>

/* DEFAULTS */

struct NumericParams : Interface {
	NumericParams();

    auto getn() const -> size_t;
    auto getN() const -> size_t;
    auto getL() const -> Real;
    auto getxLeft() const -> Real;
    auto gett() const -> Real;
    auto getr() const -> Real;
    auto geth() -> Real;
    auto getdt() const -> Real;

private:
    IntegerParameter N = IntegerParameter{1024, "N", "Discretization of space dimensions. Has to be POT for GPU"};
    DoubleParameter L = DoubleParameter{10., "L", "Space length"};
    DoubleParameter xLeft = DoubleParameter{-5., "xLeft", "Left border of simulation space"};
    DoubleParameter t =  DoubleParameter{5., "t", "Total simulation time"};
    DoubleParameter r = DoubleParameter{.1, "r", "dt/h"};

	Real h;
	Real dt;

} __attribute__((aligned(64)));

#endif /* PARAMS_H_ */
