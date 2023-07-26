/*
 * scatterParams.h
 *
 *  Created on: 21 de set de 2017
 *      Author: johnny
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include <Base/Controller/Interface/InterfaceOwner.h>
#include <Base/Controller/Interface/CommonParameters.h>
#include <Common/Types.h>
#include "Common/NativeTypes.h"

/* DEFAULTS */

class NumericParams : public InterfaceOwner {
	NumericParams(const NumericParams&) = delete;

	IntegerParameter::Ptr 	N       = IntegerParameter  ::New(1024, "N", "Discretization of space dimensions. "
																		  "Has to be POT for GPU");
	RealParameter::Ptr      L       = RealParameter     ::New(10., "L", "Space length");
	RealParameter::Ptr      xCenter = RealParameter     ::New(.0, "c,xCenter", "Center of simulation space");
	RealParameter::Ptr      t       = RealParameter     ::New(-1, "t", "Max simulation time. If left negative, defaults to L/2. "
																	   "Note that this value can be overriden by simulation.");

	IntegerParameter::Ptr   dimMode = IntegerParameter  ::New(0, "m,mode", "Method to compute space measure:"
																		   "\n\t0: h=L/N (option --h is ignored)"
																		   "\n\t1: L=h*N (option --L is ignored)"
																		   "\n\t2: N=L/h (option --N is ignored)"
																		   "\nNote that mode 2 is not too safe if N is "
																		   "not Natural number.");

	RealParameter::Ptr      h       =  RealParameter    ::New(0.009765625, "h", "Cell 1-measure. Defaults to L/N. This value is "
																				"ignored unless '--mode' is properly set.");
	RealParameter::Ptr      r       =  RealParameter    ::New(.1, "r", "dt/h");

	Real dt;

public:
	NumericParams();

    auto getn() const -> PosInt;
    auto getN() const -> PosInt;
    auto getL() const -> Real;
    auto getxLeft() const -> Real;
    auto gett() const -> Real;
    auto getr() const -> Real;
    auto geth() const -> Real;
    auto getdt() const -> Real;

    void sett(Real tMax) const;

	auto notifyCLArgsSetupFinished() -> void override;

} __attribute__((aligned(64)));

#endif /* PARAMS_H_ */

