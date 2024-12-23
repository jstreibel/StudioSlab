//
// Created by joao on 17/05/2021.
//

#include "SingleSimViewController.h"
#include <SFML/Graphics.hpp>
#include <ostream>
#include <fstream>
#include <boost/range/combine.hpp>
#include "../Tools/GraphAndAverageCalc.h"
#include "../../Utils/ViewerUtils.h"


#define COMPUTE_AVERAGES false


namespace ThermoOutput {

    // View
    const int Cols = 3;
    const int Rows = 5;
    const float isingSpriteSize = 700;
    const float border = 35;
    const int fontSize = 20;

    // user input parameters to manip T and h.
    const float delta = .01;
    const float TMin = delta, TMax = 5;
    const float hRange = 1.5;

    // Program:
    const float T_min=T_c-0.5, T_max = T_c+0.5;
    const int transientToSamplesRatio=100;

    // Output
    const std::string fileLoc = "/home/joao/Developer/StudiesC++/Ising/DataAndPlot/Workdir/";


#if FULLSCREEN == true
    const int WinW=3840, WinH=2160;
    auto style=sf::Style::Fullscreen;
#else
    const int WinW=2400, WinH=2400*(9./16);
    auto style=sf::Style::Titlebar;
#endif

    SingleSimViewController::SingleSimViewController(const int L, int MCSteps, int transientSize)
        : window(sf::VideoMode(WinW, WinH), "SFML window", style)
    {
#if FULLSCREEN == false
        window.setPosition(sf::Vector2i(10, 100));
#endif
#if LIMIT_SIM_SPEED == false
        window.setFramerateLimit(1000./FRAME_INTERVAL_MSEC);
#endif

        timer.restart();

        const float isingSpriteScale = isingSpriteSize/L;

        ViewerUtils::LoadFont(font);

        text = sf::Text("Ising", font, fontSize);
        text.setPosition(border, isingSpriteSize + border);


        IsingBitmap.create(L, L);
        for(int i=0; i<L; ++i) for(int j=0; j<L; ++j) IsingBitmap.setPixel(i, j, sf::Color::Blue);
        IsingTexture.loadFromImage(IsingBitmap);
        //IsingTexture.setSmooth(true);
        IsingTexture.generateMipmap();
        IsingSprite.setTexture(IsingTexture, true);
        IsingSprite.setScale(isingSpriteScale, isingSpriteScale);
        IsingSprite.setPosition(border, border);


        sf::IntRect subWindow(2 * border + isingSpriteSize, border, (WinW-isingSpriteSize-(Cols+1+1)*border)/Cols, (WinH-(Rows+1)*border)/Rows);

        float xMin=0, xMax=MCSteps; u_char gray=.4*255; sf::Color dashColor(gray, gray, gray);
        auto *_mag_t_view = new GraphAndAverageCalc(subWindow, {xMin, -1.1}, {xMax, 1.1}, MCSteps, transientSize, "t", "m", true);
        mag_t_View = _mag_t_view;
        mag_t_View->addHorizontalDashedLine( 1, 200, dashColor);
        mag_t_View->addHorizontalDashedLine(-1, 200, dashColor);

        subWindow.top += border+subWindow.height;
        auto *_en_t_View = new GraphAndAverageCalc(subWindow, {xMin, -5}, {xMax, 0.2}, MCSteps, transientSize, "t", "e", false);
        en_t_View = _en_t_View;
        en_t_View->addHorizontalDashedLine(-1, 200, dashColor);
        en_t_View->addHorizontalDashedLine(-2, 200, dashColor);
        en_t_View->addHorizontalDashedLine(-3, 200, dashColor);
        en_t_View->addHorizontalDashedLine(-4, 200, dashColor);


        subWindow.top += border+subWindow.height;
        auto *_corr_t_View = new Graph(subWindow, {xMin, -5}, {xMax, 0.2}, "t", "C(t)/C(0)");
        corr_t_View = _corr_t_View;
        corr_t_View->addHorizontalDashedLine(-1, 200, dashColor);
        corr_t_View->addHorizontalDashedLine(-2, 200, dashColor);
        corr_t_View->addHorizontalDashedLine(-3, 200, dashColor);
        corr_t_View->addHorizontalDashedLine(-4, 200, dashColor);


        subWindow.top += border+subWindow.height;
        T_t_View = new Graph(subWindow, {xMin, 0}, {xMax, 5.5}, "t", "T");
        T_t_View->addHorizontalDashedLine(T_c, 100, sf::Color::Magenta);
        T_t_View->addHorizontalDashedLine(5, 200, dashColor);
        T_t_View->addHorizontalDashedLine(4, 200, dashColor);
        T_t_View->addHorizontalDashedLine(3, 200, dashColor);
        T_t_View->addHorizontalDashedLine(2, 200, dashColor);
        T_t_View->addHorizontalDashedLine(1, 200, dashColor);

        subWindow.top += border+subWindow.height;
        h_t_View = new Graph(subWindow, {xMin, -hRange}, {xMax, +hRange}, "t", "h");

        subWindow.top = border;
        subWindow.left += border+subWindow.width;


        //subWindow.top += border+subWindow.height;
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


#if !COMPUTE_AVERAGES
        mag_t_View->toggleComputeAverages();
        en_t_View->toggleComputeAverages();
#endif
    }


    SingleSimViewController::~SingleSimViewController() {
        // TODO deletar todos os views
        delete mag_t_View; // etc.
    }


    bool SingleSimViewController::doOperate(SystemParams &params, OutputData &data) {
        _treatEvents(params, data);

        if(params.shouldRun) {
            if(data.mcStep>0) {
                _updateGraphsAndData(params, data);
                // _runIOProgram(params, data);
            }
        }

#if LIMIT_SIM_SPEED == false
        if(timer.getElapsedTime().asMilliseconds() > FRAME_INTERVAL_MSEC)
#endif
        {
            _updateIsingGraph(data.S);
            window.clear();
            _drawEverything(params, data);
            window.display();

            timer.restart();
        }

        return window.isOpen();
    }

    void SingleSimViewController::_updateGraphsAndData(SystemParams &params, OutputData &data) {
        sf::Color graphColor = sf::Color::White;
        if (data.mcStep > data.transientSize)
            graphColor = sf::Color::Green;
        float MCStep = data.mcStep;
        mag_t_View->addPoint(MCStep, data.m, graphColor);
        en_t_View->addPoint(MCStep, data.e, graphColor);
        T_t_View->addPoint(MCStep, params.T, sf::Color::White);
        h_t_View->addPoint(MCStep, params.h, sf::Color::White);
    }

    void SingleSimViewController::_updateIsingGraph(const IsingNetwork &S) {
        u_char neg = .2*255;
        u_char pos = .8*255;
        auto positiveColor = sf::Color(pos,pos,pos);
        auto negativeColor = sf::Color(neg,neg,neg);
        for(int i=0; i<S.L; ++i)
            for(int j=0; j<S.L; ++j) {
                auto color = S.s(i,j)==1?positiveColor:negativeColor;
                IsingBitmap.setPixel(i, j, color);
            }
        IsingTexture.loadFromImage(IsingBitmap);
    }

    void SingleSimViewController::_treatEvents(SystemParams &params, OutputData &data) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }

            else if(event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;

                auto delta = ThermoOutput::delta;

                if(event.key.shift) delta *= 5;
                else if(event.key.control) delta *= .2;

                if(key == sf::Keyboard::Escape){
                    window.close();
                    exit(0);
                }
                else if(key == sf::Keyboard::RBracket){
                    __manipulationOfParametersHasHappened(params.T, params.h, data.N);
                    params.T += delta; if(params.T>TMax) params.T=TMax;
                }
                else if(key == sf::Keyboard::LBracket){
                    params.T -= delta; if(params.T<TMin) params.T=TMin;
                    __manipulationOfParametersHasHappened(params.T, params.h, data.N);

                }
                else if(key == sf::Keyboard::H){
                    __outputHistoryToFile((int)sqrt(data.N));
                }
                else if(key == sf::Keyboard::M){
                    __outputAveragesHistoryToFile((int) sqrt(data.N));
                }
                else if(key == sf::Keyboard::A){
                    __outputHisteresisToFile((int)sqrt(data.N));
                }
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
                    __manipulationOfParametersHasHappened(params.T, params.h, data.N);
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
                    __manipulationOfParametersHasHappened(params.T, params.h, data.N);
                }
                else if(key == sf::Keyboard::Space){
                    params.shouldRun = !params.shouldRun;
                }
            }
        }
    }

    void SingleSimViewController::_runIOProgram(SystemParams &params, OutputData &data) {
        const int totalMCSteps = data.mcTotalSteps;
        const int transient = data.transientSize;
        const int sampleSizeDemand = (transientToSamplesRatio+1)*transient;
        const int totalSamples = totalMCSteps/sampleSizeDemand;

        if(data.mcStep==0) {
            auto lastT = params.T;
            params.T = T_min;
            __manipulationOfParametersHasHappened(lastT, params.h, data.N);
            return;
        }

        double dT = (T_max-T_min)/totalSamples;
        if((data.mcStep % ((transientToSamplesRatio+1 )*data.transientSize)) == 0){
            auto lastT = params.T;
            params.T += dT;
            __manipulationOfParametersHasHappened(lastT, params.h, data.N);
            return;
        }
    }

    void SingleSimViewController::_drawEverything(SystemParams &params, OutputData &data) {
        window.draw(IsingSprite);

        std::ostringstream textOutput;
        textOutput << "t=" << data.mcStep << "/" << data.mcTotalSteps << " MC steps"
                   << "\nTransient@" << data.transientSize << " MC steps"
                   << "\n" << data.S.L << "x" << data.S.L << " network"
                   << "\nTc=2.269"
                   << "\nT=" << params.T
                   << "\nh=" << params.h
                   << "\ne=" << data.e
                   << "\nm=" << data.m;
        text.setString(textOutput.str());
        window.draw(text);

        window.draw(*mag_t_View);
        window.draw(*en_t_View);
        window.draw(*corr_t_View);
        window.draw(*T_t_View);
        window.draw(*h_t_View);
        window.draw(*histeresisView);
        window.draw(*mag_T_View);
        window.draw(*en_T_View);
        window.draw(*chi_T_View);
        window.draw(*C_v_View);

    }

    void SingleSimViewController::__manipulationOfParametersHasHappened(double last_T, double last_h, double N) {
        mag_t_View->manipulationOfParametersHasHappened(last_T, N/last_T);
        en_t_View->manipulationOfParametersHasHappened(last_T, N/(last_T*last_T));
    }

    void SingleSimViewController::__outputHistoryToFile(int L) {
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
            Pair<double, double> T, h, e, m;
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

    void SingleSimViewController::__outputAveragesHistoryToFile(int L) {
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

    void SingleSimViewController::__outputHisteresisToFile(int L) {
        auto mag_h = histeresisView->getData();

        std::stringstream fileName;
        fileName << "MCIsing-" << L << "x" << L << "-histeresis";

        std::ofstream myfile;
        myfile.open (fileLoc + fileName.str() + ".dat");
        myfile << "# History output\n# h    <m>(h) \n";
        Pair<double,double> mag, e, Cv, chi;
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

    void SingleSimViewController::hide() {
        window.setVisible(false);
    }

}
