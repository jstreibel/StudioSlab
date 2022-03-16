//
// Created by joao on 22/07/2021.
//

#ifndef ISING_QUENCHVIEWCONTROLLER_H
#define ISING_QUENCHVIEWCONTROLLER_H

#include "../../IsingMonteCarlo/IsingNetwork.h"
#include "../../Utils/ThermoUtils.h"
#include "../Tools/GraphAndAverageCalc.h"
#include "../ViewControlBase.h"
#include <SFML/Graphics.hpp>

#include "../../IsingMonteCarlo/IsingMonteCarloCalculator.h"

namespace ThermoOutput {
    class QuenchViewController : public ViewControlBase {
        double T;
        int MCStep;

        const int L;
        const int MCSteps;
        const int tau_eq;
        const int tau_corr;
        const int total_measures;
        const int SQRT_SIMS;

        bool running = true;

        sf::RenderWindow window;
        sf::Font font;
        sf::Text text;

        Graph *e_t_view;
        Graph *m_t_view;
        Graph *T_t_view;

        std::vector<ThermoUtils::Real> av_e, av_e2, av_m, av_m2, av_m4, T_values;

        typedef std::pair<IsingMonteCarloCalculator *, sf::Texture *> ModelViewDataPair;

        sf::Image helperBitmap;
        std::vector<sf::Drawable *> drawables;
        std::vector<ModelViewDataPair> simulations;
        std::vector<std::vector<OutputData *>> histories;
    public:
        explicit QuenchViewController(int L, int MCSteps, int tau_eq, int tau_corr, int sqrtNSims);

        void run();

        bool doOperate(SystemParams &params, OutputData &data) override;

        void hide();

    private:
        void _stepSims();

        void __updateIsingGraph(const IsingNetwork &S, sf::Texture &networkTexture);

        void _treatEvents();

        auto _runProgram() -> bool;

        void _drawEverything();

        void _dumpData();

        sf::Clock timer;
    };
}


#endif //ISING_QUENCHVIEWCONTROLLER_H
