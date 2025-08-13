//
// Created by joao on 17/05/2021.
//

#include <ostream>
#include <fstream>
#include <boost/range/combine.hpp>

#include "SingleSimViewController.h"

#include "Graphics/Styles/Colors.h"
#include "Graphics/Modules/Nuklear/NuklearModule.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Core/SlabCore.h"
#include "Core/Tools/Resources.h"
#include "Graphics/SlabGraphics.h"


#define COMPUTE_AVERAGES false


namespace Slab::Lost::ThermoOutput {

#if FULLSCREEN == true
    const int WinW=3840, WinH=2160;
    auto style=sf::Style::Fullscreen;
#else
    const int WinW=2700, WinH=2000;
    auto style=sf::Style::Default;
#endif
    // View
    const int Cols = 3;
    const int Rows = 5;
    const int fontSize = 28;
    const float _border = 1.2f*fontSize;
    const float isingSpriteSize = (WinH- 3 * _border) / 2; //2l + 3b = H => 2l = H-3b => l = (H-3b)/2
    const int _graphsWidth = int(WinW-isingSpriteSize- (Cols+1+1) * _border) / Cols;
    const int _graphsHeight = int(WinH- (Rows+1) * _border) / Rows;
    DevFloat GAP = .0; // in degrees, used to view the Dirac strings between field twists.

    // user input parameters to manip T and h.
    const float delta = .01;
    const float TMin = 0, TMax = 5;
    const float hRange = 1.5;

    // Program:
    const float T_min=(float)T_c-0.8816f, T_max = (float)T_c+0.5f;
    const int transientToSamplesRatio=100;

    // Output
    const std::string fileLoc = "/home/joao/Data/StudioSlab/Ising/";

    SingleSimViewController::SingleSimViewController(const int L, int MCSteps, int transientSize)
        : MCSteps(MCSteps), transientSize(transientSize)
    {
        auto main_syswin = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
        auto nkModule_abstract = Core::GetModule("Nuklear");
        auto &nkModule = *dynamic_cast<Graphics::NuklearModule*>(nkModule_abstract.get());
        nkContext = nkModule.CreateContext(main_syswin);
        timer.restart();

        const float isingSpriteScale = isingSpriteSize/(float)L;

        if (!font.loadFromFile(Slab::Core::Resources::GetIndexedFontFileName(10)))
            throw "SFML error while loading font.";

        text = sf::Text("Ising", font, fontSize);
        text.setPosition(2 * _border + isingSpriteSize, _border);

        {
            XYThetaBitmap.create(L, L, sf::Color::Blue);
            XYThetaTexture.loadFromImage(XYThetaBitmap);
            XYThetaTexture.generateMipmap();
            XYThetaSprite.setTexture(XYThetaTexture, true);
            XYThetaSprite.setScale(isingSpriteScale, isingSpriteScale);
            XYThetaSprite.setPosition(_border, _border);
        }
        {
            XYEnergyBitmap.create(L, L, sf::Color::Blue);
            XYEnergyTexture.loadFromImage(XYEnergyBitmap);
            XYEnergyTexture.generateMipmap();
            XYEnergySprite.setTexture(XYEnergyTexture, true);
            XYEnergySprite.setScale(isingSpriteScale, isingSpriteScale);
            XYEnergySprite.setPosition(_border, 2 * _border + isingSpriteSize);
        }


        sf::IntRect subWindow((int) (2 * _border + 2 * isingSpriteSize), (int)_border, _graphsWidth, _graphsHeight);

        float xMin=0, xMax=(float)MCSteps; u_char gray=.4*255; sf::Color dashColor(gray, gray, gray);
        auto *_mag_t_view = new GraphAndAverageCalc(subWindow, {xMin, -1.1}, {xMax, 1.1}, MCSteps, transientSize, "t", "m", true);
        mag_t_View = _mag_t_view;
        mag_t_View->addHorizontalDashedLine( 1, 200, dashColor);
        mag_t_View->addHorizontalDashedLine(-1, 200, dashColor);

        subWindow.top += (int)_border + subWindow.height;
        auto *_en_t_View = new GraphAndAverageCalc(subWindow, {xMin, -5}, {xMax, 0.2}, MCSteps, transientSize, "t", "e", false);
        en_t_View = _en_t_View;
        en_t_View->addHorizontalDashedLine(-1, 200, dashColor);
        en_t_View->addHorizontalDashedLine(-2, 200, dashColor);
        en_t_View->addHorizontalDashedLine(-3, 200, dashColor);
        en_t_View->addHorizontalDashedLine(-4, 200, dashColor);
        en_t_View->addVerticalDashedLine((float)transientSize);


        subWindow.top += (int)_border + subWindow.height;
        auto *_corr_t_View = new Graph(subWindow, {xMin, -1.1}, {xMax, 1.1}, "t", "C(t)/C(0)");
        corr_t_View = _corr_t_View;
        corr_t_View->addHorizontalDashedLine(-1.f, 200, dashColor);
        corr_t_View->addHorizontalDashedLine(-2.f, 200, dashColor);
        corr_t_View->addHorizontalDashedLine(-3.f, 200, dashColor);
        corr_t_View->addHorizontalDashedLine(-4.f, 200, dashColor);
        corr_t_View->addVerticalDashedLine((float)transientSize);


        subWindow.top += int(_border + subWindow.height);
        T_t_View = new Graph(subWindow, {xMin, 0}, {xMax, 5.5}, "t", "T");
        T_t_View->addHorizontalDashedLine((float)T_c, 100, sf::Color::Magenta);
        T_t_View->addHorizontalDashedLine(5, 200, dashColor);
        T_t_View->addHorizontalDashedLine(4, 200, dashColor);
        T_t_View->addHorizontalDashedLine(3, 200, dashColor);
        T_t_View->addHorizontalDashedLine(2, 200, dashColor);
        T_t_View->addHorizontalDashedLine(1, 200, dashColor);
        T_t_View->addVerticalDashedLine((float)transientSize);

        subWindow.top += int(_border + (float)subWindow.height);
        h_t_View = new Graph(subWindow, {xMin, -hRange}, {xMax, +hRange}, "t", "h");

        subWindow.top = (int)_border;
        subWindow.left += int(_border + (float)subWindow.width);

        //subWindow.top += border+subWindow.height;
        accepted_t_View = new Graph(subWindow, {0, -.1}, {static_cast<float>(MCSteps), 1.1}, "t", "Rejected/total ss change");
        accepted_t_View->addHorizontalDashedLine(1.0);

        /*
        subWindow.top += border+subWindow.height;
        mag_T_View = new Graph(subWindow, {T_min*0.9f, -.1}, {T_max*1.1f, 1.1}, "T", "<|m|>");
        mag_T_View->addVerticalDashedLine(T_c, 100, sf::Color::Magenta);
        mag_T_View->addHorizontalDashedLine(1, 100, dashColor);

        subWindow.top += border + subWindow.height;
        en_T_View =  new Graph(subWindow, {T_min*0.9f, -5.1}, {T_max*1.1f, 1.1}, "T", "<e>");
        en_T_View->addVerticalDashedLine(T_c, 100, sf::Color::Magenta);
        en_T_View->addHorizontalDashedLine( 1, 200, dashColor);
        en_T_View->addHorizontalDashedLine(-1, 200, dashColor);
        en_T_View->addHorizontalDashedLine(-2, 200, dashColor);
        en_T_View->addHorizontalDashedLine(-3, 200, dashColor);
        en_T_View->addHorizontalDashedLine(-4, 200, dashColor);

        subWindow.top += border + subWindow.height;
        chi_T_View =  new Graph(subWindow, {T_min*0.9f, -0.1}, {T_max*1.1f, 10}, "T", "chi");
        chi_T_View->addVerticalDashedLine(T_c, 100, sf::Color::Magenta);
        chi_T_View->addHorizontalDashedLine(1, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(2, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(3, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(4, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(5, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(6, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(7, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(8, 200, dashColor);
        chi_T_View->addHorizontalDashedLine(9, 200, dashColor);
        _mag_t_view->setAvgOutputSubGraph(mag_T_View, chi_T_View);

        subWindow.top += border + subWindow.height;
        C_v_View = new Graph(subWindow, {T_min*0.9f, 0,}, {T_max*1.1f, 2.1}, "T", "C_v");
        C_v_View->addVerticalDashedLine(T_c, 100, sf::Color::Magenta);
        C_v_View->addHorizontalDashedLine(1, 200, dashColor);
        C_v_View->addHorizontalDashedLine(2, 200, dashColor);
        _en_t_View->setAvgOutputSubGraph(en_T_View, C_v_View);



        subWindow.left = border;
        subWindow.top = isingSpriteSize + border + 11*fontSize;
        subWindow.width = isingSpriteSize;
        subWindow.height = WinH - subWindow.top - border;
        histeresisView = new Graph(subWindow, {-1.1f*hRange, -1.1}, {1.1f*hRange, 1.1}, "h", "m");
         */


#if !COMPUTE_AVERAGES
        mag_t_View->toggleComputeAverages();
        en_t_View->toggleComputeAverages();
#endif
    }


    SingleSimViewController::~SingleSimViewController() {
        // TODO deletar todos os views
        delete mag_t_View; // etc.
    }


    void SingleSimViewController::computeTimeCorrelations(int upToMCStep) {
        auto tau_eq = transientSize;
        auto t0 = tau_eq;
        auto t_max = 5*tau_eq < (MCSteps-tau_eq) ? 5*tau_eq : (MCSteps-tau_eq);

        if(upToMCStep <= t0) return;

        auto t_total = upToMCStep - t0;
        auto tCorrMax_local = fmin(t_total, t_max);

        Vector<DevFloat> c((size_t)tCorrMax_local);

        for (auto t=0; t < tCorrMax_local; ++t){
            auto corr = .0;
            auto m = .0;
            auto mt = .0;

            {
                for (int tp = 0; tp < t_total - t; ++tp) {
                    auto _m = fabs(history[t0+tp].m);
                    auto _mt = fabs(history[t0+tp+t].m);

                    corr += (_m*_mt);
                    m += _m;
                    mt += _mt;
                }
            }

            auto samples = (t_total - t);
            c[t] = corr/samples - m/samples * mt/samples;
        }

        {
            auto c0 = c[0];
            corr_t_View->clearData();
            for(auto t=0; t < tCorrMax_local; ++t) {
                auto norm_corr = c[t] / c0;
                corr_t_View->addPoint((float)t, (float)norm_corr);
            }
        }
    }

    void SingleSimViewController::updateGraphsAndData(SystemParams &params, OutputData &data) {
        sf::Color graphColor = sf::Color::White;
        if (currStep > transientSize)
            graphColor = sf::Color::Green;
        auto MCStep = (float)currStep;

        mag_t_View->addPoint(MCStep, (float)data.m, graphColor);
        en_t_View->addPoint(MCStep, (float)data.e, graphColor);
        T_t_View->addPoint(MCStep, (float)params.T, sf::Color::White);
        h_t_View->addPoint(MCStep, (float)params.h, sf::Color::White);

        accepted_t_View->addPoint(MCStep, (float)data.rejected / data.N);

        //_computeTimeCorrelations(data.mcStep);
    }

    void SingleSimViewController::updateIsingGraph() {
        const XYNetwork &S = algorithm->getData().S;

        float eMin=100, eMax=-100;
        for(int i=0; i<S.L; ++i) {
            for (int j = 0; j < S.L; ++j) {
                {
                    auto val = S.theta(i, j) / (2 * M_PI);
                    val -= floor(val);
                    val *= 360.0;

                    auto rgbColor = Graphics::hsv2rgb({static_cast<DevFloat>(val), 1, 1});
                    auto color = sf::Color((float)rgbColor.r * 255., (float)rgbColor.g * 255., (float)rgbColor.b * 255.);

                    if(val>(360.0-.5*GAP) || val<(.5*GAP)){ color.r*=0; color.g*=0; color.b*=0; }

                    XYThetaBitmap.setPixel(i, j, color);
                }

                {
                    auto val = (S.e(i, j) + 4.0)/8.0;

                    val = fmin(fmax((1-b)*val + b*val*val, 0), 1);

                    auto color = sf::Color((sf::Uint8)val * 255., (sf::Uint8)val * 255., (sf::Uint8)val * 255.);

                    XYEnergyBitmap.setPixel(i, j, color);
                }
            }
        }
        XYThetaTexture.loadFromImage(XYThetaBitmap);
        XYEnergyTexture.loadFromImage(XYEnergyBitmap);
    }

    void SingleSimViewController::event(const sf::Event &event) {
        if(event.type != sf::Event::KeyPressed) return;


        sf::Keyboard::Key key = event.key.code;

        auto delta = ThermoOutput::delta;

        if(event.key.shift) delta *= 5;
        else if(event.key.control) delta *= .2;

        auto params = algorithm->getParams();
        auto data = algorithm->getData();

        if(key == sf::Keyboard::RBracket){
            manipulationOfParametersHasHappened(params.T, params.h, data.N);
            params.T += delta; if(params.T>TMax) params.T=TMax;
        }
        else if(key == sf::Keyboard::LBracket){
            params.T -= delta; if(params.T<TMin) params.T=TMin;
            manipulationOfParametersHasHappened(params.T, params.h, data.N);
        }
        else if(key == sf::Keyboard::H) outputHistoryToFile((int)sqrt(data.N));
        else if(key == sf::Keyboard::M) outputAveragesHistoryToFile((int) sqrt(data.N));
        else if(key == sf::Keyboard::A) outputHisteresisToFile((int)sqrt(data.N));
        else if(key == sf::Keyboard::P){
            params.h += delta;
            if(params.h>hRange) params.h=hRange;
            else {
                sf::Color color = sf::Color::Green;
                if(event.key.shift) color = sf::Color::Red;
                else if(event.key.control) color = sf::Color(135, 206, 235);
                color.a = 0.7*255;
                histeresisView->addPoint(params.h, data.m, color);
            }
            manipulationOfParametersHasHappened(params.T, params.h, data.N);
        }
        else if(key == sf::Keyboard::O){
            params.h -= delta;
            if(params.h<-hRange) params.h=-hRange;
            else {
                sf::Color color = sf::Color::Green;
                if(event.key.shift) color = sf::Color::Red;
                else if(event.key.control) color = sf::Color(135, 206, 235);
                color.a = 0.7*255;
                histeresisView->addPoint(params.h, data.m, color);
            }
            manipulationOfParametersHasHappened(params.T, params.h, data.N);
        }

        else if(key == sf::Keyboard::S) {
            XYThetaTexture.setSmooth(!XYThetaTexture.isSmooth());
            XYEnergyTexture.setSmooth(!XYEnergyTexture.isSmooth());
        }
        else if(key == sf::Keyboard::Space) params.shouldRun = !params.shouldRun;
    }

    void SingleSimViewController::render(sf::RenderWindow *pWindow) {
        this->window = pWindow;

        algorithm->MCStep();

        auto params = algorithm->getParams();
        auto data = algorithm->getData();

        // if running:
        history.push_back(data);
        updateGraphsAndData(params, data);

        updateIsingGraph();
        drawEverything(params, data);

        auto context = (nk_context*)nkContext->GetContextPointer();
        if(nk_begin(context, "Manips",
                    nk_rect(isingSpriteSize+ 2 * _border, isingSpriteSize, (float)_graphsWidth, (float)_graphsHeight),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)){

            nk_layout_row_begin(context, NK_STATIC, 30, 2);

            auto lw = 60.f;
            std::stringstream ss;

            nk_layout_row_push(context, lw);
            ss.str("");
            ss << "T = " << std::setprecision(3) << params.T;
            nk_label(context, ss.str().c_str(), NK_TEXT_LEFT);

            nk_layout_row_push(context, 0.75f*(float)_graphsWidth - lw);
            static auto fT = (float)params.T;
            if (nk_slider_float(context, 0.f, &fT, 5.f, 0.001f)) {
                params.T = fT;
            }


            nk_layout_row_push(context, lw);
            ss.str("");
            ss << "d = " << std::setprecision(3) << params.δ;
            nk_label(context, ss.str().c_str(), NK_TEXT_LEFT);

            nk_layout_row_push(context, 0.75f*(float)_graphsWidth - lw);
            static auto fδ = (float)params.δ;
            if (nk_slider_float(context, 0.0f, &fδ, 2*M_PI, 0.001f)) {
                params.δ = fδ;
            }


            nk_layout_row_push(context, lw);
            ss.str("");
            ss << "b = " << std::setprecision(3) << b;
            nk_label(context, ss.str().c_str(), NK_TEXT_LEFT);

            nk_layout_row_push(context, 0.75f*(float)_graphsWidth - lw);
            fδ = (float)params.δ;
            if (nk_slider_float(context, -100, &b, 20, 0.001f)) {
                params.δ = fδ;
            }


            nk_layout_row_push(context, 0.75f*(float)_graphsWidth - lw);

            nk_bool shouldNotOverrelax = !params.shouldOverrelax;
            if(nk_checkbox_text(context, "Overrelax", 9, &shouldNotOverrelax))
                params.shouldOverrelax = !shouldNotOverrelax;

            nk_layout_row_end(context);
        }
        nk_end(context);
    }


    void SingleSimViewController::runIOProgram(SystemParams &params, OutputData &data) {
        const int totalMCSteps = MCSteps;
        const int transient = transientSize;
        const int sampleSizeDemand = (transientToSamplesRatio+1)*transient;
        const int totalSamples = totalMCSteps/sampleSizeDemand;

        if(currStep==0) {
            auto lastT = params.T;
            params.T = T_min;
            manipulationOfParametersHasHappened(lastT, params.h, data.N);
            return;
        }

        DevFloat dT = (T_max-T_min)/totalSamples;
        if((currStep % ((transientToSamplesRatio+1 )*transientSize)) == 0){
            auto lastT = params.T;
            params.T += dT;
            manipulationOfParametersHasHappened(lastT, params.h, data.N);
            return;
        }
    }

    void SingleSimViewController::drawEverything(SystemParams &params, OutputData &data) {
        window->draw(XYThetaSprite);
        window->draw(XYEnergySprite);

        std::ostringstream textOutput;
        textOutput << "t=" << currStep << "/" << MCSteps << " MC steps"
                   << "\nTransient@" << transientSize << " MC steps"
                   << "\n" << data.S.L << "x" << data.S.L << " network"
                   << "\nTc=" << T_c
                   << "\nT=" << params.T
                   << "\nh=" << params.h
                   << "\ne=" << data.e
                   << "\nm=" << data.m << "\n"
                   << "\naccepted: " << (data.N - data.rejected) << "/" << int(data.N) << "  (" << std::setprecision(3) << (100*(1-data.rejected/data.N)) << "%)"
                   << "\nrejected: " << data.rejected << "/" << int(data.N) << "  (" << (100*(data.rejected/data.N)) << "%)"
                   << "\n\n"
                   << (params.shouldOverrelax?"Is overrelaxing":"Not overrelaxing");
        text.setString(textOutput.str());

        text.setString("O HAI!");
        window->draw(text);


        // window.draw(*mag_t_View);
        // window.draw(*en_t_View);
        // window.draw(*corr_t_View);
        // window.draw(*T_t_View);
        // window.draw(*h_t_View);

        // window.draw(*accepted_t_View);

        //window.draw(*histeresisView);
        //window.draw(*mag_T_View);
        //window.draw(*en_T_View);
        //window.draw(*chi_T_View);
        //window.draw(*C_v_View);
    }

    void SingleSimViewController::manipulationOfParametersHasHappened(DevFloat last_T, DevFloat last_h, DevFloat N) {
        //mag_t_View->manipulationOfParametersHasHappened(last_T, N/last_T);
        //en_t_View->manipulationOfParametersHasHappened(last_T, N/(last_T*last_T));
    }

    void SingleSimViewController::outputHistoryToFile(int L) {
        std::stringstream ssFileName;
        ssFileName << "MCIsing-" << L << "x" << L;
        auto fileName = ssFileName.str();


        std::ofstream myfile;


        {
            auto mag_t = mag_t_View->getData();
            auto en_t = en_t_View->getData();
            auto T_t = T_t_View->getData();
            auto h_t = h_t_View->getData();

            myfile.open(fileLoc + fileName + ".dat");
            myfile << "# History output\n# t    T(t)    h(t)    e(t)    m(t)\n";
            Pair<DevFloat, double> T, h, e, m;
            for (auto data : boost::combine(T_t, h_t, en_t, mag_t)) {
                boost::tie(T, h, e, m) = data;
                const auto t = T.first; // ou h.first ou e.first ou m.first. Eles devem (!) todos retornar a mesma coisa.
                myfile << t << " " << T.second << " " << h.second << " " << e.second << " " << m.second << "\n";
            }
            myfile.close();
        }


        if(0)
        {
            auto magAv_t = mag_t_View->getAveragesHistory();
            auto enAv_t = en_t_View->getAveragesHistory();

            myfile.open(fileLoc + fileName + "-averages.dat");
            myfile << "# History output\n# t    <e>(t)    <e>(t)+sigma(t)    <e>(t)-sigma(t)    <m>(t)    <m>(t)+sigma(t)    <m>(t)-sigma(t)\n";
            Pair<Pair<double, double>, Pair<double, double>> mag, en;
            for (auto data : boost::combine(magAv_t, enAv_t)) {
                boost::tie(mag, en) = data;
                const auto t = mag.first.first;

                const auto m = mag.first.second;
                const auto mps = mag.second.first;
                const auto mms = mag.second.second;

                const auto e = en.first.second;
                const auto eps = en.second.first;
                const auto ems = en.second.second;

                myfile << t << " " << e << " " << eps << " " << ems << " " << m << " " << mps << " " << mms << "\n";
            }
            myfile.close();
        }


        {
            // with points pointtype 7 pointsize 0.5

            myfile.open(fileLoc + fileName + ".plot");
            myfile << "set terminal qt\n"
                   << "set xlabel \"t\" textcolor rgb \"#00000000\"\n"
                   << "set ylabel \"e(t)\" textcolor rgb \"#00000000\"\n"
                   << "set border lc rgb 'black'\n"
                   << "set key tc rgb 'black'\n"
                   << "plot \"" << (fileName + ".dat") << "\" using 1:4 title \""
                   << L << "x" << L << "\" with lines lc rgb \"#009999ff\"\n"

                   << "set terminal qt 1\n"
                   << "set xlabel \"t\" textcolor rgb \"#00000000\"\n"
                   << "set ylabel \"m(t)\" textcolor rgb \"#00000000\"\n"
                   << "set border lc rgb 'black'\n"
                   << "set key tc rgb 'black'\n"
                   << "plot \"" << (fileName + ".dat") << "\" using 1:5 title \""
                   << L << "x" << L << "\" with lines lc rgb \"#009999ff\"\n";

            myfile.close();
        }
    }

    void SingleSimViewController::outputAveragesHistoryToFile(int L) {
        auto mag_T = mag_T_View->getData();
        auto en_T  = en_T_View->getData();
        auto Cv_T  = C_v_View->getData();
        auto chi_T = chi_T_View->getData();

        std::stringstream fileName;
        fileName << "MCIsing-" << L << "x" << L << "-averages";

        std::ofstream myfile;
        myfile.open (fileLoc + fileName.str() + ".dat");
        myfile << "# History output\n# T    <e>(T)    <m>(t)    Cv(T)    chi(T)\n";
        Pair<double,double> mag, e, Cv, chi;
        for(auto data : boost::combine(mag_T, en_T, Cv_T, chi_T)) {
            boost::tie(mag, e, Cv, chi) = data;
            const auto T = mag.first; // ou e.first ou Cv.first ou chi.first. Eles devem (!) todos retornar a mesma coisa.
            myfile << T << " " << e.second << " " << mag.second << " " << Cv.second << " " << chi.second << "\n";
        }
        myfile.close();

        myfile.open(fileLoc + fileName.str() + ".plot");
        myfile << "set terminal qt\n"
               << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
               << "set ylabel \"<e>(T)\" textcolor rgb \"#00000000\"\n"
               << "set border lc rgb 'black'\n"
               << "set key tc rgb 'black'\n"
               << "plot \"" << (fileName.str() + ".dat") << "\" using 1:2 title \""
               << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n"

               << "set terminal qt 1\n"
               << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
               << "set ylabel \"<m>(T)\" textcolor rgb \"#00000000\"\n"
               << "set border lc rgb 'black'\n"
               << "set key tc rgb 'black'\n"
               << "plot \"" << (fileName.str() + ".dat") << "\" using 1:3 title \""
               << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n"

               << "set terminal qt 2\n"
               << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
               << "set ylabel \"C_v(T)\" textcolor rgb \"#00000000\"\n"
               << "set border lc rgb 'black'\n"
               << "set key tc rgb 'black'\n"
               << "plot \"" << (fileName.str() + ".dat") << "\" using 1:4 title \""
               << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n"

               << "set terminal qt 3\n"
               << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
               << "set ylabel \"chi(T)\" textcolor rgb \"#00000000\"\n"
               << "set border lc rgb 'black'\n"
               << "set key tc rgb 'black'\n"
               << "plot \"" << (fileName.str() + ".dat") << "\" using 1:5 title \""
               << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n";

        myfile.close();
    }

    void SingleSimViewController::outputHisteresisToFile(int L) {
        auto mag_h = histeresisView->getData();

        std::stringstream fileName;
        fileName << "MCIsing-" << L << "x" << L << "-histeresis";

        std::ofstream myfile;
        myfile.open (fileLoc + fileName.str() + ".dat");
        myfile << "# History output\n# h    <m>(h) \n";
        // Pair<double,double> mag, e, Cv, chi;
        for(auto mag : mag_h) myfile << mag.first << " " << mag.second << "\n";
        myfile.close();

        myfile.open(fileLoc + fileName.str() + ".plot");
        myfile << "set terminal qt\n"
               << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
               << "set ylabel \"<m>(h)\" textcolor rgb \"#00000000\"\n"
               << "set border lc rgb 'black'\n"
               << "set key tc rgb 'black'\n"
               << "plot \"" << (fileName.str() + ".dat") << "\" using 1:2 title \""
               << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n";

        myfile.close();
    }

    void SingleSimViewController::setAlgorithm(XYMetropolisAlgorithm *pAlgorithm) {
        this->algorithm = pAlgorithm;
    }


}
