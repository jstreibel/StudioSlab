#include "outputopengl.h"

#include <SFML/OpenGL.hpp>

OutputOpenGL::OutputOpenGL(const double L)
    : window(new sf::RenderWindow(sf::VideoMode(800, 800), "Mol. dyn."))
{
    //window->setFramerateLimit(60);
    window->setPosition(sf::Vector2i(10, 50));

    const double l = L*1.2;
    glOrtho(-l*.5, l*.5, -0.1, 1.1, -1., 1.);
    glPointSize(0.5);
}

void OutputOpenGL::operator()(const CHData &data, const double t)
{
    (void) t;

    _treatEvents();

    _beginGL();

    const double h = data.h;
    for(size_t i=0; i<data.size(); i++)
        glVertex2d(i*h, data[i]);

    _endGL();
}

/*
void OutputOpenGL::operator()( const MoleculContainer &    molecules   , double t) {
    _treatEvents();

    _beginGL();
    for(auto m : molecules)
    {
        const Point2D &q = m.q;

        glVertex2d(q.x, q.y);
    }
    _endGL();
}
*/

void OutputOpenGL::_treatEvents()
{
    sf::Event event;
    while(window->pollEvent(event)){
        if (event.type == sf::Event::Closed){
            // window->close();
            finished = true;
        } else if (event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Escape){
                finished = true;
            }
            if(event.key.code == sf::Keyboard::PageDown){

            }
        }
    }
}

void OutputOpenGL::_beginGL()
{
    const float fAtt=0.3f;
    sf::Color bg(int(fAtt*86), int(fAtt*110), int(fAtt*73));
    window->clear(bg);

    glColor3d(1, 1, 1);

    /*
    glBegin(GL_LINES);
    {
        glVertex2d(-.5*SIGMA, 0.);
        glVertex2d( .5*SIGMA, 0.);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    {
        const double hw = SIM_AREA * .5;
        glVertex2d(-hw, -hw);
        glVertex2d(-hw,  hw);
        glVertex2d( hw,  hw);
        glVertex2d( hw, -hw);
    }
    glEnd();
    */
    glBegin(GL_POINTS);
}

void OutputOpenGL::_endGL()
{
    glEnd();
    window->display();
}
