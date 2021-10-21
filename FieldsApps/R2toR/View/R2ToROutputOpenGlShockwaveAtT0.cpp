//
// Created by joao on 28/05/2021.
//

#include "R2ToROutputOpenGlShockwaveAtT0.h"

#include <filesystem>
#include "FieldsApps/R2toR/Model/Transform.h"
#include "FieldsApps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "FieldsApps/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "R2toROutputOpenGLShockwave.h"
#include "FieldsApps/R2toR/Model/FieldState.h"

#include "View/Graphic/Tools/FunctionRenderer.h"


R2toR::OutputOpenGLShockwaveAt_t0::OutputOpenGLShockwaveAt_t0(UserParamMap userParamMap)
        : Base::OutputOpenGL(),
          userParamMap(userParamMap)
{
    yMax=1;yMin=-1;

    std::cout << "Initialized R2toR::OutputOpenGLShockwaveAt_t0." << std::endl;
}

void R2toR::OutputOpenGLShockwaveAt_t0::draw() {
    if(!lastInfo.hasValidData()) return;

    const Real t0 = ((ParameterTemplate<double>*)userParamMap["sw_t0"])->val;
    const Real t = lastInfo.getT();

    const R2toR::FieldState& fState = *lastInfo.getFieldData<R2toR::FieldState>();
    auto &phi = fState.getPhi();
    auto &dPhi = fState.getDPhiDt();

    glDisable(GL_LINE_STIPPLE);

    // *************************************************************************************************
    // ********** SECOES (NOS GRAFICOS) ****************************************************************
    glDisable(GL_DEPTH_TEST);
    for(auto &section : sections){
        RtoR2::StraightLine line = getLine(section);
        Color color = getColor(section);

        Graph &graph = getPhiGraph(section);
        graph.DrawAxes();

        RtoR::FunctionRenderer::renderSection(phi, line, color, true, phi.getN());

        if(showAnalytic)
        {
            RtoR::AnalyticShockwave2DRadialSymmetry shockwave;
            shockwave.sett(t0+t);
            auto shockwave2d = R2toR::FunctionAzimuthalSymmetry(&shockwave, 1., 0., false,false);
            RtoR::FunctionRenderer::renderSection(shockwave2d, line, Color(1, .0, .0, 1.0), true, 1024, 2.0);
        }

        getDtPhiGraph(section).DrawAxes();
        RtoR::FunctionRenderer::renderSection(dPhi, line, color, true, phi.getN());
        if(showAnalytic)
        {
            RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtShockwave;
            ddtShockwave.sett(t0+t);
            auto ddtShockwave2d = R2toR::FunctionAzimuthalSymmetry(&ddtShockwave, 0., 0., false,
                                                                   false);

            RtoR::FunctionRenderer::renderSection(ddtShockwave2d, line, Color(.0, 1, .0, 1.0),
                                                  true, 1024, 2.0);
        }
    }

    // *************************************************************************************************
    // ********** CAMPO INTEIRO (3D) ************************************************************************
    const Real GLOBAL_Z_SCALE_PHI = 0.1;
    const Real GLOBAL_Z_SCALE_DDTPHI = 0.1*GLOBAL_Z_SCALE_PHI;
    //glEnable(GL_DEPTH_TEST);
    phiGraph.BindWindow();
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(zpr.getModelview());

    xMin = phi.getDomain().xMin; xMax = phi.getDomain().xMax;
    // yMin = phi.getDomain().yMin; yMax = phi.getDomain().yMax;
    glColor3f(1.f, 1.f, 1.f);
    RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, 250, 500, GLOBAL_Z_SCALE_PHI);

    // Secoes sobre o campo inteiro
    glLineWidth(3);
    for(auto &section : sections){
        RtoR2::StraightLine &line = section.second.first;
        Color color = section.second.second;

        //glColor4d(color.r, color.g, color.b, color.a);
        glColor4d(color.r, color.g, color.b, 1);

        glBegin(GL_LINE_STRIP);
        {
            const int resolution = 2000;
            const Real step = 1. / resolution;
            for(Real s=0; s<=1; s+=step) {
                const Real2D x = line(s);
                Real val = phi(x);
                glVertex3d(x.x, x.y, GLOBAL_Z_SCALE_PHI*val);
            }
        }
        glEnd();
    }

    // Cone de luz
    if(1) {
        const auto innerConeRadius = t0-t,
                   outerConeRadius = t0+t;

        glLineWidth(1.5);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xAAAA);
        glColor4d(0.5, 0, 0, 0.8);
        glBegin(GL_LINE_LOOP);
        for (Real theta = 0.0; theta < 2 * M_PI; theta += M_PI / 3000) {
            const Real x = innerConeRadius * cos(theta), y = innerConeRadius * sin(theta);
            glVertex3d(x, y, GLOBAL_Z_SCALE_PHI * phi({x, y}));
        }
        for (Real theta = 0.0; theta < 2 * M_PI; theta += M_PI / 3000) {
            const Real x = outerConeRadius * cos(theta), y = outerConeRadius * sin(theta);
            glVertex3d(x, y, GLOBAL_Z_SCALE_PHI * phi({x, y}));
        }
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }


    if(showDPhidt) {
        dPhiGraph.BindWindow();
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(zpr.getModelview());
        RtoR::FunctionRenderer::renderFunction(dPhi, xMin, xMin, xMax - xMin, 1024, 1024, GLOBAL_Z_SCALE_DDTPHI);
    }
}

void R2toR::OutputOpenGLShockwaveAt_t0::reshape(int width, int height) {
    //ModelBase::OutputOpenGL::reshape(width, height);

    windowWidth = width;
    windowHeight = height;

    const Real minSize = std::min(Real(windowWidth-statsWindowWidth), Real(windowHeight));
    Real phiGraphY = 0;
    Real phiGraphH = minSize;
    if(showDPhidt){ phiGraphY = height/2; phiGraphH = minSize/2; }
    phiGraph =     Graph(statsWindowWidth, phiGraphY, minSize, phiGraphH, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    dPhiGraph =    Graph(statsWindowWidth, 0,        minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    zpr.zprReshape(phiGraph.winX, phiGraph.winY, phiGraph.winW, phiGraph.winH);

    const PosInt nSections = sections.size();
    if(!nSections) return;

    const Real secGraphHeight = windowHeight/(2.*nSections);
    Real y = windowHeight - secGraphHeight;
    for(auto& sec : sections){
        Base::GraphPair &graphs = sec.first;

        {
            auto &graph = graphs.first;
            graph.winX = phiGraph.winX + phiGraph.winW;
            graph.winY = y;
            graph.winW = windowWidth - graph.winX;
            graph.winH = secGraphHeight;
            graph.yMin = yMin;
            graph.yMax = yMax;
        }

        y-=secGraphHeight;

        {
            auto &graph = graphs.second;
            graph.winX = phiGraph.winX + phiGraph.winW;
            graph.winY = y;
            graph.winW = windowWidth - graph.winX;
            graph.winH = secGraphHeight;

            graph.yMin = yMin;
            graph.yMax = yMax;
        }

        y-=secGraphHeight;
    }
}

void R2toR::OutputOpenGLShockwaveAt_t0::notifyGLUTKeyboardSpecial(int key, int x, int y) {
    letc angle = 2.5e-3 * M_PI;
    if(key == GLUT_KEY_RIGHT) {
        Rotation T(-angle);
        for (auto &section : sections) {
            RtoR2::StraightLine &line = getLine(section);
            line = T * line;
        }
    } else if(key == GLUT_KEY_LEFT){
        Rotation T(angle);
        for(auto &section : sections){
            RtoR2::StraightLine &line = getLine(section);
            line = T*line;
        }
    }
}

void R2toR::OutputOpenGLShockwaveAt_t0::notifyGLUTKeyboard(unsigned char key, int x, int y) {
    if(key == '2'){
        showAnalytic = !showAnalytic;
        return;
    }
    else if(key == ']'){
        yMin *= 1.1;
        yMax *= 1.1;
    }
    else if(key == '['){
        yMin /= 1.1;
        yMax /= 1.1;
    }
    else if(key == '}'){
        yMin *= 1.5;
        yMax *= 1.5;
    }
    else if(key == '{'){
        yMin /= 1.5;
        yMax /= 1.5;
    }
    else if(key == 'd'){
        showDPhidt = !showDPhidt;
        reshape(windowWidth, windowHeight);
    } else if(key == 13) { // enter
        // save snapshot to file
        this->_outputSnapshot();
    }

    for(auto& sec : sections){
        Base::GraphPair &graphs = sec.first;

        {
            auto &graph = graphs.first;
            graph.yMin = yMin;
            graph.yMax = yMax;
        }

        {
            auto &graph = graphs.second;
            graph.yMin = yMin;
            graph.yMax = yMax;
        }
    }
}

IntPair R2toR::OutputOpenGLShockwaveAt_t0::getWindowSizeHint() {
    return {1600, 1000};
}

void R2toR::OutputOpenGLShockwaveAt_t0::addSection(const RtoR2::StraightLine &section, String name) {
    letc rangeField = 0.15;
    const Real rangeDdtField = 1.5;

    const Color color = Base::colors[sections.size()];

    Graph fieldGraph = Graph(phiGraph.winX + phiGraph.winW, .0,
                             windowWidth - statsWindowWidth, windowHeight, .0, 1., -rangeField, rangeField, name + " \\phi");
    Graph ddtFieldGraph = Graph(phiGraph.winX + phiGraph.winW, .0,
                                windowWidth - statsWindowWidth, windowHeight, .0, 1., -rangeDdtField, rangeDdtField, name + " D_t \\phi");
    sections.emplace_back(Base::GraphPair(fieldGraph, ddtFieldGraph), Base::LineAndColor(section, color));

}

void R2toR::OutputOpenGLShockwaveAt_t0::notifyGLUTMouseButton(int button, int dir, int x, int y) {
    zpr.zprMouseButton(button, dir, x, y);
}

void R2toR::OutputOpenGLShockwaveAt_t0::notifyGLUTMouseMotion(int x, int y) {
    zpr.zprMouseMotion(x, y);
}

void R2toR::OutputOpenGLShockwaveAt_t0::_outputSnapshot() {
    letc *field = this->lastInfo.getFieldData<R2toR::FieldState>();
    letc &phi = field->getPhi();

    letc t0 = ((ParameterTemplate<double>*)userParamMap["sw_t0"])->val;
    letc t = lastInfo.getT();
    letc xMin = phi.getDomain().xMin;
    letc yMin = phi.getDomain().yMin;
    letc L = phi.getDomain().getLx();

    std::string sFileName = "./file-t0="+ToString(t0) + "-t=" + ToString(t);
    int count=0;
    do count++; while(std::filesystem::exists(sFileName + (count>1?"-"+std::to_string(count):"") + ".dat"));


    {
        letc N = phi.getN();
        letc dh = L/N;

        std::string ssFileName = sFileName + (count > 1 ? "-" + std::to_string(count) : "") + ".dat";

        std::ofstream file;
        file.open(ssFileName, std::ios::out);

        file << "# x   phi_#    phi_a" << std::endl;

        RtoR::AnalyticShockwave2DRadialSymmetry shockwave;
        shockwave.sett(t0+t);

        for (int i = 0; i < N; i++) {
            letc x = xMin + i * dh;
            letc f = phi({x, 0});
            file << x << " " << f << " " << shockwave(x) << std::endl;
        }
        file.close();
    }


    {
        letc dpi = 200;
        letc textWidthInches = 6.46;
        letc figWidthInches = textWidthInches/3;
        letc N = 200*textWidthInches;
        letc dh = L/N;

        std::string ssFileName = sFileName + (count > 1 ? "-" + std::to_string(count) : "") + "-fullSnapshot.dat";

        std::ofstream file;
        file.open(ssFileName, std::ios::out);

        file << "\n\n{\"L\":" << L << ", \"xMin\":" << xMin << ", \"N\": " << N << ",\n";

        file << "\"phi_n\": np.asarray((";
        for (int j = 0; j < N; j++) {
            file << "(";
            for (int i = 0; i < N; i++) {
                letc x = xMin+i*dh, y = yMin+j*dh;
                letc f = phi({x, y});

                file << f << ", ";
            }
            file << "), \n";
        }

        file << "))}" << std::endl;
        file.close();
    }
}
