//
// Created by joao on 9/08/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H

#include "Graphics/Window/SlabWindow.h"
#include "Math/Numerics/OutputChannel.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "MolDynNumericConfig.h"

#define MOLS_HISTORY_SIZE 100

namespace Slab::Models::MolecularDynamics {

    using namespace Slab;

    class FMolecularDynamicsMonitor final : public Math::FOutputChannel, public Graphics::FSlabWindow {

    public:
        enum Model {
            LennardJones,
            SoftDisk
        };

        using Config = MolDynNumericConfig;

        FMolecularDynamicsMonitor(const TPointer<Config>&, Model);

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

    protected:
        auto HandleOutput(const Math::FOutputPacket&) -> void override;

    private:
        sf::CircleShape MoleculeShape;
        sf::Texture MolTexture;
        sf::Image MolRenderedPotential;

        // Vector<sf::Vertex[MOLS_HISTORY_SIZE]> MoleculesHistory;

        CountType N;
        float L;

    };

} // MolecularDynamics

#endif //STUDIOSLAB_MONITOR_H
