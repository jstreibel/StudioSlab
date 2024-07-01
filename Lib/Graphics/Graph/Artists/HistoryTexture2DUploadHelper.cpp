//
// Created by joao on 21/04/24.
//

#include "HistoryTexture2DUploadHelper.h"
#include "Graphics/Graph/Tool/FieldTextureKontraption.h"

#define min(a, b) ((a)<(b)?(a):(b))

namespace Slab::Graphics {

    HistoryTexture2DUploadHelper::HistoryTexture2DUploadHelper(NumericFunction_Ptr history,
                                                               Pointer<FieldTextureKontraption> texture)
    : function(history)
    , textureKontraption(texture)
    {

    }

    void HistoryTexture2DUploadHelper::uploadUpTo(const Real time) {
        if(time == lastUpdatedTime) return;

        fix n = function->getM();
        fix t₀ = function->getDomain().yMin;
        fix tₘₐₓ = function->getDomain().yMax;
        fix t = min(time, tₘₐₓ);

        auto upToRow = (Count)(Real(n-1) * (t-t₀)/(tₘₐₓ-t₀));

        assert(upToRow < n);

        const auto N = function->getN();
        for(auto i=0; i<N; ++i)
            for(auto j=nextRow; j<=upToRow; ++j)
                textureKontraption->setValue(i,j, (Real32)function->At(i, j));

        fix totalRows = upToRow-nextRow+1;
        if(totalRows<1) return;

        if(!textureKontraption->upload(nextRow, totalRows))
        {
            Log::Error( __PRETTY_FUNCTION__ + Str(" failed uploading texture from row ")) << nextRow << " up to row " << upToRow << ", both inclusive'"
                                                                                       << " which implies a total of " << totalRows
                                                                                       << " rows @ t=" << t
                                                                                       << " with Δt=" << (t-lastUpdatedTime) << Log::Flush;

        }

        nextRow = upToRow+1;
        lastUpdatedTime = time;

    }
} // Graphics