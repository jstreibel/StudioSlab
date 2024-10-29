//
// Created by joao on 4/1/23.
//

#include "Signal.h"

#include "JackServer.h"
#include "OutGL.h"
#include "Math/Numerics/Socket.h"
#include "RtoRBuilder.h"

#include <Math/Numerics/Output/OutputManager.h>



Real RtoR::Signal::xInitDampCutoff_normalized;
Real RtoR::Signal::dampFactor;
Vector<Real> damps;
Real jackProbeLocation=0;
size_t lastBufferDumpedSamplesCount = 0;
Real t0 = 5;





/***
 *         ____.                 __             _____              .___.__
 *        |    |_____     ____  |  | __        /  _  \   __ __   __| _/|__|  ____
 *        |    |\__  \  _/ ___\ |  |/ /       /  /_\  \ |  |  \ / __ | |  | /  _ \
 *    /\__|    | / __ \_\  \___ |    <       /    |    \|  |  // /_/ | |  |(  <_> )
 *    \________|(____  / \___  >|__|_ \      \____|__  /|____/ \____ | |__| \____/
 *                   \/      \/      \/              \/             \/
 */
RtoR::Signal::JackOutput::JackOutput(const NumericConfig &params)
: Math::Socket(params, "Jack output", 1) {
    JackServer::GetInstance();

    auto delta = xInitDampCutoff_normalized*params.getL();
    auto xLeft = params.getxMin();

    jackProbeLocation = xLeft+delta;
}
void RtoR::Signal::JackOutput::handleOutput(const OutputPacket &packet) {
    Function &field = packet.getEqStateData<RtoR::EquationState>()->getPhi();

    auto measure = field(jackProbeLocation);

    *JackServer::GetInstance() << measure;
}
bool RtoR::Signal::JackOutput::shouldOutput(Real t, unsigned long timestep) {
    return Socket::shouldOutput(t, timestep);
}


/***
 *    ________            __                    __        ___.           .__ .__       .___
 *    \_____  \   __ __ _/  |_ ______   __ __ _/  |_      \_ |__   __ __ |__||  |    __| _/  ____  _______
 *     /   |   \ |  |  \\   __\\____ \ |  |  \\   __\      | __ \ |  |  \|  ||  |   / __ | _/ __ \ \_  __ \
 *    /    |    \|  |  / |  |  |  |_> >|  |  / |  |        | \_\ \|  |  /|  ||  |__/ /_/ | \  ___/  |  | \/
 *    \_______  /|____/  |__|  |   __/ |____/  |__|        |___  /|____/ |__||____/\____ |  \___  > |__|
 *            \/               |__|                            \/                       \/      \/
 */
auto RtoR::Signal::OutputBuilder::buildOpenGLOutput() -> RtoR::Monitor * {
    const Real phiMin = -1.4;
    const Real phiMax = -phiMin;

    const Real xLeft = Numerics::Allocator::getInstance().getNumericParams().getxMin();
    const Real xRight = xLeft + Numerics::Allocator::getInstance().getNumericParams().getL();

    return new RtoR::Signal::OutGL (xLeft, xRight, phiMin, phiMax); }
OutputManager *RtoR::Signal::OutputBuilder::build(Str outputFileName) {
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
RtoR::Signal::BoundaryCondition::BoundaryCondition(Real f, Real A) : f(f), A(A) { }
void RtoR::Signal::BoundaryCondition::apply(RtoR::EquationState &function, Real t) const {
    auto jackServer = JackServer::GetInstance();

    if(bufferNumber < jackServer->getInputBufferUpdateCount()){
        currentBuffer = jackServer->getInputBuffer();
        bufferSize = jackServer->getInputBufferSize();
        bufferNumber = jackServer->getInputBufferUpdateCount();
        lastBufferDumpedSamplesCount = currentBufferLocation;
        currentBufferLocation=0;
    }

    if(t==0.0){
        function.setPhi(NullFunction());
        function.setDPhiDt(NullFunction());

        return;
    }

    auto &vec_phi = function.getPhi().getSpace().getHostData();
    auto &vec_dphidt = function.getDPhiDt().getSpace().getHostData();


    auto _t = t-t0;
    if(0) {
        static auto wallclock = Timer();
        auto wallt = wallclock.getElTime_sec();
        _t = wallt;
    }
    auto omega = 2 * M_PI * f;
    auto alpha = 1;
    if(_t>0)
    {
        auto val = A*sin(omega*_t)*exp(-alpha*_t);
        auto dval = A*omega*cos(omega*_t)*exp(-alpha*_t) - alpha*A*sin(omega*_t)*exp(-alpha*_t);

        vec_phi[0] = val;
        vec_dphidt[0] = dval;
    }

    if(0) {
        static auto lastSample = .0f;

        auto loc=currentBufferLocation;
        auto sLast = lastSample,
             sNew = currentBuffer[loc];
        auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
        vec_phi[0] = sNew;
        vec_dphidt[0] = (sNew-sLast)/dt;

        lastSample = sNew;
        currentBufferLocation+=8;
    }




    // DAMPING
    {
        auto &phi = function.getPhi();

        auto &params = Numerics::Allocator::getInstance().getNumericParams();
        auto L = params.getL();
        auto xLeft = params.getxMin();
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

            damps.push_back(factor);
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
    addParameters({&freq, &amplitude, &damping, &dampPercent}); }
auto RtoR::Signal::CLI::getBoundary() const -> const void * {
    const Real f = *freq;
    const Real A = *amplitude;

    dampFactor = *damping;
    xInitDampCutoff_normalized = 1-*dampPercent;

    return new RtoR::Signal::BoundaryCondition(f, A); }

RtoR::Monitor *RtoR::Signal::CLI::buildOpenGLOutput() {
    const Real phiMin = -1.4;
    const Real phiMax = -phiMin;

    const Real xLeft = numericParams.getxMin();
    const Real xRight = numericParams.getxMax();

    return new RtoR::Signal::OutGL(numericParams, phiMin, phiMax);
}
