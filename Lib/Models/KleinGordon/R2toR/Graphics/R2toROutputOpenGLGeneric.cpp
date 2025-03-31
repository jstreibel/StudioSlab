//

#include "R2toROutputOpenGLGeneric.h"

#include <chrono>

#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Models/KleinGordon/R2toR/EquationState.h"

#define ADD_TO_NEW_COLUMN true


namespace Slab::Models::KGR2toR {

    OutputOpenGL::OutputOpenGL(Count max_steps)
    : BaseMonitor(max_steps, "ℝ²↦ℝ OpenGL monitor", 1)
    , mSectionGraph("Sections")
    , mFieldDisplay("Field")
    {
        addWindow(Naked(mSectionGraph), ADD_TO_NEW_COLUMN, .50);
        mSectionGraph.addArtist(Slab::Naked(sectionArtist));

        addWindow(Naked(mFieldDisplay), ADD_TO_NEW_COLUMN, .25);
    }

    void OutputOpenGL::draw() {
        if (!lastPacket.hasValidData()) return;

        BaseMonitor::draw();

        if (sectionArtist.getFunction() == nullptr) {
            fix cat = lastPacket.getStateCategory();
            if (cat == "2nd-order|R2->R") {
                IN state = *lastPacket.GetNakedStateData<R2toR::EquationState>();

                auto &phi = state.getPhi();

                // TODO: this is insanely dangerous: this naked pointer could be gone at any moment afaik
                sectionArtist.setFunction(Naked(phi));
            }

            else if (cat == "2nd-order|R2->R") {
                NOT_IMPLEMENTED
                // IN state = *lastPacket.GetNakedStateData<Models::Stoch SPIStg::EquationState>();

                // auto &phi = state.getPhi();

                // TODO: this is insanely dangerous: this naked pointer could be gone at any moment afaik
                // sectionArtist.setFunction(Naked(phi));
            }


        } else if (sectionArtist.getSections().empty()) {
            IN state = *lastPacket.GetNakedStateData<R2toR::EquationState>();
            auto &phi = dynamic_cast<R2toR::NumericFunction&>(state.getPhi());

            auto yMin = phi.getDomain().yMin,
                 yMax = phi.getDomain().yMax;

            auto line = Slab::New<RtoR2::StraightLine>(Real2D{0, yMin}, Real2D{0, yMax}, yMin, yMax);

            sectionArtist.addSection(line, Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0].clone(), "section 1");
        }
    }


}