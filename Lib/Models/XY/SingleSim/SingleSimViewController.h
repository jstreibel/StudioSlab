//
// Created by joao on 17/05/2021.
//

#ifndef ISING_SINGLESIMVIEWCONTROLLER_H
#define ISING_SINGLESIMVIEWCONTROLLER_H

#include "Utils/Types.h"

#include "Graphics/Backend/SFML/SFMLListener.h"
#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/SFML/Tools/GraphAndAverageCalc.h"

#include "Math/Thermal/ThermoUtils.h"

#include "3rdParty/NuklearInclude.h"

#include "Math/Thermal/XYNetwork.h"
#include "Math/Thermal/ViewControlBase.h"
#include "Math/Numerics/Metropolis/XYMetropolisAlgorithm.h"
#include "Math/Thermal/ThermoUtils.h"

#include <SFML/Graphics.hpp>


namespace Slab::Lost::ThermoOutput {
    const double T_c = 0.8935;

    class SingleSimViewController : public SFMLListener {
        sf::Font font;
        sf::Text text;

        sf::Image XYThetaBitmap;
        sf::Texture XYThetaTexture;
        sf::Sprite XYThetaSprite;

        sf::Image XYEnergyBitmap;
        sf::Texture XYEnergyTexture;
        sf::Sprite XYEnergySprite;
        float b = 0;


        GraphAndAverageCalc *mag_t_View,
                            *en_t_View;

        Graph *corr_t_View      = nullptr,
              *T_t_View         = nullptr,
              *h_t_View         = nullptr,
              *accepted_t_View  = nullptr,
              *mag_T_View       = nullptr,
              *en_T_View        = nullptr,
              *chi_T_View       = nullptr,
              *C_v_View         = nullptr,
              *histeresisView   = nullptr;

        int MCSteps;
        int transientSize;
        int currStep = 0;
        Vector<ThermoOutput::OutputData> history;

        std::shared_ptr<Graphics::GUIContext> nkContext = nullptr;
        sf::RenderWindow *window = nullptr;
        XYMetropolisAlgorithm *algorithm = nullptr;

    public:

        explicit SingleSimViewController(int L, int MCSteps, int transientSize);
        ~SingleSimViewController();

        void event(const sf::Event &event) override;

        void render(sf::RenderWindow *window) override;

        void setAlgorithm(XYMetropolisAlgorithm *algorithm);

    private:
        void computeTimeCorrelations(int upToMCStep);
        void updateGraphsAndData(SystemParams &params, OutputData &data);
        void updateIsingGraph();

        sf::Clock timer;
        virtual void runIOProgram(SystemParams &params, OutputData &data);
        void manipulationOfParametersHasHappened(DevFloat newValue, DevFloat lastValue, DevFloat N);


        void drawEverything(SystemParams &params, OutputData &data);


        void outputHistoryToFile(int L);
        void outputAveragesHistoryToFile(int L);
        void outputHisteresisToFile(int L);
    };
}


#endif //ISING_SINGLESIMVIEWCONTROLLER_H
