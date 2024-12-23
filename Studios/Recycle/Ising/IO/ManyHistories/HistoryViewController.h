//
// Created by joao on 22/07/2021.
//

#ifndef ISING_HISTORYVIEWCONTROLLER_H
#define ISING_HISTORYVIEWCONTROLLER_H

#include "../../IsingMonteCarlo/IsingNetwork.h"
#include "../../Utils/ThermoUtils.h"
#include "../Tools/GraphAndAverageCalc.h"
#include "../ViewControlBase.h"
#include <SFML/Graphics.hpp>

#include "../../IsingMonteCarlo/IsingMonteCarloCalculator.h"

namespace ThermoOutput {
    class HistoryViewController : public ViewControlBase {
        double T;
        double eT; // expected avg e/spin in thermod. equilibrium

        const int L;
        const int MCSteps;
        const int tau_eq;
        int tau_int=-1;
        const int SQRT_SIMS;

        // Program:
        int t_max;
        int MCSTEPS_BETWEEN_FRAMES = 1;

        // Output
        const std::string fileLoc = std::string("/home/joao/Developer/StudiesC++/Ising/Result/Tarefa7/");

        bool running=true;

        sf::RenderWindow window;
        sf::Font font;
        sf::Text text;

        Graph *avg_e_view;

        Graph *avg_t_view_1;
        Graph *ddtavg_t_view_1;

        Graph *avg_t_view_2;
        Graph *ddtavg_t_view_2;

        Graph *time_corr_view;
        Graph *space_corr_est_view;

        typedef Pair<IsingMonteCarloCalculator*, sf::Texture*> ModelViewDataPair;

        sf::Image helperBitmap;
        Vector<sf::Drawable*> drawables;
        Vector<ModelViewDataPair> simulations;
        Vector<Vector<OutputData*>> histories;
    public:
        explicit HistoryViewController(double T, int L, int MCSteps, int tau_eq, int sqrtNSims);

        void run();

        bool doOperate(SystemParams &params, OutputData &data) override;

        void hide();

    private:
        void _stepSims(int &MCStep);
        void _computeTimeCorrelations(int upToMCStep);
        void _computeSpaceCorrelations();

        void __updateIsingGraph(const IsingNetwork &S, sf::Texture &networkTexture);
        void _treatEvents();
        void _drawEverything(int MCStep);

        void _dumpData();

        sf::Clock timer;


        bool realTimeCorrelations=false;
    };
}


#endif //ISING_HISTORYVIEWCONTROLLER_H
