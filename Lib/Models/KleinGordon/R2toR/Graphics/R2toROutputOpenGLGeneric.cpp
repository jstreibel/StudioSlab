//

#include "R2toROutputOpenGLGeneric.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Models/KleinGordon/R2toR/EquationState.h"

#define ADD_TO_NEW_COLUMN true


namespace Slab::Math {

    R2toR::OutputOpenGL::OutputOpenGL(Real t_max, Count max_steps, Real x_min, Real x_max, Real y_min, Real y_max)
    : Graphics::BaseMonitor(t_max, max_steps, "R2 -> R OpenGL output", 1)
    , mSectionGraph(x_min, x_max, -1, 1, "Sections")
    , mFieldDisplay()
    {
        addWindow(Slab::Naked(mSectionGraph), ADD_TO_NEW_COLUMN, .50);

        auto yMin = y_min,
             yMax = y_max;

        auto line = Slab::New<RtoR2::StraightLine>(Real2D{0, yMin}, Real2D{0, yMax}, yMin, yMax);


        mSectionGraph.addArtist(Slab::Naked(sectionArtist));
        sectionArtist.addSection(line, Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0].clone(), "section 1");

        addWindow(Slab::Naked(mFieldDisplay), ADD_TO_NEW_COLUMN, .25);
    }

    void R2toR::OutputOpenGL::draw() {
        Graphics::BaseMonitor::draw();

        if (sectionArtist.getFunction() == nullptr) {
            IN state = *lastPacket.GetNakedStateData<R2toR::EquationState>();

            auto &phi = state.getPhi();

            // TODO: this is insanely dangerous: this naked pointer could be gone at any moment afaik
            sectionArtist.setFunction(Slab::Naked(phi));
        }
    }


}