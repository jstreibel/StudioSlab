//
// Created by joao on 18/08/23.
//

#include "HistoryDisplay.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Core/Tools/Log.h"

#define min(a, b) ((a)<(b)?(a):(b))

#define CHECK_GL_ERRORS(count) OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");

Graphics::HistoryDisplay::HistoryDisplay(Str name, Real phiMin, Real phiMax)
: Graphics::FlatFieldDisplay(name) {

}

void Graphics::HistoryDisplay::set_t(Real t_) {
    if(t_ == lastUpdatedTime) return;

    //auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);
//
    //fix n = discreteFunc.getM();
    //fix t₀ = discreteFunc.getDomain().yMin;
    //fix tₘₐₓ = discreteFunc.getDomain().yMax;
    //fix t = min(t_, tₘₐₓ);
//
    //auto upToRow = (Count)(Real(n-1) * (t-t₀)/(tₘₐₓ-t₀));
//
    //assert(upToRow < n);

    NOT_IMPLEMENTED
    //const auto N = discreteFunc.getN();
    //for(auto i=0; i<N; ++i)
    //    for(auto j=nextRow; j<=upToRow; ++j)
    //        textureData->setValue(i,j, (Real32)discreteFunc.At(i, j));
//
    //fix totalRows = upToRow-nextRow+1;
    //if(totalRows<1) return;
    //if(!textureData->upload(nextRow, totalRows))
    //{
    //    Log::Error("Graph " + this->title + " failed uploading texture from row ") << nextRow << " up to row " << upToRow << ", both inclusive'"
    //                                                                   << " which implies a total of " << totalRows
    //                                                                   << " rows @ t=" << t
    //                                                                   << " with Δt=" << (t-lastUpdatedTime) << Log::Flush;
//
    //}

    // nextRow = upToRow+1;
    // lastUpdatedTime = t_;
}


