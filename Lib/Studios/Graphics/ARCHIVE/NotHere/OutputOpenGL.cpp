#include "OutputOpenGL.h"

#include <Phys/Numerics/Allocator.h>


using namespace Base;

OutputOpenGL::OutputOpenGL()
    : OutputChannel("OpenGL output", 80), lastT(0.0)
{
    finishFrameAndRender();
}

OutputOpenGL::~OutputOpenGL() = default;

void OutputOpenGL::_out(const OutputPacket &outInfo)
{
    this->finishFrameAndRender();
}

auto OutputOpenGL::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return finishFrameAndRender();
}

bool OutputOpenGL::needDraw() const {
    return lastInfo.getSpaceData().first != nullptr;
}

void OutputOpenGL::draw() {
    /*
    char buffer[128];

    const NumericParams &p = Allocator::getInstance().getNumericParams();

    const double h = p.geth();
    const double dt = p.getdt();
    const size_t N = p.getN();

    sprintf(buffer, "h = %.2e", h);
    addVolatileStat(buffer);

    sprintf(buffer, "dt = %.2e", dt);
    addVolatileStat(buffer);

    sprintf(buffer, "N = %i", int(N));
    addVolatileStat(buffer);

    sprintf(buffer, "t = %f", lastInfo.getT());
    addVolatileStat(buffer);

    sprintf(buffer, "steps = %i", (int)lastInfo.getSteps());
    addVolatileStat(buffer);
     */

    //sprintf(buffer, "<time/step> = %d microsecond/step", (int)GLUTBackend::getSingleton()->getProgram().getHistogram().getAverage());
    //addVolatileStat(buffer);
}

bool OutputOpenGL::finishFrameAndRender() {
        for(auto *anim : animations) anim->step(frameTimer.getElTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        {
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_POINT_SMOOTH);
                glEnable(GL_LINE_SMOOTH);
            }

            if (needDraw())
                draw();
        }
        glPopMatrix();

        frameTimer.reset();
        return true;
    }

void OutputOpenGL::notifyRender() {
    GLUTEventListener::notifyRender();

    finishFrameAndRender();
}

