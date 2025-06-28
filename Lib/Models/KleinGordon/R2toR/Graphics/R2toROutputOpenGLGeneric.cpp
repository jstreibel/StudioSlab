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
            return Naked( dynamic_cast<R2toR::NumericFunction&>(state.getPhi()));
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
        addWindow(Naked(mSectionGraph), ADD_TO_NEW_COLUMN, .50);
        mSectionGraph.addArtist(Slab::Naked(sectionArtist));

        addWindow(Naked(mFieldDisplay), ADD_TO_NEW_COLUMN, .25);
    }

    void OutputOpenGL::draw() {
        if (!lastPacket.hasValidData()) return;

        BaseMonitor::draw();

        if (sectionArtist.getFunction() == nullptr) {
            auto phi = getPhi(lastPacket);
            sectionArtist.setFunction(phi);

        } else if (sectionArtist.getSections().empty()) {
            fix phi = getPhi(lastPacket);

            auto yMin = phi->getDomain().yMin,
                 yMax = phi->getDomain().yMax;

            fix line = Slab::New<RtoR2::StraightLine>(Real2D{0, yMin}, Real2D{0, yMax}, yMin, yMax);

            sectionArtist.addSection(line, Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0].clone(), "section 1");
        }
    }


}