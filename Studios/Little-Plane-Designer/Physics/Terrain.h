//
// Created by joao on 11/3/25.
//

#ifndef STUDIOSLAB_TERRAIN_H
#define STUDIOSLAB_TERRAIN_H

#include <functional>

#include "box2d/box2d.h"
#include "../../../Lib/Graphics/Interfaces/IDrawable.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Utils/Arrays.h"

struct FTerrainDescriptor {
    std::function<float(float)> H;
    std::function<float(float)> x = [](const float x) { return x; };
    float tMin = -10.0f;
    float tMax = 10.0f;
    int Count = 30;

    Slab::Vector<b2Vec2> GetPoints() const;
};

class FTerrain final : public Slab::Graphics::IDrawable
{
    public:
    FTerrain();

    void Setup(b2WorldId, const FTerrainDescriptor& Descriptor);

    void Draw(const Slab::Graphics::IDrawProviders&) override;

private:
    bool b_IsInitialized = false;
    b2ChainId ChainId;
    Slab::TPointer<Slab::Math::FPointSet> SurfPoints;
    Slab::TPointer<Slab::Math::FPointSet> MeshPoints;
};


#endif //STUDIOSLAB_TERRAIN_H