#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "defs.h"
#include "CHData.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class OutputOpenGL
{
public:
    OutputOpenGL(const double L);

    //void operator()( const MoleculContainer &molecules   , double t);
    void operator()(const CHData &data, const double t);

    bool isFinished() {return finished;}
private:
    void _treatEvents();
    void _beginGL();
    void _endGL();
    bool finished = false;
    sf::RenderWindow *window;
};

#endif // OUTPUTOPENGL_H
