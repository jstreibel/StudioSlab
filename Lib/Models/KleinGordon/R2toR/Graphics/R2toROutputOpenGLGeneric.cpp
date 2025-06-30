//

#include "R2toROutputOpenGLGeneric.h"

#include <chrono>

#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Models/Stochastic-Path-Integral/SPI-State.h"

#define ADD_TO_NEW_COLUMN true


namespace Slab::Models::KGR2toR {

    auto getPhi(const OutputPacket& packet) {
        fix category = packet.getStateCategory();
        if (category == "2nd-order|R2->R") {
            IN state = *packet.GetNakedStateData<R2toR::EquationState>();

            // TODO: this is insanely dangerous: this naked pointer could be gone at any moment afaik
            return Naked( dynamic_cast<R2toR::FNumericFunction&>(state.getPhi()));
        }

        if (category == "1st-order|R2->R") {
            IN state = *packet.GetNakedStateData<StochasticPathIntegrals::SPIState>();

            return state.getPhi();
        }

        NOT_IMPLEMENTED
    }

    OutputOpenGL::OutputOpenGL(CountType max_steps)
    : BaseMonitor(max_steps, "ℝ²↦ℝ OpenGL monitor", 1)
    , mSectionGraph("Sections")
    , mFieldDisplay("Field")
    {
        AddWindow(Naked(mSectionGraph), ADD_TO_NEW_COLUMN, .50);
        mSectionGraph.AddArtist(Slab::Naked(sectionArtist));

        AddWindow(Naked(mFieldDisplay), ADD_TO_NEW_COLUMN, .25);
    }

    void OutputOpenGL::ImmediateDraw() {
        if (!LastPacket.hasValidData()) return;

        BaseMonitor::ImmediateDraw();

        if (sectionArtist.getFunction() == nullptr) {
            auto phi = getPhi(LastPacket);
            sectionArtist.setFunction(phi);

        } else if (sectionArtist.getSections().empty()) {
            fix phi = getPhi(LastPacket);

            auto yMin = phi->getDomain().yMin,
                 yMax = phi->getDomain().yMax;

            fix line = Slab::New<RtoR2::StraightLine>(Real2D{0, yMin}, Real2D{0, yMax}, yMin, yMax);

            sectionArtist.addSection(line, Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone(), "section 1");
        }
    }


}