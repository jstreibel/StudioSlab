#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include <Phys/Numerics/Output/Channel/OutputChannel.h>
#include <Studios/Backend/GLUT/GLUTEventListener.h>

#include <GL/glut.h>
// STD includes
#include <iostream>
#include <vector>

namespace Base {

    #define isOutputOpenGL(output) (dynamic_cast<Base::OutputOpenGL*>(output) != nullptr)

    class OutputOpenGL : public OutputChannel, public GLUTEventListener{
    public:
        OutputOpenGL();
        ~OutputOpenGL() override;


    // ********************* From GLUTEventListener ************** //

    // ********************* End GLUTEventListener ************** //


    // ********************* From OutputBase ********************* //
        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;

        void notifyRender() override;

    protected:
        void _out(const OutputPacket &outInfo) override;
    // ********************* END OutputBase ********************** //

    bool finishFrameAndRender();


    // ********************* From OpenGLArtistBase *************** //
    auto needDraw() const -> bool override;
    virtual void draw() override;

    // ********************* END OpenGLArtistBase **************** //

    public:
        void addAnimation(Animation *animation) {animations.push_back(animation); }

    private:
        std::vector<Animation*> animations;


    protected:
        double lastT; // essa variavel precisa existir para ficar aparecendo na tela.

        int windowWidth, windowHeight;
    };
}

#endif // OUTPUTOPENGL_H
