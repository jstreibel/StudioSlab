//
// Created by joao on 17/05/2021.
//

#ifndef ISING_SINGLESIMVIEWCONTROLLER_H
#define ISING_SINGLESIMVIEWCONTROLLER_H

#include "../../Model/XYNetwork.h"
#include "Phys/Thermal/Utils/ThermoUtils.h"
#include "../Tools/GraphAndAverageCalc.h"
#include "../ViewControlBase.h"
#include <Studios/Controller/Nuklear/NuklearSFML.h>

#include <SFML/Graphics.hpp>


namespace ThermoOutput {
    const double T_c = 0.8935;

    class SingleSimViewController : public ViewControlBase {
        sf::RenderWindow window;
        sf::Font font;
        sf::Text text;

        sf::Image XYThetaBitmap;
        sf::Texture XYThetaTexture;
        sf::Sprite XYThetaSprite;

        sf::Image XYEnergyBitmap;
        sf::Texture XYEnergyTexture;
        sf::Sprite XYEnergySprite;
        float b = 0;


        nk_context *nkContext = nullptr;


        GraphAndAverageCalc *mag_t_View,
                            *en_t_View;

        Graph *corr_t_View,
              *T_t_View,
              *h_t_View,
              *accepted_t_View,
              *mag_T_View,
              *en_T_View,
              *chi_T_View,
              *C_v_View,
              *histeresisView;

        int MCSteps;
        int transientSize;
        std::vector<ThermoOutput::OutputData> history;

    public:

        explicit SingleSimViewController(int L, int MCSteps, int transientSize);
        ~SingleSimViewController();

        bool doOperate(SystemParams &params, OutputData &data);

        void hide();

    private:
        void _computeTimeCorrelations(int upToMCStep);
        void _updateGraphsAndData(SystemParams &params, OutputData &data);
        void _updateIsingGraph(const XYNetwork &S);
        void _treatEvents(SystemParams &params, OutputData &data);

        sf::Clock timer;
        virtual void _runIOProgram(SystemParams &params, OutputData &data);
        void __manipulationOfParametersHasHappened(double newValue, double lastValue, double N);


        void _drawEverything(SystemParams &params, OutputData &data);


        void __outputHistoryToFile(int L);
        void __outputAveragesHistoryToFile(int L);
        void __outputHisteresisToFile(int L);
    };
}


#endif //ISING_SINGLESIMVIEWCONTROLLER_H
