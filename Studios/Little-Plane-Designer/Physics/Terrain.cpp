//
// Created by joao on 11/3/25.
//

#include "Terrain.h"

#include <algorithm>
#include <boost/fusion/algorithm/query/count.hpp>

#include "Graphics/OpenGL/WriterOpenGL.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Math/Function/R2toR/Model/Operators/Integral.h"

Slab::Vector<b2Vec2> FTerrainDescriptor::GetPoints() const {
    const auto dt = (tMax - tMin) / static_cast<float>(Count);

    Slab::Vector<b2Vec2> Points(Count);
    for (int i = 0; i < Count; ++i){
        const float t = tMax - i * dt;   // right → left
        Points[i] = (b2Vec2){ x(t), H(t) };
    }

    return Points;
}

FTerrain::FTerrain() : ChainId() { }

void FTerrain::Setup(const b2WorldId World, const FTerrainDescriptor& Descriptor)
{
    if (b_IsInitialized) b2DestroyChain(ChainId);

    const auto TerrainPoints = Descriptor.GetPoints();

    b2ChainDef ChainDef = b2DefaultChainDef();
    ChainDef.points = &TerrainPoints[0];
    ChainDef.count = TerrainPoints.size();
    ChainDef.isLoop = false;

    b2BodyDef BodyDef = b2DefaultBodyDef();
    BodyDef.type = b2_staticBody;
    BodyDef.position = (b2Vec2){0.0f, 0.0f};
    b2BodyId BodyId = b2CreateBody(World, &BodyDef);

    ChainId = b2CreateChain(BodyId, &ChainDef);

    {
        Slab::Math::Point2DVec MeshTris(Descriptor.Count*6);
        Slab::Math::Point2DVec SurfLine(Descriptor.Count);
        for (int k = 0; k < Descriptor.Count-1; ++k) {
            constexpr float k_GroundHeight = -10.0f;

            const int i = k * 6;

            const auto [x0, y0] = Descriptor.GetPoints()[k];
            const auto [x1, y1] = Descriptor.GetPoints()[k + 1];

            SurfLine[k] = {x0, y0};

            MeshTris[i]   = {x0, y0};
            MeshTris[i+1] = {x0, k_GroundHeight};
            MeshTris[i+2] = {x1, y1};

            MeshTris[i+3] = {x1, y1};
            MeshTris[i+4] = {x0, k_GroundHeight};
            MeshTris[i+5] = {x1, k_GroundHeight};

        }
        SurfLine[Descriptor.Count-1] = {Descriptor.GetPoints()[Descriptor.Count-1].x, Descriptor.GetPoints()[Descriptor.Count-1].y};
        MeshPoints = Slab::New<Slab::Math::FPointSet>(MeshTris);
        SurfPoints = Slab::New<Slab::Math::FPointSet>(SurfLine);
    }

    b_IsInitialized = true;
}

void FTerrain::Draw() {
    if (!b_IsInitialized) return;

    using namespace Slab;

    const Graphics::PlotStyle GroundStyle{
        Graphics::DarkGrass,
        Graphics::Triangles,
        false,
    };

    Graphics::PlotStyle SurfStyle{
        Graphics::GrassGreen,
        Graphics::LineStrip,
        false,
    };
    SurfStyle.thickness = 3.0f;

    Graphics::OpenGL::Legacy::RenderPointSet(MeshPoints, GroundStyle);

    Graphics::OpenGL::Legacy::RenderPointSet(SurfPoints, SurfStyle);
}