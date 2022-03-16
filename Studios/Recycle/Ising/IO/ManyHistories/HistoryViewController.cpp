//
// Created by joao on 22/07/2021.
//


#include "HistoryViewController.h"
#include "../../Utils/ViewerUtils.h"
#include <SFML/Graphics.hpp>
#include <ostream>
#include <iostream>
#include <fstream>


namespace ThermoOutput {
    // View
    const bool SHOW_DDT = false;
#if SHOW_DDT == true
    const int COLS = 2;
#else
    const int COLS = 1;
#endif
    const int ROWS = 5;
    const float miniBorder = 8;
    const float border = 16;
    const int fontSize = 12;

    // Dynamic
    const auto dynamic = IsingMonteCarloCalculator::Kawasaki;
    #define FERRO_IC false
    #define PARA_IC true
    #define _SINGLE_IC ((FERRO_IC & !PARA_IC) || (!FERRO_IC & PARA_IC))
    #define _SWEEP_RAND 1
    #define _SWEEP_SEQ 2
    #define DYNAMIC_BEFORE_EQ _SWEEP_RAND
    #define DYNAMIC_AFTER_EQ _SWEEP_RAND
    #define _CHANGE_DYNAMIC_AFTER_TRANSIENT (DYNAMIC_AFTER_EQ != DYNAMIC_BEFORE_EQ)

    #if FULLSCREEN == true
    const int WinW=3840, WinH=2160;
    auto style=sf::Style::Fullscreen;
    #else
    const int WinW=2400, WinH=1500;
    auto style=sf::Style::Titlebar;
    #endif


    /**
     * Builder
     * @param L
     * @param MCSteps
     * @param tau_eq
     * @param sqrtNSims
     */
    HistoryViewController::HistoryViewController(double T, const int L, int MCSteps, int tau_eq, const int sqrtNSims)
            : window(sf::VideoMode(WinW, WinH), "SFML window", style), T(T), MCSteps(MCSteps), tau_eq(tau_eq), L(L), SQRT_SIMS(sqrtNSims)
    {
        #if FULLSCREEN == false
        window.setPosition(sf::Vector2i(200, 100));
        #endif
        #if LIMIT_SIM_SPEED == true
        window.setFramerateLimit(1000./FRAME_INTERVAL_MSEC);
        #endif

        #if USE_LOOKUP_TABLE_FOR_DELTA_E == true
        ThermoUtils::GenerateBoltzmannWeightLookUpTable(T);
        #endif

        timer.restart();

        t_max = 5*tau_eq < (MCSteps-tau_eq) ? 5*tau_eq : (MCSteps-tau_eq);

        eT = ThermoUtils::eT_Onsager_by_T[T];

        const float isingSpriteSize = (WinH-border-(SQRT_SIMS+1)*miniBorder) / SQRT_SIMS;
        const float isingSpriteScale = isingSpriteSize/float(L);

        helperBitmap.create(L, L);
        for(int i=0; i<L; ++i) for(int j=0; j<L; ++j)
            helperBitmap.setPixel(i, j, sf::Color::Blue);

        for(int i=0; i<SQRT_SIMS; ++i) {
            for(int j=0; j<SQRT_SIMS; ++j) {
                auto texture = new sf::Texture;
                texture->loadFromImage(helperBitmap);
                //texture->setSmooth(true);
                texture->generateMipmap();

                auto sprite = new sf::Sprite;
                sprite->setTexture(*texture, true);
                sprite->setScale(isingSpriteScale, isingSpriteScale);
                sprite->setPosition(float(i) * (miniBorder + isingSpriteSize) + border, float(j) * (miniBorder + isingSpriteSize) + border);

                auto ic = (!_SINGLE_IC) ? (j >= SQRT_SIMS / 2 ? IsingMonteCarloCalculator::Paramagnetic  : IsingMonteCarloCalculator::Ferromagnetic)
                                        : (FERRO_IC           ? IsingMonteCarloCalculator::Ferromagnetic : IsingMonteCarloCalculator::Paramagnetic);

                auto sweep = (DYNAMIC_BEFORE_EQ == _SWEEP_SEQ) ? IsingMonteCarloCalculator::Sequential : IsingMonteCarloCalculator::Random;
                auto calc = new IsingMonteCarloCalculator(L, T, .0, this, ic, dynamic, sweep);

                drawables.push_back(sprite);
                simulations.emplace_back(calc, texture);
            }
        }


        auto xPos = (SQRT_SIMS)*(isingSpriteSize+miniBorder) + 2*border;
        auto colWidth = (WinW-xPos-border) / COLS;
        auto rowHeight = (WinH-border) / ROWS;

        sf::IntRect subWindow(xPos, border, colWidth-border, rowHeight);
        float xMin=0, xMax=MCSteps; u_char gray=.4*255; sf::Color dashColor(gray, gray, gray);

        {
            avg_e_view = new Graph(subWindow, {xMin, float(eT*1.1)}, {xMax, 0.1}, "t", "<e>(t)");
            avg_e_view->addHorizontalDashedLine(eT);

            drawables.push_back(avg_e_view);
        }

        {
            subWindow.top += rowHeight + border;

            avg_t_view_1 = new Graph(subWindow, {xMin, -1.1}, {xMax, 1.1}, "t", "<m>(t)");
            drawables.push_back(avg_t_view_1);
            avg_t_view_1->addVerticalDashedLine(tau_eq);
            avg_t_view_1->addHorizontalDashedLine(1);

            if(!_SINGLE_IC) {
                avg_t_view_2 = new Graph(subWindow, {xMin, -1.1}, {xMax, 1.1}, "", "");
                drawables.push_back(avg_t_view_2);
            }
        }

        if(SHOW_DDT)
        {
            subWindow.top += 400 + 2*border;

            ddtavg_t_view_1 = new Graph(subWindow, {xMin, -0.05}, {xMax, 0.01}, "t", "d<m>(t)/dt");
            drawables.push_back(ddtavg_t_view_1);

            if(!_SINGLE_IC) {
                ddtavg_t_view_2 = new Graph(subWindow, {xMin, -0.05}, {xMax, 0.01}, "", "");
                drawables.push_back(ddtavg_t_view_2);
            }
        }

        {
            if(SHOW_DDT) {
                subWindow.top = border;
                subWindow.left += colWidth + border;
                subWindow.height *= 2;
                subWindow.height += 2 * border;
            } else {
                subWindow.top += rowHeight + border;
            }

            time_corr_view = new Graph(subWindow, {0, -1.0}, {float(t_max), 2.0}, "t", "C(t)/C(0)");
            time_corr_view->addHorizontalDashedLine(1);

            drawables.push_back(time_corr_view);
        }

        {
            subWindow.top += rowHeight + border;

            space_corr_est_view = new Graph(subWindow, {0, -1.0}, {float(MCSteps), 2.0}, "t", "R_e(t)");

            drawables.push_back(space_corr_est_view);
        }

        {
            subWindow.top += rowHeight + border;

            ViewerUtils::LoadFont(font);

            text = sf::Text("Ising", font, fontSize);
            text.setPosition(subWindow.left, subWindow.top);

            histories.resize(SQRT_SIMS * SQRT_SIMS);
        }
    }


    void HistoryViewController::_stepSims(int &MCStep) {
        #if _CHANGE_DYNAMIC_AFTER_TRANSIENT
        if(MCStep == tau_eq)  {
            std::cout << "\n# Changing sweep mode @ MCS "<< MCStep << ".\n";
            auto dynamic = (DYNAMIC_AFTER_EQ==_SWEEP_RAND) ? IsingMonteCarloCalculator::Random : IsingMonteCarloCalculator::Sequential;
            for (auto sim : simulations) sim.first->sweeping = dynamic;
        }
        #endif

        ++MCStep;

        auto N = L * L;
        ThermoUtils::Real mag_ferro = .0, mag_para = .0;
        ThermoUtils::Real e = .0;
        int i=0;
        for (auto sim : simulations) {
            auto calc = sim.first;
            const IsingNetwork &S = calc->getS();

            calc->MCStep();

            auto data = new OutputData(S, 0);
            histories[i].push_back(data);
            ++i;

            e += data->e;

            if(calc->ic == IsingMonteCarloCalculator::Ferromagnetic || _SINGLE_IC)
                mag_ferro += fabs(data->m);
            else if(!_SINGLE_IC)
                mag_para += fabs(data->m);
        }

        {
            e /= simulations.size();
            avg_e_view->addPoint(MCStep, e);


            mag_ferro /= simulations.size();
            if(!_SINGLE_IC) mag_ferro *= 2;

            avg_t_view_1->addPoint(MCStep, mag_ferro);

            #if SHOW_DDT == true
            auto data = avg_t_view_1->getData();
            if (data.size() > 1) {
                auto i = data.size() - 1;
                auto davg = data[i].second - data[i - 1].second;
                auto val = davg;
                ddtavg_t_view_1->addPoint(MCStep, val);
            }
            #endif
        }

        if(!_SINGLE_IC)
        {
            mag_para /= .5 * simulations.size();
            avg_t_view_2->addPoint(MCStep, mag_para);

            #if SHOW_DDT == true
            auto data = avg_t_view_2->getData();
            if (data.size() > 1) {
                auto i = data.size() - 1;
                auto davg = data[i].second - data[i - 1].second;
                //const auto eps = 1.e-2;
                //auto val = log(1+davg/eps);
                auto val = davg;
                ddtavg_t_view_2->addPoint(MCStep, val);
            }
            #endif
        }
    }

    void HistoryViewController::_computeTimeCorrelations(int upToMCStep) {
        auto t0 = tau_eq;

        if(upToMCStep <= t0) return;

        auto nHistories = histories.size();

        auto t_total = upToMCStep - t0;
        auto tCorrMax_local = fmin(t_total, t_max);

        std::vector<double> c(tCorrMax_local);

        for (auto t=0; t < tCorrMax_local; ++t){
            auto corr = .0;
            auto m = .0;
            auto mt = .0;

            for (auto &history : histories){
                for (int tp = 0; tp < t_total - t; ++tp) {
                    auto _m = fabs(history[t0+tp]->m);
                    auto _mt = fabs(history[t0+tp+t]->m);

                    corr += (_m*_mt);
                    m += _m;
                    mt += _mt;
                }
            }

            auto samples = (t_total - t)*nHistories;
            c[t] = corr/samples - m/samples * mt/samples;
        }

        {
            auto c0 = c[0];
            time_corr_view->clearData();
            for(auto t=0; t < tCorrMax_local; ++t) {
                auto norm_corr = c[t] / c0;
                time_corr_view->addPoint(t, norm_corr);
            }
        }
    }

    void HistoryViewController::_computeSpaceCorrelations(){
        //space_corr_est_view->clearData();

        const auto MCStep = histories.front().size();
        const auto t = MCStep-1;
        const auto samples = histories.size();

        {
            auto e=.0;
            for(const auto &history : histories)
                e += history[t]->e;

            e /= samples;

            space_corr_est_view->addPoint(t, 2./(e-eT));
        }
    }

    void HistoryViewController::run() {
        running = true;

        auto MCStep = 0;
        while(running) {
            if(MCStep < MCSteps) {
                _stepSims(MCStep);
            }
            else if(MCStep == MCSteps){
                ++MCStep;
                _computeTimeCorrelations(MCSteps);

                _dumpData();
            }

            _treatEvents();

            if(!(MCStep%MCSTEPS_BETWEEN_FRAMES) || MCStep>=MCSteps-1)
            {
                #if LIMIT_SIM_SPEED == true
                if (timer.getElapsedTime().asMilliseconds() > FRAME_INTERVAL_MSEC)
                #endif

                {
                    for (auto &sim : simulations) {
                        auto &S = sim.first->getS();
                        auto &texture = *sim.second;

                        __updateIsingGraph(S, texture);
                    }

                    if(MCStep < MCSteps && realTimeCorrelations) // gambiarra
                        _computeTimeCorrelations(MCStep);

                    _computeSpaceCorrelations();

                    window.clear();
                    _drawEverything(MCStep);
                    window.display();
                    timer.restart();
                }
            }
        }

        if(MCStep >= MCSteps) {
            std::cout << "Dumping data... ";
            _dumpData();
            std::cout << "Dumped." << std::endl;
        }

        window.clear();
        _drawEverything(MCStep);
        window.display();
        timer.restart();

        window.close();
    }

    bool HistoryViewController::doOperate(SystemParams &params, OutputData &data) {
        return true;
    }

    void HistoryViewController::__updateIsingGraph(const IsingNetwork &S, sf::Texture &networkTexture) {
        u_char neg = .2*255;
        u_char pos = .8*255;
        auto positiveColor = sf::Color(pos,pos,pos);
        auto negativeColor = sf::Color(neg,neg,neg);
        for(int i=0; i<S.L; ++i)
            for(int j=0; j<S.L; ++j) {
                auto color = S.s(i,j)==1?positiveColor:negativeColor;
                helperBitmap.setPixel(i, j, color);
            }
        networkTexture.loadFromImage(helperBitmap);
    }

    void HistoryViewController::_treatEvents() {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running=false;
                return;
            }

            else if(event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;

                if(key == sf::Keyboard::Escape){
                    running=false;
                    return;
                }
                else if(key== sf::Keyboard::Equal) ++MCSTEPS_BETWEEN_FRAMES;
                else if(key== sf::Keyboard::Hyphen) {
                    --MCSTEPS_BETWEEN_FRAMES;
                    if(MCSTEPS_BETWEEN_FRAMES < 1) MCSTEPS_BETWEEN_FRAMES=1;
                }
                else if(key== sf::Keyboard::Space) realTimeCorrelations = !realTimeCorrelations;
            }
        }
    }

    void HistoryViewController::_drawEverything(int MCStep) {
        for(auto drawable : drawables)
            window.draw(*drawable);

        auto sweep = simulations[0].first->sweeping==IsingMonteCarloCalculator::Random?"RANDOM":"SEQUENTIAL";
        auto dynamicStr = dynamic==IsingMonteCarloCalculator::Metropolis?"METROPOLIS":"KAWASAKI";
        std::ostringstream textOutput;
        textOutput  << "Simulating with "<< sweep <<" sweep and " << dynamicStr << " algorithm.\n\n"

                    << "tau_eq = " << tau_eq << "\n"
                    //<< "tau_int = " << tau_int << "\n"
                    //<< "tau_corr = " << "?" << "\n"
                    //<< "\n"
                    << "t_max_corr = " << t_max << "\n"
                    << "\n"
                    << "L = " << L << "\n"
                    << "eT = " << eT << "\n"
                    << "T_c = 2.269\n"
                    << "T = " << T << "\n"
                    << "T/T_c = " << T/2.269 << "\n"
                    << "\n"
                    << "MCSteps/frame: " << MCSTEPS_BETWEEN_FRAMES << "\n"
                    << "\n"
                    << "MCS = " << MCStep << "/" << MCSteps << "\n";

        text.setString(textOutput.str());
        window.draw(text);
    }

    void HistoryViewController::hide() {
        window.setVisible(false);
    }

    void HistoryViewController::_dumpData() {


        std::ofstream myfile;
        std::stringstream fileName;

        fileName << "MCIsing-manyHistories--" << SQRT_SIMS << "x" << SQRT_SIMS << "HISTORIES--" << L << "x" << L
                 << "GRID--T=" << T;

        if(1)
        {
            auto data = space_corr_est_view->getData();
            auto nameAppend = std::string(dynamic==IsingMonteCarloCalculator::Kawasaki?"-kawasaki":"-metropolis") + "--space-corr.dat";
            myfile.open(fileLoc + fileName.str() + nameAppend);
            for (const auto d : data) {
                auto t = d.first;
                auto R = d.second;
                myfile << t << " " << R << "\n";
            }
            myfile.close();
        }

        if(0)
        {


            {
                auto data_ferro = avg_t_view_1->getData();
                myfile.open(fileLoc + fileName.str() + "--ferro-IC.dat");
                myfile << "# Many histories avg output with ferromagnetic initial conditions.\n# t    <m>(t) \n";
                for (const auto data : data_ferro) {
                    auto t = data.first;
                    auto m = data.second;
                    myfile << t << " " << m << "\n";
                }
                myfile.close();
            }


            #if !_SINGLE_IC
            {
                auto data_para = avg_t_view_2->getData();
                myfile.open(fileLoc + fileName.str() + "--para-IC.dat");
                myfile << "# Many histories avg output with paramagnetic initial conditions.\n# t    <m>(t) \n";
                for (const auto data : data_para) {
                    auto t = data.first;
                    auto m = data.second;
                    myfile << t << " " << m << "\n";
                }
                myfile.close();
            }
            #endif


            {
                auto data_corr = time_corr_view->getData();
                myfile.open(fileLoc + fileName.str() + "--time-corr-normalized.dat");
                myfile << "# Many histories avg output\n# t    C(t)/C(0) \n";
                for (const auto data : data_corr) {
                    auto t = data.first;
                    auto C = data.second;
                    myfile << t << " " << C << "\n";
                }
                myfile.close();
            }


            {
                myfile.open(fileLoc + fileName.str() + ".gp");
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
        }
    }
}