//
// Created by joao on 25/06/2021.
//

#include "NuklearInclude.h"
#include "sfml-utils/Graph.h"

#include "Integrator.h"

#include <string>
#include <sstream>


struct OutputToVector : public Output
{
    std::vector< Funcs > m_states;
    std::vector< double > m_times;

    OutputToVector() = default;

    void clear(){
        m_states.clear();
        m_times.clear();
    }

    void operator()( const Funcs &x , double t )
    {
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
};



sf::RenderWindow window{{2000, 900}, "Nuklear SFML"};

const uint32_t MaxVertexMemory = 512 * 1024;
const uint32_t MaxElementMemory = 128 * 1024;

nk_context *ctx = nullptr;

OutputToVector output;

auto w = 300.f;
int rectLeft = 1.1 * w + 4 * 20;

std::vector<Graphing::Graph> graphs;
float start_x = 0.0001, end_x = 5, dx = 1.e-5 ;
float f0 = 1.09868f, dgdx0 = 1.31902f, dhdx0 = 0.000119189;

//std::vector<unsigned> colorInts({0xbeef9eff, 0xa6c36fff,0x828c51ff,0x335145ff,0x1e352fff,0x7d5ba6ff});
//std::vector<unsigned> colorInts({0xe27396ff,0xea9ab2ff,0xe0e2dbff,0xd2d4c8ff,0xd3efbdff,0x447604ff});
const std::vector<unsigned> ci{ 0x54478cff, 0x2c699aff, 0x048ba8ff, 0x0db39eff, 0x16db93ff, 0x83e377ff, 0xb9e769ff, 0xefea5aff, 0xf1c453ff, 0xf29e4cff };
const std::vector<unsigned> colorInts{ ci[0], ci[3], ci[6], ci[1], ci[4], ci[7] };

std::vector<sf::Color> colors = std::vector<sf::Color>(6);


void InitGUI() {
    ctx = nk_sfml_init(&window);

    nk_font_atlas *atlas{nullptr};
    nk_sfml_font_stash_begin(&atlas);
    // Adding fonts:
    // nk_font *font = nk_font_atlas_add_from_file(atlas, "path", height, config);
    nk_sfml_font_stash_end();

    // Setting a font:
    // nk_style_set_font(ctx, &font->handle);
}

void updateGraph() {
    for (auto &g : graphs)
        g.clear();

    for (int i = 0; i < output.m_states.size(); i++) {
        auto x = output.m_times[i];

        for (int k = 0; k < 6; k++)
            graphs[k].addPoint(x, output.m_states[i][k], colors[k]);
    }
}

void GUI(nk_context *ctx) {
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
        run(&output, f0, dgdx0, dhdx0, start_x, end_x, dx);
        updateGraph();
    }
}


int main(int, const char **)
{
    {
        for(int k=0; k<6; k++){
            sf::PrimitiveType primitiveType = sf::LineStrip;
            if(k<3) primitiveType = sf::Lines;
            auto winW = window.getSize().x, winH = window.getSize().y;
            graphs.push_back(Graphing::Graph(sf::IntRect(rectLeft, 20, winW-rectLeft-20, winH-2*20), {0, -0.2}, {end_x, 1.2}, "x", "f(x), g(x), h(x)", primitiveType));
            colors[k] = sf::Color(colorInts[k]);
        }

        graphs[0].addHorizontalDashedLine(1, 200, sf::Color::Yellow);
        run(&output, f0, dgdx0, dhdx0, start_x, end_x, dx);
        updateGraph();

        InitGUI();
    }


    bool running = true;
    sf::Event event{};


    while (running) {
        nk_input_begin(ctx);
        while (window.pollEvent(event)) {
            nk_sfml_handle_event(event);

            if (event.type == sf::Event::Closed) {
                running = false;
                break;
            }
        }
        nk_input_end(ctx);

        GUI(ctx);

        window.clear();
        {
            for(auto &g : graphs){
                window.draw(g);
            }

            nk_sfml_render(NK_ANTI_ALIASING_ON, MaxVertexMemory, MaxElementMemory);
        }
        window.display();
    }

    window.close();
    nk_sfml_shutdown();

    return 0;
}