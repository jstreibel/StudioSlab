//
// Created by joao on 17/05/2021.
//

#ifndef ISING_SINGLESIMVIEWCONTROLLER_H
#define ISING_SINGLESIMVIEWCONTROLLER_H

#include <SFML/Graphics.hpp>
#include "../../IsingMonteCarlo/IsingNetwork.h"
#include "../../Utils/ThermoUtils.h"
#include "../Tools/GraphAndAverageCalc.h"
#include "../ViewControlBase.h"


namespace ThermoOutput {
    const double T_c = 2.269;

    class SingleSimViewController : public ViewControlBase {
        sf::RenderWindow window;
        sf::Font font;
        sf::Text text;

        sf::Image IsingBitmap;
        sf::Texture IsingTexture;
        sf::Sprite IsingSprite;

        GraphAndAverageCalc *mag_t_View,
                            *en_t_View;

        Graph *corr_t_View,
              *T_t_View,
              *h_t_View,
              *mag_T_View,
              *en_T_View,
              *chi_T_View,
              *C_v_View,
              *histeresisView;
    public:
        explicit SingleSimViewController(int L, int MCSteps, int transientSize);
        ~SingleSimViewController();

        bool doOperate(SystemParams &params, OutputData &data);

        void hide();

    private:
        void _updateGraphsAndData(SystemParams &params, OutputData &data);
        void _updateIsingGraph(const IsingNetwork &S);
        void _treatEvents(SystemParams &params, OutputData &data);

        sf::Clock timer;
        virtual void _runIOProgram(SystemParams &params, OutputData &data);
        void __manipulationOfParametersHasHappened(Real newValue, Real lastValue, Real N);


        void _drawEverything(SystemParams &params, OutputData &data);


        void __outputHistoryToFile(int L);
        void __outputAveragesHistoryToFile(int L);
        void __outputHisteresisToFile(int L);
    };
}


#endif //ISING_SINGLESIMVIEWCONTROLLER_H
