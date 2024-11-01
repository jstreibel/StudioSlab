//
// Created by joao on 22/07/2021.
//


#include "QuenchViewController.h"
#include "Graphics/SFML/Palletes.h"
#include "Core/Tools/Resources.h"

#include <SFML/Graphics.hpp>

#include <ostream>
#include <iostream>

namespace Slab::Lost::ThermoOutput {
    // View
    const bool SHOW_ENERGY = false;
    const int COLS = 1;
    const int ROWS = 4;
    const float miniBorder = 8;
    const float _border = 16;
    const int fontSize = 12;

    // Dynamic
    const auto dynamic = XYMetropolisAlgorithm::Metropolis;

    // Program:
    int MCSTEPS_BETWEEN_FRAMES = 1;

    // Output
    const std::string fileLoc = std::string("Tarefa8/");


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
    QuenchViewController::QuenchViewController(const int L, int tau_eq, int tau_corr, int total_measures, const int sqrtNSims)
            : window(sf::VideoMode(WinW, WinH), "SFML window", style), T(3.0), L(L),
            MCSteps(tau_eq+total_measures*3*tau_corr), tau_eq(tau_eq), tau_corr(tau_corr), total_measures(total_measures), SQRT_SIMS(sqrtNSims)
    {
        #if FULLSCREEN == false
        window.setPosition(sf::Vector2i(200, 100));
        #endif
        #if LIMIT_SIM_SPEED == true
        window.setFramerateLimit(1000. / _FRAME_INTERVAL_MSEC);
        #endif

        timer.restart();

        const float isingSpriteSize = (WinH - _border - (SQRT_SIMS + 1) * miniBorder) / SQRT_SIMS;
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
                sprite->setPosition(float(i) * (miniBorder + isingSpriteSize) + _border, float(j) * (miniBorder + isingSpriteSize) + _border);

                /***********************************************************************************
                 * *********************************************************************************
                 * ********** TODO: MAKE THIS WORK AGAIN *******************************************
                 * *********************************************************************************
                 */
                auto calc = new XYMetropolisAlgorithm(L, T, .0, XYMetropolisAlgorithm::Ferromagnetic, dynamic, XYMetropolisAlgorithm::Random);

                drawables.push_back(sprite);
                simulations.emplace_back(calc, texture);
            }
        }


        auto xPos = (SQRT_SIMS)*(isingSpriteSize+miniBorder) + 2 * _border;
        auto colWidth = (WinW - xPos - _border) / COLS;
        auto rowHeight = (WinH - _border) / ROWS;

        sf::IntRect subWindow(xPos, _border, colWidth - _border, rowHeight);
        float xMin=0, xMax=MCSteps; u_char gray=.4*255; sf::Color dashColor(gray, gray, gray);

        {
            e_t_view = new Graph(subWindow, {xMin, -2.1}, {xMax, 0.1}, "t", "<e>(t)");
            e_t_view->addHorizontalDashedLine(0);

            drawables.push_back(e_t_view);
        }

        {
            subWindow.top += rowHeight + _border;

            m_t_view = new Graph(subWindow, {xMin, -1.1}, {xMax, 1.1}, "t", "<m>(t)");
            drawables.push_back(m_t_view);
            m_t_view->addVerticalDashedLine(tau_eq);
            m_t_view->addHorizontalDashedLine(1);
        }

        {
            subWindow.top += rowHeight + _border;

            T_t_view = new Graph(subWindow, {0, float(1.9)}, {float(MCSteps), 3.1}, "t", "T(t)");
            // T_t_view->addHorizontalDashedLine(T_c);

            drawables.push_back(T_t_view);
        }

        {
            subWindow.top += rowHeight + _border;

            font.loadFromFile(Slab::Core::Resources::fontFileName(10));
            text = sf::Text("Ising", font, fontSize);
            text.setPosition(subWindow.left, subWindow.top);

            histories.resize(SQRT_SIMS * SQRT_SIMS);
        }
    }


    void QuenchViewController::_stepSims() {
        ++MCStep;

        Real mag = .0;
        Real e = .0;
        int i=0;
        for (auto sim : simulations) {
            auto calc = sim.first;
            const XYNetwork &S = calc->getS();

            calc->MCStep();

            auto data = new OutputData(S, 0);
            histories[i].push_back(data);

            e += data->e;
            mag += fabs(data->m);

            ++i;
        }

        {
            e /= simulations.size();
            e_t_view->addPoint(MCStep, e);

            mag /= simulations.size();
            m_t_view->addPoint(MCStep, mag);

            T_t_view->addPoint(MCStep, T);
        }

    }

    auto QuenchViewController::_runProgram() -> bool {
        if(MCStep < tau_eq) return true;
        else if(MCStep > MCSteps) return false;

        const Real ΔT = 1.0 / total_measures;
        if(!((MCStep-tau_eq)%(3*tau_corr))){
            auto e=0., e2=0., m=0., m2=0., m4=0.;

            for(auto &history : histories) {
                auto &last = history.back();

                e += last->e;
                e2 += last->e2;
                m += last->m;
                m2 += last->m2;
                m4 += last->m2*last->m2;
            }

            auto samples = histories.size();
            av_e.emplace_back(e/samples);
            av_e2.emplace_back(e2/samples);
            av_m.emplace_back(m/samples);
            av_m2.emplace_back(m2/samples);
            av_m4.emplace_back(m4/samples);
            T_values.emplace_back(T);

            T-=ΔT;
            for(auto &sim_pair : simulations)
                sim_pair.first->set_T(T);
        }

        return true;
    }

    void QuenchViewController::run() {
        running = true;
        auto simulating = true;

        MCStep = 0;
        while(running) {
            if(simulating) {
                _stepSims();
            }
            else {
                _dumpData();
            }

            _treatEvents();

            simulating = _runProgram();

            if(!(MCStep%MCSTEPS_BETWEEN_FRAMES) || !simulating)
            {
                #if LIMIT_SIM_SPEED == true
                if (timer.getElapsedTime().asMilliseconds() > _FRAME_INTERVAL_MSEC)
                #endif

                {
                    for (auto &sim : simulations) {
                        auto &S = sim.first->getS();
                        auto &texture = *sim.second;

                        __updateIsingGraph(S, texture);
                    }

                    window.clear();
                    _drawEverything();
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
        _drawEverything();
        window.display();
        timer.restart();

        window.close();
    }

    bool QuenchViewController::doOperate(SystemParams &params, OutputData &data) {
        return true;
    }

    void QuenchViewController::__updateIsingGraph(const XYNetwork &S, sf::Texture &networkTexture) {
        u_char neg = .2*255;
        u_char pos = .8*255;
        auto positiveColor = sf::Color(pos,pos,pos);
        auto negativeColor = sf::Color(neg,neg,neg);
        for(int i=0; i<S.L; ++i)
            for(int j=0; j<S.L; ++j) {
                sf::Color color;
                if(!SHOW_ENERGY)
                    color = S.theta(i, j) == 1 ? positiveColor : negativeColor;
                else {
                    throw "Not implemented";
                    //color = pallete2[(S.e(i, j) + 4) / 2];

                }

                helperBitmap.setPixel(i, j, color);
            }
        networkTexture.loadFromImage(helperBitmap);
    }

    void QuenchViewController::_treatEvents() {
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
            }
        }
    }

    void QuenchViewController::_drawEverything() {
        for(auto drawable : drawables)
            window.draw(*drawable);

        auto sweep = simulations[0].first->sweeping == XYMetropolisAlgorithm::Random ? "RANDOM" : "SEQUENTIAL";
        auto dynamicStr = dynamic == XYMetropolisAlgorithm::Metropolis ? "METROPOLIS" : "KAWASAKI";
        std::ostringstream textOutput;
        textOutput  << "Simulating with "<< sweep <<" sweep and " << dynamicStr << " algorithm.\n\n"

                    << "tau_eq = " << tau_eq << "\n"
                    //<< "tau_int = " << tau_int << "\n"
                    //<< "tau_corr = " << "?" << "\n"
                    //<< "\n"
                    << "\n"
                    << "L = " << L << "\n"
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

    void QuenchViewController::hide() {
        window.setVisible(false);
    }

    void QuenchViewController::_dumpData() {

        NOT_IMPLEMENTED_CLASS_METHOD

        /*
        std::stringstream fileName;

        fileName << "MCIsing-manyHistories-manyT--" << SQRT_SIMS << "x" << SQRT_SIMS << "HISTORIES--" << L << "x" << L
                 << "GRID--n_T=" << total_measures;

        std::ofstream file;


        /*
        {
            file.open(fileLoc + fileName.str() + ".dat");

            file << "# 1   2    3   4    5    6    7     8    9   10\n";
            file << "# T   e   e2   m   m2   m4   C_v   Xi   σ_e  σ_m\n";

            Real e, e2, m, m2, m4, T;
            for (auto data : boost::combine(av_e, av_e2, av_m, av_m2, av_m4, T_values)) {
                boost::tie(e, e2, m, m2, m4, T) = data;
                auto N=L*L;

                auto σ2_e = e2-e*e,
                     σ2_m = m2-m*m;
                auto C_v  = (N/(T*T)*σ2_e),
                     Xi   = (N/T*σ2_m);

                file << T << " "
                     << e << " " << e2 << " "
                     << m << " " << m2 << " " << m4 << " "
                     << C_v << " " << Xi << " "
                     << sqrt(σ2_e) << " " << sqrt(σ2_m) << "\n";
            }

            file.close();
        }


        {
            file.open(fileLoc + fileName.str() + ".gp");
            file << "set terminal qt\n"
                   << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                   << "set ylabel \"<e>(T)\" textcolor rgb \"#00000000\"\n"
                   << "set border lc rgb 'black'\n"
                   << "set key tc rgb 'black'\n"
                   << "plot \"" << (fileName.str() + ".dat") << "\" using 1:2 title \""
                                << L << "x" << L << R"(" with lines lc rgb "#00ff9999", )"
                                << "\"" << (fileName.str() + ".dat") << "\" using 1:2:9 title \"error\" with errorbars\n\n"

                   << "set terminal qt 1\n"
                   << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                   << "set ylabel \"<e^2>(T)\" textcolor rgb \"#00000000\"\n"
                   << "set border lc rgb 'black'\n"
                   << "set key tc rgb 'black'\n"
                   << "plot \"" << (fileName.str() + ".dat") << "\" using 1:3 title \""
                   << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n\n"

                   << "set terminal qt 2\n"
                   << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                   << "set ylabel \"<m>(T)\" textcolor rgb \"#00000000\"\n"
                   << "set border lc rgb 'black'\n"
                   << "set key tc rgb 'black'\n"
                   << "plot \"" << (fileName.str() + ".dat") << "\" using 1:4 title \""
                                << L << "x" << L << R"(" with lines lc rgb "#00ff9999", )"
                                << "\"" << (fileName.str() + ".dat") << "\" using 1:4:10 title \"error\" with errorbars\n\n"

                   << "set terminal qt 3\n"
                   << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                   << "set ylabel \"<m^2>(T)\" textcolor rgb \"#00000000\"\n"
                   << "set border lc rgb 'black'\n"
                   << "set key tc rgb 'black'\n"
                   << "plot \"" << (fileName.str() + ".dat") << "\" using 1:5 title \""
                   << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n\n"

                    << "set terminal qt 4\n"
                    << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                    << "set ylabel \"<m^4>(T)\" textcolor rgb \"#00000000\"\n"
                    << "set border lc rgb 'black'\n"
                    << "set key tc rgb 'black'\n"
                    << "plot \"" << (fileName.str() + ".dat") << "\" using 1:6 title \""
                    << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n\n"

                    << "set terminal qt 5\n"
                    << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                    << "set ylabel \"<C_v>(T)\" textcolor rgb \"#00000000\"\n"
                    << "set border lc rgb 'black'\n"
                    << "set key tc rgb 'black'\n"
                    << "plot \"" << (fileName.str() + ".dat") << "\" using 1:7 title \""
                    << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n\n"

                    << "set terminal qt 6\n"
                    << "set xlabel \"T\" textcolor rgb \"#00000000\"\n"
                    << "set ylabel \"<Xi>(T)\" textcolor rgb \"#00000000\"\n"
                    << "set border lc rgb 'black'\n"
                    << "set key tc rgb 'black'\n"
                    << "plot \"" << (fileName.str() + ".dat") << "\" using 1:8 title \""
                    << L << "x" << L << "\" with lines lc rgb \"#00ff9999\"\n\n";



            file.close();
        }
         */
    }

}