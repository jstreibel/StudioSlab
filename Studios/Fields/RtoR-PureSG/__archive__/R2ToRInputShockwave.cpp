//
// Created by joao on 23/09/2019.
//

#include "R2ToRInputShockwave.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h"
#include "Math/Function/R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"

const auto pi = 3.1415926535897932384626;

using namespace Slab::Math::R2toR;

R2toRInputShockwave::R2toRInputShockwave()
: R2toR::Builder("Shockwave", "(2+1)-dim signum-Gordon shockwave") {


    interface->addParameters({eps, theta, E, e, t0});
}

auto R2toRInputShockwave::getBoundary() -> void * {
    const Real a = sqrt((4./3)*pi*(*eps)*(*eps)*(*E));

    let *phi0 = new FunctionAzimuthalSymmetry(new RtoR::NullFunction, 1.0, **e, **theta);
    //auto *dPhiDt0 = new FunctionAzimuthalSymmetry(new RtoRMap::RegularDiracDelta(a, a), sqrt(3./M_PI),e, theta);
    let *dPhiDt0 = new R2toR::R2toRRegularDelta(*eps, a);

    let *initCond = new BoundaryCondition(phi0, dPhiDt0);
    auto dPhi = initCond;

    if(0) // Output da energia nominal.
    {
        //{
        //    Real E = 0.0;
        //    Real N = 4096;
        //    const Real h = 2. * (*eps) / N;
        //    for (Real x = -(*eps); x <= (*eps); x += h) {
        //        for (Real y = -(*eps); y <= (*eps); y += h) {
        //            Real2D r = {x, y};
        //            const Real val = (*dPhiDt0)(r);
        //            E += val * val;
        //        }
        //    }
        //    E *= .5 * h * h;
        //    Log::Info() << "\nCartesianas: E = " << E << Log::Flush;
        //}
    }

    //auto *outGLShockwave = new OutputOpenGLShockwave(getParameters());
    //outGL = outGLShockwave;

    //RtoR2::StraightLine section1, section2;
    //{
    //    //const Real rMin = ModelBuilder::getInstance().getParams().getxLeft();
    //    //const Real rMax = rMin + ModelBuilder::getInstance().getParams().getL();
    //    const Real rMin = -5;
    //    const Real rMax =  5;
    //    const Real2D x0 = {rMin, .0}, xf = {rMax, .0};
    //    Real theta = 0.0;

    //    Rotation R;
    //    R = Rotation(theta + .5 * M_PI);
    //    section1 = RtoR2::StraightLine(R * x0, R * xf);
    //    R = Rotation(theta);
    //    section2 = RtoR2::StraightLine(R * x0, R * xf);
    //}
    //outGLShockwave->addSection(section1, "Section1");// String("theta = ") + std::to_string(theta) + "rad");
    //outGLShockwave->addSection(section2, "Section2");// String("theta = ") + std::to_string(theta) + "rad");

    //if(vm["outputzeros"].as<bool>()){
    //    letc N = GET("N", size_t);
    //    letc outRes = 1024;
    //    OutputChannel *zerosOut = new R2toR::OutputShockwaveZeros((int)N, (int)(outRes>N?N:outRes));
    //    myOutputs.push_back(zerosOut);
    //}

    return dPhi;
}
