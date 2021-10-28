#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Studios/CoreMath/Integrator/Output/Channel/OutputChannel.h"
#include "Studios/CoreMath/Integrator/Output/Channel/OutputHistory.h"
#include "Studios/Backend/Graphic/OpenGLArtistBase.h"

#include <GL/glut.h>
// STD includes
#include <iostream>
#include <vector>

namespace Base {

    #define isOutputOpenGL(output) (dynamic_cast<Base::OutputOpenGL*>(output) != nullptr)

    class OutputOpenGL : public OutputChannel, public OpenGLArtistBase{
    public:
        OutputOpenGL();
        ~OutputOpenGL() override;


    // ********************* From OutputBase ********************* //
        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;
        auto description() const -> String override { return "OpenGL output"; }
    protected:
        void _out(const OutputPacket &outInfo) override;
    // ********************* END OutputBase ********************** //


    // ********************* From OpenGLArtistBase *************** //
    auto needDraw() const -> bool override;
    virtual void draw() override;
    // ********************* END OpenGLArtistBase **************** //


    protected:
        double lastT; // essa variavel precisa existir para ficar aparecendo na tela.
    };
}

#endif // OUTPUTOPENGL_H
