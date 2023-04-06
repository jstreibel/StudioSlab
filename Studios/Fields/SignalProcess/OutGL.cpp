//
// Created by joao on 4/5/23.
//

#include "Signal.h"
#include "JackServer.h"
#include "Phys/Thermal/Utils/RandUtils.h"
#include "Studios/Tools/Resample.h"

#include <Fields/Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h>

extern size_t lastBufferDumpedSamplesCount;
extern Real RtoR::Signal::xInitDampCutoff_normalized;
extern Real RtoR::Signal::dampFactor;
extern std::vector<std::pair<Real,Real>> damps;
extern Real jackProbeLocation;

/***
 *    ________                             ________ .____
 *    \_____  \  ______    ____    ____   /  _____/ |    |
 *     /   |   \ \____ \ _/ __ \  /    \ /   \  ___ |    |
 *    /    |    \|  |_> >\  ___/ |   |  \\    \_\  \|    |___
 *    \_______  /|   __/  \___  >|___|  / \______  /|_______ \
 *            \/ |__|         \/      \/         \/         \/
 */
RtoR::Signal::OutGL::OutGL(Real xMin, Real xMax, Real phiMin, Real phiMax)
        : RtoR::OutputOpenGL() {
    nStepsBetweenRecordings = 1;

    RtoR::OutputOpenGL::xMax = xMax;
    RtoR::OutputOpenGL::xMin = xMin;
    RtoR::OutputOpenGL::phiMax = phiMax;
    RtoR::OutputOpenGL::phiMin = phiMin;

    Window *window = nullptr;

    window = new Window; window->addArtist(&this->stats);
    panel->addWindow(window);

    {
        auto samples = (int) Numerics::Allocator::getInstance().getNumericParams().getN();
        this->mFieldsGraph = {xMin, xMax, phiMin, phiMax, "AAA", true, samples};
        window = new Window;
        window->addArtist(&this->mFieldsGraph);
        panel->addWindow(window, true, 0.85);
        this->fieldWindow = window;
    }

    {
        this->signalBufferGraph = GraphRtoR(0, 10, -1, 1, "Signal buffer", true, 2000);
        window = new Window;
        window->addArtist(&this->signalBufferGraph);
        panel->addWindow(window);
        this->signalBufferWindow = window;
    }

    {
        this->signalFullGraph = GraphRtoR(0, 1, -1.2, 1.2, "Signal", true, 2000);
        window = new Window;
        window->addArtist(&this->signalFullGraph);
        panel->addWindow(window);
        this->signalFullWindow = window;
    }



    const auto faktor = 120;
    phiMinAnim = new Animation(&mFieldsGraph.yMin, mFieldsGraph.yMin, faktor);
    phiMaxAnim = new Animation(&mFieldsGraph.yMax, mFieldsGraph.yMax, faktor);
    addAnimation(phiMaxAnim);
    addAnimation(phiMinAnim);

    xMinAnim = new Animation(&mFieldsGraph.xMin, mFieldsGraph.xMin, faktor);
    xMaxAnim = new Animation(&mFieldsGraph.xMax, mFieldsGraph.xMax, faktor);
    addAnimation(xMinAnim);
    addAnimation(xMaxAnim);

}
void RtoR::Signal::OutGL::_out(const OutputPacket &outInfo) {
    // OutputOpenGL::_out(outInfo);

    auto field = outInfo.getFieldData<RtoR::FieldState>();
    probingData.push_back(field->getPhi()(jackProbeLocation));
    gotNewData = true;
}
void RtoR::Signal::OutGL::draw() {
    static size_t lastStep = 0;

    auto deltaSteps = 0;
    if(gotNewData) {
        deltaSteps = lastData.getSteps() - lastStep;
        lastStep = lastData.getSteps();
    }

    auto interval = frameTimer.getElTime_msec();

    auto stepsPerSecond = 1e3*deltaSteps/interval;

    stats.addVolatileStat("Rendering:");
    stats.addVolatileStat(String("Steps between calls to draw: ") + ToString(deltaSteps));
    stats.addVolatileStat(ToString(interval, 2) + "ms since last draw");
    stats.addVolatileStat("");
    stats.addVolatileStat("Integration:");
    stats.addVolatileStat(ToString(stepsPerSecond, stepsPerSecond<2?10:0) + " steps/sec");
    stats.addVolatileStat(String("t = ") + ToString(getLastSimTime()));
    stats.addVolatileStat(String("step ") + ToString(lastData.getSteps()));
    stats.addVolatileStat("");

    auto jackServer = JackServer::GetInstance();
    Color samplingStatusColor = jackServer->isSubSampled()?Color{1,0,0,1}:Color{0,1,0,1};

    stats.addVolatileStat("Jack I/O:");
    stats.addVolatileStat(String("Probed samples/nframes: ")
                          + ToString(jackServer->getLastOutputProcessedSamples()) + "/"
                          + ToString(jackServer->getnframes()), samplingStatusColor);
    stats.addVolatileStat(String("Sampled input ")
                          + ToString(lastBufferDumpedSamplesCount));
    stats.addVolatileStat(String("Input buffer updates: "
                                 + ToString(jackServer->getInputBufferUpdateCount())));

    // *************************** SIGNAL TESTS ****************************
    if(1) {
        static RtoR::FunctionArbitraryCPU *func1 = nullptr,
                                          *func2 = nullptr;

        auto probed = JackServer::GetInstance()->getLastOutputData();

        const auto intervalSec = 1. / (48000 / 1024.);
        if(probed.size()>0) {
            if (func1 != nullptr) delete func1;
            signalBufferGraph.clearFunctions();

            func1 = new RtoR::FunctionArbitraryCPU(probed.size(), .0, intervalSec);
            auto &F1 = func1->getSpace().getX();

            for (int i = 0; i < probed.size(); i++) F1[i] = probed[i];

            signalBufferGraph.xMax = intervalSec;
            signalBufferGraph.addFunction(func1);
        }

        auto pdsize = probingData.size();
        if(pdsize > 0)
        {
            if (func2 != nullptr) delete func2;
            signalFullGraph.clearFunctions();

            auto t = lastData.getSimTime();
            func2 = new RtoR::FunctionArbitraryCPU(pdsize, 0, t);
            auto &F2 = func2->getSpace().getX();

            for (int i = 0; i < pdsize; i++) F2[i] = probingData[i];

            signalFullGraph.xMax = t > 1 ? t : 1;
            signalFullGraph.xMin = t > 1 ? t-1 : 0;
            signalFullGraph.addFunction(func2);
        }
    }
    else
    {
        auto L = 10.;
        auto N = 50, NMore = 800, NLess = 10;
        static auto func =     RtoR::FunctionArbitraryCPU(N, .0, L);
        static auto funcMore = RtoR::FunctionArbitraryCPU(NMore, .0, L);
        static auto funcLess = RtoR::FunctionArbitraryCPU(NLess, .0, L);
        static bool started = false;

        if(!started){
            auto &F = func.getSpace().getX();
            auto &FMore = funcMore.getSpace().getX();
            auto &FLess = funcLess.getSpace().getX();

            for(auto &f : F) f = RandUtils::random(0.25,.7);

            std::vector<float> in(F.size());
            for(auto i=0; i<func.N; i++) in[i] = F[i];

            {
                auto floats = Studios::Utils::resample(in, NMore);
                for (auto i = 0; i < floats.size(); i++) FMore[i] = floats[i];
            } {
                auto floats = Studios::Utils::resample(in, NLess);
                for (auto i = 0; i < floats.size(); i++) FLess[i] = floats[i];
            }

            started = true;
        }
        signalBufferGraph.clearFunctions();
        signalBufferGraph.addFunction(&func);
        signalBufferGraph.addFunction(&funcMore, Color(0, 1, 0, 1));
        //signalGraph.addFunction(&funcLess, Color(.3,.3,1,1));
    }

    // *************************** FIELD ***********************************
    if(gotNewData || 1)
    {
        const RtoR::FieldState &fieldState = *lastData.getFieldData<RtoR::FieldState>();
        if (&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";

        mFieldsGraph.clearFunctions();

        if (showPhi) {
            const Color colorPhi = V_color;

            //mFieldsGraph.addFunction(&energyCalculator.getPotential(), colorPhi, "|phi|");
            auto &phi = fieldState.getPhi();
            mFieldsGraph.addFunction(&phi, colorPhi, "phi");
        }

        if (showKineticEnergy) {
            const Color colorKinetic = K_color;

            // mFieldsGraph.addFunction(&energyCalculator.getKinetic(), colorKinetic);
            mFieldsGraph.addFunction(&fieldState.getDPhiDt(), colorKinetic, "kinetic");
        }

        if (showGradientEnergy) {
            const Color colorGradient = W_color;

            mFieldsGraph.addFunction(&energyCalculator.getGradient(), colorGradient, "grad^2");
        }

        if (showEnergyDensity) {
            const Color color = U_color;
            mFieldsGraph.addFunction(&energyCalculator.getEnergy(), color, "E");
        }

        gotNewData = false;
    }

    panel->draw(true, true);

    {
        fieldWindow->setupViewport(false, false);
        mFieldsGraph.setupOrtho();
        glColor4f(1, 0, 0, 1);

        glBegin(GL_LINE_STRIP);
        for (auto &p: damps) {
            glVertex2d(p.first, 1e2 * p.second);
        }
        glEnd();

        glColor4f(0, 1, 0, 1);
        glBegin(GL_LINES);
        glVertex2d(jackProbeLocation, phiMin);
        glVertex2d(jackProbeLocation, phiMax);
        glEnd();
    }


}
auto RtoR::Signal::OutGL::getWindowSizeHint() -> IntPair {
    return {2600, 1700}; }

