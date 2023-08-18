//
// Created by joao on 9/08/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H

#include "Base/Graphics/Window/Window.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>


#define MOLS_HISTORY_SIZE 100

namespace MolecularDynamics {
    class Monitor : public Numerics::OutputSystem::Socket, public Window {
        sf::RenderWindow &renderWindow;

        std::vector<sf::Vertex> molShapes;

        sf::CircleShape molShape;
        sf::Texture molTexture;

        // std::vector<sf::Vertex[MOLS_HISTORY_SIZE]> moleculesHistory;
    public:
        enum Model {
            LennardJones,
            SoftDisk
        };

        Monitor(const NumericConfig &params, Model model);

        void draw() override;

    protected:
        auto handleOutput(const OutputPacket &packet) -> void override;
    };

} // MolecularDynamics

#endif //STUDIOSLAB_MONITOR_H
