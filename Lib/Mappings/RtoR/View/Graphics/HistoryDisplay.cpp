//
// Created by joao on 18/08/23.
//

#include "HistoryDisplay.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "Base/Tools/Log.h"

#define min(a, b) ((a)<(b)?(a):(b));

void RtoR::Graphics::HistoryDisplay::set_t(Real t_) {
    static Count nextRow = 0;
    static Real lastUpdatedTime = -1;

    if(t_ == lastUpdatedTime) return;

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    const auto n = discreteFunc.getM();
    const auto t₀ = discreteFunc.getDomain().yMin;
    const auto tₘₐₓ = discreteFunc.getDomain().yMax;
    const auto t = min(t_, tₘₐₓ);

    auto upToRow = min((Count)( n * (t-t₀)/(tₘₐₓ-t₀)), n-1);

    const auto N = discreteFunc.getN();
    for(auto i=0; i<N; ++i){
        for(auto j=nextRow; j<=upToRow; ++j){
            auto val = discreteFunc.At(i, j);
            texture->setColor(i,j, computeColor(val).inverse());
        }
    }

    fix totalRows = upToRow-nextRow+1;
    texture->upload(nextRow, totalRows);
    Log::Note("Texture uploading from row ") << nextRow << " up to row " << upToRow
                                                         << " a total of " << totalRows
                                                         << " rows @ t=" << t
                                                         << " with Δt=" << (t-lastUpdatedTime) << Log::Flush;

    nextRow = upToRow+1;
    lastUpdatedTime = t_;
}
