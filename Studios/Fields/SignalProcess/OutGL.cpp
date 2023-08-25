//
// Created by joao on 4/5/23.
//

#include "Signal.h"
#include "OutGL.h"
#include "JackServer.h"

#include "Common/RandUtils.h"
#include "Common/UtilsCollection/Resample.h"
#include "3rdParty/imgui/imgui.h"

#include <Mappings/RtoR/Model/RtoRDiscreteFunctionCPU.h>

extern size_t lastBufferDumpedSamplesCount;
extern std::vector<Real> damps;
extern Real jackProbeLocation;
extern Real t0;
Real __t=0;



void RtoR::Signal::JackControl::draw( ) {
    Window::draw( );

    float w = float(this->w) - (float)2*this->winXoffset,
            h = float(this->h) - (float)2*this->winYoffset;
    float x = this->x+this->winXoffset;
    float y = this->y+this->winYoffset;

    bool closable=false;

    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("Jack control", &closable, flags);
    ImGui::SetWindowPos(ImVec2{x, y});
    ImGui::SetWindowSize(ImVec2{w, h});


    if (ImGui::Button("Generate pulse"))
    {
        t0 = __t;
    }



    static bool isRecording = false;

    if (ImGui::Button("Rec", {120, 60}))
        isRecording = JackServer::GetInstance()->toggleRecording();

    ImVec4 color = isRecording ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.2f, .0f, .0f, 1.0f);

    // Draw the circle
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 center = ImGui::GetItemRectMax();

    float radius = 25;
    center.x = w - 2*radius;
    center.y -= .5*ImGui::GetItemRectSize().y;
    drawList->AddCircleFilled(center, radius, ImColor(color));

    // Change button color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    // Draw the button
    ImGui::Button("##", ImVec2(radius * 2, radius * 2));

    // Restore button color
    ImGui::PopStyleColor(3);




    ImGui::End();
}


/***
 *    ________                             ________ .____
 *    \_____  \  ______    ____    ____   /  _____/ |    |
 *     /   |   \ \____ \ _/ __ \  /    \ /   \  ___ |    |
 *    /    |    \|  |_> >\  ___/ |   |  \\    \_\  \|    |___
 *    \_______  /|   __/  \___  >|___|  / \______  /|_______ \
 *            \/ |__|         \/      \/         \/         \/
 */
RtoR::Signal::OutGL::OutGL(const NumericConfig &params, Real phiMin, Real phiMax)
        : RtoR::Monitor(params, phiMin, phiMax) {

    panel.addWindow(&jackControlWindow);

    {
        auto &p = params;

        auto samples = (int) p.getN();
        mFieldsGraph = {p.getxMin(), p.getxMax(), phiMin, phiMax, "AAA", true, samples};
        panel.addWindow(&mFieldsGraph, true, 0.85);
    }

    {
        this->signalBufferGraph = GraphRtoR(0, 10, -1, 1, "Signal buffer", true, 2000);
        panel.addWindow(&this->signalBufferGraph);
    }

    {
        this->signalFullGraph = GraphRtoR(0, 1, -1.2, 1.2, "Signal", true, 2000);
        panel.addWindow(&this->signalFullGraph);
    }

    {
        this->fullRecordingGraph = GraphRtoR(0, 1, -1.2, 1.2, "Rec", true, 2000);
        panel.addWindow(&fullRecordingGraph);
    }
}
void RtoR::Signal::OutGL::handleOutput(const OutputPacket &packet) {
    // OutputOpenGL::_out(outInfo);

    auto field = packet.getEqStateData<RtoR::EquationState>();
    __t = packet.getSimTime();
    probingData.push_back(field->getPhi()(jackProbeLocation));
    gotNewData = true;

    if(!packet.getSteps() % 100) {
        auto newField =
                static_cast<RtoR::DiscreteFunction *>
                (packet.getEqStateData<RtoR::EquationState>()->getPhi().Clone());
        history.push_back(newField);
    }
}

#include <SOIL/SOIL.h>

void renderMatrix(const std::vector<std::vector<Real>>& matrix)
{
    // Get the size of the matrix
    int width = matrix.size();
    int height = matrix[0].size();

    // Convert the matrix to an array of floats
    std::vector<float> data(width * height * 4, 0.0f);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int index = (i * height + j) * 4;
            data[index] = static_cast<float>(matrix[i][j]);
            data[index + 1] = static_cast<float>(matrix[i][j]);
            data[index + 2] = static_cast<float>(matrix[i][j]);
            data[index + 3] = 1.0f;
        }
    }

    // Create a new texture using SOIL
    GLuint textureID = SOIL_create_OGL_texture(
            reinterpret_cast<unsigned char*>(data.data()),
            width, height, 4, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Render the quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Delete the texture
    glDeleteTextures(1, &textureID);
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
    stats.addVolatileStat(Str("Steps between calls to draw: ") + ToStr(deltaSteps));
    stats.addVolatileStat(ToStr(interval, 2) + "ms since last draw");
    stats.addVolatileStat("");
    stats.addVolatileStat("Integration:");
    stats.addVolatileStat(ToStr(stepsPerSecond, stepsPerSecond < 2 ? 10 : 0) + " steps/sec");
    stats.addVolatileStat(Str("t = ") + ToStr(getLastSimTime()));
    stats.addVolatileStat(Str("step ") + ToStr(lastData.getSteps()));
    stats.addVolatileStat("");

    auto jackServer = JackServer::GetInstance();
    Styles::Color samplingStatusColor = jackServer->isSubSampled()?Styles::Color{1,0,0,1}:Styles::Color{0,1,0,1};

    stats.addVolatileStat("Jack I/O:");
    stats.addVolatileStat(Str("Probed samples/nframes: ")
                          + ToStr(jackServer->getLastOutputProcessedSamples()) + "/"
                          + ToStr(jackServer->getnframes()), samplingStatusColor);
    stats.addVolatileStat(Str("Sampled input ")
                          + ToStr(lastBufferDumpedSamplesCount));
    stats.addVolatileStat(Str("Input buffer updates: "
                              + ToStr(jackServer->getInputBufferUpdateCount())));


    // *************************** RECORDING *******************************
    {
        static RtoR::FunctionArbitraryCPU *func = nullptr;

        std::vector<float> rec_f = JackServer::GetInstance()->getRecording();
        RealVector rec(rec_f.begin(), rec_f.end());
        auto recTime =  rec.size()/48000.;

        if(rec.size()){
            if(func != nullptr) delete func;

            func = new RtoR::FunctionArbitraryCPU(rec, .0, recTime);

            fullRecordingGraph.clearFunctions();
            fullRecordingGraph.addFunction(func);
            fullRecordingGraph.set_xMax(recTime);

            stats.addVolatileStat(Str("Recorded samples: ") + ToStr(func->N));
        }

    }


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
            auto &F1 = func1->getSpace().getHostData();

            for (int i = 0; i < probed.size(); i++) F1[i] = probed[i];

            signalBufferGraph.set_xMax(intervalSec);
            signalBufferGraph.addFunction(func1);
        }

        auto pdsize = probingData.size();
        if(pdsize > 0)
        {
            if (func2 != nullptr) delete func2;
            signalFullGraph.clearFunctions();

            auto t = lastData.getSimTime();
            func2 = new RtoR::FunctionArbitraryCPU(pdsize, 0, t);
            auto &F2 = func2->getSpace().getHostData();

            for (int i = 0; i < pdsize; i++) F2[i] = probingData[i];

            signalFullGraph.set_xMax(t > 1 ? t : 1);
            signalFullGraph.set_xMin(t > 1 ? t-1 : 0);
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
            auto &F = func.getSpace().getHostData();
            auto &FMore = funcMore.getSpace().getHostData();
            auto &FLess = funcLess.getSpace().getHostData();

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
        // signalBufferGraph.addFunction(&funcMore, Styles::Color(0, 1, 0, 1));
        //signalGraph.addFunction(&funcLess, Color(.3,.3,1,1));
    }

    // *************************** FIELD ***********************************
    if(gotNewData || 1)
    {
        const RtoR::EquationState &fieldState = *lastData.getEqStateData<RtoR::EquationState>();

        mFieldsGraph.clearFunctions();

        if (showPhi) {
            // const Color colorPhi = V_color;
//
            // //mFieldsGraph.addFunction(&energyCalculator.getPotential(), colorPhi, "|phi|");
            // auto &phi = fieldState.getPhi();
            // mFieldsGraph.addFunction(&phi, colorPhi, "phi");
        }

        if (showKineticEnergy) {
            //const Color colorKinetic = K_color;
//
            //// mFieldsGraph.addFunction(&energyCalculator.getKinetic(), colorKinetic);
            //mFieldsGraph.addFunction(&fieldState.getDPhiDt(), colorKinetic, "kinetic");
        }

        if (showGradientEnergy) {
            // const Color colorGradient = W_color;
//
            // mFieldsGraph.addFunction(&energyCalculator.getGradient(), colorGradient, "grad^2");
        }

        if (showEnergyDensity) {
            //const Color color = U_color;
            //mFieldsGraph.addFunction(&energyCalculator.getEnergy(), color, "E");
        }

        gotNewData = false;
    }

    panel.draw();

    {
        //auto L = Numerics::Allocator::getInstance().getNumericParams().getL();
        //static RtoR::FunctionArbitraryCPU* func = nullptr; if(func) delete func;
        //func = new RtoR::FunctionArbitraryCPU(damps, xInitDampCutoff_normalized*L, xMax);
//
        //mFieldsGraph.addFunction(func, Color(1,0,0,1));
//
        //glColor4f(0, 1, 0, 1);
        //glBegin(GL_LINES);
        //glVertex2d(jackProbeLocation, phiMin);
        //glVertex2d(jackProbeLocation, phiMax);
        //glEnd();

        renderMatrix(getHistoryMatrixData());
    }


}
auto RtoR::Signal::OutGL::getWindowSizeHint() -> IntPair {
    return {2600, 1700}; }

bool RtoR::Signal::OutGL::notifyKeyboard(unsigned char key, int x, int y) {
    EventListener::notifyKeyboard(key, x, y);

    if(key == 13) t0 = lastData.getSimTime();
}

std::vector<std::vector<Real>> RtoR::Signal::OutGL::getHistoryMatrixData() {
    auto M = history.size();

    std::vector<std::vector<Real>> outputData(M);
    for (int i=0; i<M; ++i) outputData[i] = history[i]->getSpace().getHostData();

    return outputData;
}
