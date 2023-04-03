//
// Created by joao on 4/1/23.
//

#include "Signal.h"

#include "JackServer.h"

#include <Phys/Numerics/Output/OutputManager.h>

#include <boost/timer/timer.hpp>



Real RtoR::Signal::xInitDampCutoff_normalized;
Real RtoR::Signal::dampFactor;
std::vector<std::pair<Real,Real>> damps;
Real jackProbeLocation=0;

typedef boost::timer::cpu_timer CPUTimer;
typedef boost::timer::cpu_times CPUTimes;




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
    RtoR::OutputOpenGL::xMax = xMax;
    RtoR::OutputOpenGL::xMin = xMin;
    RtoR::OutputOpenGL::phiMax = phiMax;
    RtoR::OutputOpenGL::phiMin = phiMin;

    Window *window = nullptr;

    window = new Window; window->addArtist(&this->stats);
    panel->addWindow(window);

    auto samples = (int)Numerics::Allocator::getInstance().getNumericParams().getN();
    this->mFieldsGraph = {xMin, xMax, phiMin, phiMax, "AAA", true, samples};
    window = new Window; window->addArtist(&this->mFieldsGraph);
    panel->addWindow(window, true, 0.85);
    this->graphWindow = window;

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
void RtoR::Signal::OutGL::draw() {
    static auto timer = CPUTimer();
    static size_t lastStep = 0;

    timer.stop();
    double interval = timer.elapsed().wall*1e-6; // nsec -> milisec
    auto deltaSteps = lastInfo.getSteps() - lastStep;
    auto stepsPerSecond = 1e3*deltaSteps/interval;

    stats.addVolatileStat("");

    stats.addVolatileStat(ToString(interval, 2) + "ms since last draw");
    stats.addVolatileStat(ToString(stepsPerSecond, 2) + "steps/sec");
    stats.addVolatileStat(std::string("t = ") + std::to_string(getLastSimTime()));
    stats.addVolatileStat(std::string("step ") + std::to_string(lastInfo.getSteps()));
    stats.addVolatileStat(std::string("delta step: ") + std::to_string(deltaSteps));

    auto jackServer = JackServer::GetInstance();
    Color samplingStatusColor = jackServer->isSubSampled()?Color{1,0,0,1}:Color{0,1,0,1};

    stats.addVolatileStat(String("samples/nframes: ")
                       + ToString(jackServer->getLastOutputProcessedSamples()) + "/"
                       + ToString(jackServer->getnframes()), samplingStatusColor);
    stats.addVolatileStat(String("In buffer updates: "
                       + ToString(jackServer->getInputBufferUpdateCount())));

    lastStep = lastInfo.getSteps();


    // *************************** FIELD ***********************************
    //mFieldsGraph.draw();
    const RtoR::FieldState &fieldState = *lastInfo.getFieldData<RtoR::FieldState>();
    if(&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";

    mFieldsGraph.clearFunctions();

    if(showPhi){
        const Color colorPhi = V_color;

        //mFieldsGraph.addFunction(&energyCalculator.getPotential(), colorPhi, "|phi|");
        auto &phi = fieldState.getPhi();
        mFieldsGraph.addFunction(&phi, colorPhi, "phi");
    }

    if(showKineticEnergy){
        const Color colorKinetic = K_color;

        // mFieldsGraph.addFunction(&energyCalculator.getKinetic(), colorKinetic);
        mFieldsGraph.addFunction(&fieldState.getDPhiDt(), colorKinetic, "kinetic");
    }

    if(showGradientEnergy) {
        const Color colorGradient = W_color;

        mFieldsGraph.addFunction(&energyCalculator.getGradient(), colorGradient, "grad^2");
    }

    if(showEnergyDensity){
        const Color color = U_color;
        mFieldsGraph.addFunction(&energyCalculator.getEnergy(), color, "E");
    }

    panel->draw(true, true);

    {
        graphWindow->setupViewport(false, false);
        mFieldsGraph.setupOrtho();
        glColor4f(1,0,0,1);

        glBegin(GL_LINE_STRIP);
        for (auto &p : damps) {
            glVertex2d(p.first, 1e2*p.second);
        }
        glEnd();

        glColor4f(0,1,0,1);
        glBegin(GL_LINES);
        glVertex2d(jackProbeLocation, phiMin);
        glVertex2d(jackProbeLocation, phiMax);
        glEnd();
    }

    timer = CPUTimer();
}
auto RtoR::Signal::OutGL::getWindowSizeHint() -> IntPair {
    return {3200, 1200}; }




/***
 *         ____.                 __             _____              .___.__
 *        |    |_____     ____  |  | __        /  _  \   __ __   __| _/|__|  ____
 *        |    |\__  \  _/ ___\ |  |/ /       /  /_\  \ |  |  \ / __ | |  | /  _ \
 *    /\__|    | / __ \_\  \___ |    <       /    |    \|  |  // /_/ | |  |(  <_> )
 *    \________|(____  / \___  >|__|_ \      \____|__  /|____/ \____ | |__| \____/
 *                   \/      \/      \/              \/             \/
 */
RtoR::Signal::JackOutput::JackOutput() : OutputChannel("Jack output", 1) {
    JackServer::GetInstance();

    auto params = Numerics::Allocator::getInstance().getNumericParams();
    auto delta = xInitDampCutoff_normalized*params.getL();
    auto xLeft = params.getxLeft();

    jackProbeLocation = xLeft+delta;
}
void RtoR::Signal::JackOutput::_out(const OutputPacket &outputPacket) {
    Function &field = outputPacket.getFieldData<RtoR::FieldState>()->getPhi();

    auto measure = field(jackProbeLocation);

    *JackServer::GetInstance() << measure;
}
bool RtoR::Signal::JackOutput::shouldOutput(double t, unsigned long timestep) {
    return OutputChannel::shouldOutput(t, timestep);
}



/***
 *    ________            __                    __        ___.           .__ .__       .___
 *    \_____  \   __ __ _/  |_ ______   __ __ _/  |_      \_ |__   __ __ |__||  |    __| _/  ____  _______
 *     /   |   \ |  |  \\   __\\____ \ |  |  \\   __\      | __ \ |  |  \|  ||  |   / __ | _/ __ \ \_  __ \
 *    /    |    \|  |  / |  |  |  |_> >|  |  / |  |        | \_\ \|  |  /|  ||  |__/ /_/ | \  ___/  |  | \/
 *    \_______  /|____/  |__|  |   __/ |____/  |__|        |___  /|____/ |__||____/\____ |  \___  > |__|
 *            \/               |__|                            \/                       \/      \/
 */
auto RtoR::Signal::OutputBuilder::buildOpenGLOutput() -> RtoR::OutputOpenGL * {
    const double phiMin = -1.4;
    const double phiMax = -phiMin;

    const Real xLeft = Numerics::Allocator::getInstance().getNumericParams().getxLeft();
    const Real xRight = xLeft + Numerics::Allocator::getInstance().getNumericParams().getL();

    return new RtoR::Signal::OutGL (xLeft, xRight, phiMin, phiMax); }
OutputManager *RtoR::Signal::OutputBuilder::build(String outputFileName) {
    auto *builder = OutputStructureBuilderRtoR::build(outputFileName);

    JackOutput *jack = new JackOutput();
    builder->addOutputChannel(jack);

    return builder;

}




/***
 *    __________                            .___                                                           .___.__   __   .__
 *    \______   \  ____   __ __   ____    __| _/_____   _______  ___.__.         ____    ____    ____    __| _/|__|_/  |_ |__|  ____    ____
 *     |    |  _/ /  _ \ |  |  \ /    \  / __ | \__  \  \_  __ \<   |  |       _/ ___\  /  _ \  /    \  / __ | |  |\   __\|  | /  _ \  /    \
 *     |    |   \(  <_> )|  |  /|   |  \/ /_/ |  / __ \_ |  | \/ \___  |       \  \___ (  <_> )|   |  \/ /_/ | |  | |  |  |  |(  <_> )|   |  \
 *     |______  / \____/ |____/ |___|  /\____ | (____  / |__|    / ____|        \___  > \____/ |___|  /\____ | |__| |__|  |__| \____/ |___|  /
 *            \/                     \/      \/      \/          \/                 \/              \/      \/                             \/
 */
RtoR::Signal::BoundaryCondition::BoundaryCondition(double T, double A) : T(T), A(A) {

}
void RtoR::Signal::BoundaryCondition::apply(RtoR::FieldState &function, Real t) const {
    if(t==0.0){
        function.setPhi(NullFunction());
        function.setDPhiDt(NullFunction());

        return;
    }

    auto &vec_phi = function.getPhi().getSpace().getX();
    auto &vec_dphidt = function.getDPhiDt().getSpace().getX();


    if (t < T && 0) {
        auto omega = 2 * M_PI / T;
        auto val = A * sin(omega * t);
        auto dval = A * omega * cos(omega * t);

        vec_phi[0] = val;
        vec_dphidt[0] = dval;
    } else {
        vec_phi[0] = 0;
        vec_dphidt[0] = 0;
    }

    static auto lastSample = .0;
    auto sample = JackServer::GetInstance()->getInputBuffer();
    const auto s1 = 5e0*(double)sample[0], s2 = lastSample;
    vec_phi[0] = lastSample;
    vec_dphidt[0] = s2-s1;
    lastSample = s1;



    // DAMPING
    {
        auto &phi = function.getPhi();

        auto &params = Numerics::Allocator::getInstance().getNumericParams();
        auto L = params.getL();
        auto xLeft = params.getxLeft();
        auto N = function.getPhi().getSpace().getDim().getN(0);
        auto delta = L * (1 - xInitDampCutoff_normalized);
        auto xInit = xLeft + xInitDampCutoff_normalized * L;
        auto i0 = function.getPhi().mapPosToInt(xInit);
        auto dx = params.geth();

        damps.clear();
        for (auto i = i0; i < N; ++i) {
            auto x = phi.mapIntToPos(i) - dx;

            auto s = (x - xInit) / delta;
            auto factor = dampFactor * s;

            auto &phi = vec_phi[i];
            auto &dphi = vec_dphidt[i];

            phi -= 0.1 * phi * factor;
            dphi -= dphi * factor;

            damps.push_back({x, factor});
        }
    }

    vec_phi.back() = .0;
    vec_dphidt.back() = .0;
}




/***
 *    _________  .____     .___
 *    \_   ___ \ |    |    |   |
 *    /    \  \/ |    |    |   |
 *    \     \____|    |___ |   |
 *     \______  /|_______ \|___|
 *            \/         \/
 */
RtoR::Signal::CLI::CLI() : RtoRBCInterface("(1+1)-d Signal studies platform.", "gh", new OutputBuilder) {
    addParameters({&period, &amplitude, &damping, &dampPercent}); }
auto RtoR::Signal::CLI::getBoundary() const -> const void * {
    const double T = *period;
    const double A = *amplitude;

    dampFactor = *damping;
    xInitDampCutoff_normalized = 1-*dampPercent;

    return new RtoR::Signal::BoundaryCondition(T, A); }
