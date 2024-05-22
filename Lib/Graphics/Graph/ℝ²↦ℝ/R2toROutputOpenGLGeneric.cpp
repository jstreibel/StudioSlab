//

#include "R2toROutputOpenGLGeneric.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Graph/PlotThemeManager.h"
#include "Models/KleinGordon/R2toR/EquationState.h"

#define ADD_TO_NEW_COLUMN true

R2toR::OutputOpenGL::OutputOpenGL(const NumericConfig &params)
: ::Graphics::OpenGLMonitor(params, "R2 -> R OpenGL output", 1)
, mSectionGraph(params.getxMin(),
                params.getxMax(),
                -1, 1,
                "Sections")
, mFieldDisplay()
{
    addWindow(Slab::DummyPointer(mSectionGraph), ADD_TO_NEW_COLUMN, .50);

    auto yMin = params.getxMin(),
         yMax = params.getxMax();

    auto line = Slab::New<RtoR2::StraightLine>(Real2D{0, yMin},Real2D{0, yMax}, yMin, yMax);


    mSectionGraph.addArtist(Slab::DummyPointer(sectionArtist));
    sectionArtist.addSection(line, Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0]);

    addWindow(Slab::DummyPointer(mFieldDisplay), ADD_TO_NEW_COLUMN, .25);
}

void R2toR::OutputOpenGL::draw() {
    ::Graphics::OpenGLMonitor::draw();

    if(sectionArtist.getFunction() == nullptr) {
        const R2toR::EquationState &fState = *lastData.getEqStateData<R2toR::EquationState>();
        auto &phi = fState.getPhi();

        sectionArtist.setFunction(Slab::DummyPointer(phi));
    }
}



