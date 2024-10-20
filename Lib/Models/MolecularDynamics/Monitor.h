//
// Created by joao on 9/08/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H

#include "Graphics/Window/Window.h"
#include "Math/Numerics/Output/Plugs/Socket.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "MolDynNumericConfig.h"

#define MOLS_HISTORY_SIZE 100

namespace MolecularDynamics {

    using namespace Slab;

    class Monitor : public Math::Socket, public Graphics::Window {
        sf::RenderWindow &renderWindow;

        Vector<sf::Vertex> molShapes;

        sf::CircleShape molShape;
        sf::Texture molTexture;

        // Vector<sf::Vertex[MOLS_HISTORY_SIZE]> moleculesHistory;

        Count N;
        float L;
    public:
        enum Model {
            LennardJones,
            SoftDisk
        };

        using Config = Models::MolecularDynamics::MolDynNumericConfig;

        Monitor(Pointer<Config>, Model);

        void draw() override;

    protected:
        auto handleOutput(const Math::OutputPacket &packet) -> void override;
    };

} // MolecularDynamics

#endif //STUDIOSLAB_MONITOR_H