//
// Created by joao on 20/11/2019.
//

#include "R2ToRInputRadialShockwave.h"

#include "Lib/Fields/Maps/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Lib/Fields/Maps/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Base/Math/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"

#include "Lib/Fields/Maps/RtoR/Model/RtoRBoundaryCondition.h"


R2toR::R2toRInputRadialShockwave::R2toRInputRadialShockwave() : Base::BCInterface({&t0}) { }


void R2toR::R2toRInputRadialShockwave::YoureTheChosenOne(CLVariablesMap &vm) {
    auto *phi = new RtoR::AnalyticShockwave2DRadialSymmetry;
    auto *dPhiDt = new RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB;

    phi->sett(t0.value());
    dPhiDt->sett(t0.value());

    //dPhi = new RtoRMap::BoundaryCondition(new RtoRMap::NullFunction, new RtoRMap::RegularDiracDelta(a, a));
    dPhi = new RtoR::BoundaryCondition(phi, dPhiDt);
    //modelParams.mapping = Allocator::ModelParams::RtoRMap;
    //modelParams.hamiltonian = Allocator::ModelParams::StandardSphericalSymmetry;
}

auto R2toR::R2toRInputRadialShockwave::getGeneralDescription() const -> std::string {
    return String("Radially symmetric shockwave");
}


