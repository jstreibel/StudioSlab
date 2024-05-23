//
// Created by joao on 24/06/2021
// jstreibel@gmail.com
//


#include "Integrator.h"


void OutputToVector::clear() {
    m_states.clear();
    m_times.clear();
}

void OutputToVector::operator()(const Funcs &x, double t) {
    m_states.push_back( x );
    m_times.push_back( t );

    //auto f=x[3],
    //     g=x[4],
    //     h=x[5],
    //     dfdx=x[0],
    //     dgdx=x[1],
    //     dhdx=x[2];

    //std::cout << t << " ";
    //std::cout << f << " " << g << " " << h;
    //std::cout << std::endl;
}

OutputHelper::OutputHelper(Output *output) : output(output) {}

void OutputHelper::operator()(const Funcs &x, double t) { output->operator()(x, t); }

void StringsOfMarylou::RHS(const Funcs &V, Funcs &dVdx, const double x) const {
    const auto &c=V[0],
            &d=V[1],
            &e=V[2],
            &f=V[3],
            &g=V[4],
            &h=V[5];

    auto &dcdx=dVdx[0], // f''
    &dddx=dVdx[1], // g''
    &dedx=dVdx[2], // h''
    &dfdx=dVdx[3], // f'
    &dgdx=dVdx[4], // g'
    &dhdx=dVdx[5]; // h'

    const auto invx = 1./x;
    const auto a2 = alpha_phi*alpha_phi;
    const auto n2 = 1.0;
    const auto sqTerm = (beta/4)*(f*f + g*g - 2);
    dcdx = -invx*c + f*sqTerm;
    dddx = -invx*d + g*sqTerm + n2*invx*invx*g*(1-h)*(1-h);
    dedx =  invx*e - a2*(1-h)*g*g;
    dfdx = c;
    dgdx = d;
    dhdx = e;
}

void StringsOfMarylou::operator()(const Funcs &V, Funcs &dVdx, const Real x) {
    return RHS(V, dVdx, x);
}

void MaryApp::updateGraph() {
    for (auto &g : graphs)
        g.clear();

    for (int i = 0; i < output.m_states.size(); i++) {
        auto x = output.m_times[i];

        for (int k = 0; k < 6; k++)
            graphs[k].addPoint(x, output.m_states[i][k], colors[k]);
    }
}

MaryApp::MaryApp(sf::RenderWindow &window) : AppBase(0, nullptr) {
    for(int k=0; k<6; k++){
        sf::PrimitiveType primitiveType = sf::LineStrip;
        if(k<3) primitiveType = sf::Lines;
        auto winW = window.getSize().x, winH = window.getSize().y;
        graphs.push_back(
                Lost::Graph(
                        sf::IntRect(rectLeft,
                                    20,
                                    winW-rectLeft-20,
                                    winH-2*20),
                        {0, -0.2},
                        {end_x, 1.2},
                        "x",
                        "f(x), g(x), h(x)",
                        primitiveType));
        colors[k] = sf::Color(colorInts[k]);
    }

    graphs[0].addHorizontalDashedLine(1, 200, sf::Color::Yellow);

    run();
    updateGraph();
}

int MaryApp::run() {
    const Real beta=0.5, alpha_phi=2;
    const Real dfdx0=0, g0=0, h0=0;  // don't touch this;
    Funcs V0 = {dfdx0, dgdx0, dhdx0,
                f0, g0, h0};
    StringsOfMarylou rs = {beta, alpha_phi};

    if(1) {
        auto nSteps = boost::numeric::odeint::integrate(rs, V0, start_x, end_x, dx,
                                                        OutputHelper(&output));
        return nSteps;
    } else if(1) {
        boost::numeric::odeint::runge_kutta4<Funcs> stepper;
        boost::numeric::odeint::integrate_const( stepper , rs, V0, start_x , end_x , dx );
    }

    return 0;
}

void MaryApp::setupGUI(nk_context *ctx) {
    bool valueChanged = false;
    if (nk_begin(ctx, "Strings", nk_rect(20, 20, 1.1 * w, 900 - 2 * 20),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {

        // custom widget pixel width
        nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
        {
            auto lw = 75.f;

            std::ostringstream ss;
            ss.precision(6);

            nk_layout_row_push(ctx, lw);
            ss << "f(0)=" << f0;
            auto c = colors[3];
            nk_color color = {c.r, c.g, c.b, c.a};
            nk_label_colored(ctx, ss.str().c_str(), NK_TEXT_LEFT, color);

            nk_layout_row_push(ctx, w - lw);
            if (nk_slider_float(ctx, -5.f, &f0, 5.f, 0.001f)) {
                valueChanged = true;
            }

            nk_layout_row_push(ctx, lw);
            c = colors[0];
            color = {c.r, c.g, c.b, c.a};
            nk_label_colored(ctx, "f'(0)=0", NK_TEXT_LEFT, color);
            nk_layout_row_push(ctx, 50);
            nk_label(ctx, "", NK_TEXT_LEFT);


            nk_layout_row_push(ctx, lw);
            c = colors[4];
            color = {c.r, c.g, c.b, c.a};
            nk_label_colored(ctx, "g(0)=0", NK_TEXT_LEFT, color);
            nk_layout_row_push(ctx, lw);
            nk_label(ctx, "", NK_TEXT_LEFT);

            nk_layout_row_push(ctx, lw);
            ss.str("");
            ss << "g'(0)=" << dgdx0;
            c = colors[1];
            color = {c.r, c.g, c.b, c.a};
            nk_label_colored(ctx, ss.str().c_str(), NK_TEXT_LEFT, color);

            nk_layout_row_push(ctx, w - lw);
            if (nk_slider_float(ctx, -5.f, &dgdx0, 5.f, 0.001f)) {
                valueChanged = true;
            }


            nk_layout_row_push(ctx, lw);
            c = colors[5];
            color = {c.r, c.g, c.b, c.a};
            nk_label_colored(ctx, "h(0)=0", NK_TEXT_LEFT, color);
            nk_label(ctx, "", NK_TEXT_LEFT);

            nk_layout_row_push(ctx, lw);
            c = colors[2];
            color = {c.r, c.g, c.b, c.a};
            ss.str("");
            ss << "h'(0)=" << dhdx0;
            nk_label_colored(ctx, ss.str().c_str(), NK_TEXT_LEFT, color);

            nk_layout_row_push(ctx, w - lw);
            if (nk_slider_float(ctx, -5.f, &dhdx0, 5.f, 0.01f)) {
                valueChanged = true;
            }

        }
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);

    if (valueChanged) {
        output.clear();
        run();
        updateGraph();
    }
}

void MaryApp::draw(sf::RenderWindow &window) {
    for(auto &g : graphs)
        window.draw(g);
}
