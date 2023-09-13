//
// Created by joao on 18/08/23.
//

#include "HistoryDisplay.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Core/Tools/Log.h"

#define min(a, b) ((a)<(b)?(a):(b))

RtoR::Graphics::HistoryDisplay::HistoryDisplay(Str name, Real phiMin, Real phiMax)
: R2toR::Graphics::FlatFieldDisplay(name, phiMin, phiMax) {

}

void RtoR::Graphics::HistoryDisplay::set_t(Real t_) {
    if(t_ == lastUpdatedTime) return;

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    fix n = discreteFunc.getM();
    fix t₀ = discreteFunc.getDomain().yMin;
    fix tₘₐₓ = discreteFunc.getDomain().yMax;
    fix t = min(t_, tₘₐₓ);

    auto upToRow = (Count)( Real(n-1) * (t-t₀)/(tₘₐₓ-t₀));

    assert(upToRow < n);

    const auto N = discreteFunc.getN();
    for(auto i=0; i<N; ++i){
        for(auto j=nextRow; j<=upToRow; ++j){
            auto val = discreteFunc.At(i, j);
            texture->setColor(i,j, computeColor(val));
        }
    }

    fix totalRows = upToRow-nextRow+1;
    texture->upload(nextRow, totalRows);

    if(false)
        Log::Note("Texture uploading from row ") << nextRow << " up to row " << upToRow
                                                             << " a total of " << totalRows
                                                             << " rows @ t=" << t
                                                             << " with Δt=" << (t-lastUpdatedTime) << Log::Flush;

    nextRow = upToRow+1;
    lastUpdatedTime = t_;
}


