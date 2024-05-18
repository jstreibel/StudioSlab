//
// Created by joao on 28/05/2021.
//

#include "R2ToROutputOpenGlShockwaveAtT0.h"

#include <filesystem>
#include "Math/Function/R2toR/Model/Transform.h"
#include "R2toROutputOpenGLShockwave.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"


R2toR::OutputOpenGLShockwaveAt_t0::OutputOpenGLShockwaveAt_t0(const NumericConfig &params)
        : Graphics::OpenGLMonitor(params, "ℝ² ↦ ℝ shockwave at t₀ graphic monitor.")

{

}

void R2toR::OutputOpenGLShockwaveAt_t0::draw() {
    if(!lastData.hasValidData()) return;

    //const Real t0 = ((ParameterTemplate<Real>*)userParamMap["sw_t0"])->val;
    //const Real t = lastInfo.getT();
//
    //const R2toR::FieldState& fState = *lastInfo.getFieldData<R2toR::FieldState>();
    //auto &phi = fState.getPhi();
    //auto &dPhi = fState.getDPhiDt();
//
    //glDisable(GL_LINE_STIPPLE);
//
    //// *************************************************************************************************
    //// ********** SECOES (NOS GRAFICOS) ****************************************************************
    //glDisable(GL_DEPTH_TEST);
    //for(auto &section : sections){
    //    RtoR2::StraightLine line = getLine(section);
    //    Color color = getColor(section);
//
    //    Graph &graph = getPhiGraph(section);
    //    graph.DrawAxes();
//
    //    RtoR::FunctionRenderer::renderSection(phi, line, color, true, phi.getN());
//
    //    if(showAnalytic)
    //    {
    //        RtoR::AnalyticShockwave2DRadialSymmetry shockwave;
    //        shockwave.sett(t0+t);
    //        auto shockwave2d = R2toR::FunctionAzimuthalSymmetry(&shockwave, 1., 0., false,false);
    //        RtoR::FunctionRenderer::renderSection(shockwave2d, line, Color(1, .0, .0, 1.0), true, 1024, 2.0);
    //    }
//
    //    getDtPhiGraph(section).DrawAxes();
    //    RtoR::FunctionRenderer::renderSection(dPhi, line, color, true, phi.getN());
    //    if(showAnalytic)
    //    {
    //        RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtShockwave;
    //        ddtShockwave.sett(t0+t);
    //        auto ddtShockwave2d = R2toR::FunctionAzimuthalSymmetry(&ddtShockwave, 0., 0., false,
    //                                                               false);
//
    //        RtoR::FunctionRenderer::renderSection(ddtShockwave2d, line, Color(.0, 1, .0, 1.0),
    //                                              true, 1024, 2.0);
    //    }
    //}
//
    //// *************************************************************************************************
    //// ********** CAMPO INTEIRO (3D) ************************************************************************
    //const Real GLOBAL_Z_SCALE_PHI = 0.1;
    //const Real GLOBAL_Z_SCALE_DDTPHI = 0.1*GLOBAL_Z_SCALE_PHI;
    ////glEnable(GL_DEPTH_TEST);
    //phiGraph.BindWindow();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixf(zpr.getModelview());
//
    //xMin = phi.getDomain().xMin; xMax = phi.getDomain().xMax;
    //// yMin = phi.getDomain().yMin; yMax = phi.getDomain().yMax;
    //glColor3f(1.f, 1.f, 1.f);
    //RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, 250, 500, GLOBAL_Z_SCALE_PHI);
//
    //// Secoes sobre o campo inteiro
    //glLineWidth(3);
    //for(auto &section : sections){
    //    RtoR2::StraightLine &line = section.second.first;
    //    Color color = section.second.second;
//
    //    //glColor4d(color.r, color.g, color.b, color.a);
    //    glColor4d(color.r, color.g, color.b, 1);
//
    //    glBegin(GL_LINE_STRIP);
    //    {
    //        const int resolution = 2000;
    //        const Real step = 1. / resolution;
    //        for(Real s=0; s<=1; s+=step) {
    //            const Real2D x = line(s);
    //            Real val = phi(x);
    //            glVertex3d(x.x, x.y, GLOBAL_Z_SCALE_PHI*val);
    //        }
    //    }
    //    glEnd();
    //}
//
    //// Cone de luz
    //if(1) {
    //    const auto innerConeRadius = t0-t,
    //               outerConeRadius = t0+t;
//
    //    glLineWidth(1.5);
    //    glEnable(GL_LINE_STIPPLE);
    //    glLineStipple(1, 0xAAAA);
    //    glColor4d(0.5, 0, 0, 0.8);
    //    glBegin(GL_LINE_LOOP);
    //    for (Real theta = 0.0; theta < 2 * M_PI; theta += M_PI / 3000) {
    //        const Real x = innerConeRadius * cos(theta), y = innerConeRadius * sin(theta);
    //        glVertex3d(x, y, GLOBAL_Z_SCALE_PHI * phi({x, y}));
    //    }
    //    for (Real theta = 0.0; theta < 2 * M_PI; theta += M_PI / 3000) {
    //        const Real x = outerConeRadius * cos(theta), y = outerConeRadius * sin(theta);
    //        glVertex3d(x, y, GLOBAL_Z_SCALE_PHI * phi({x, y}));
    //    }
    //    glEnd();
    //    glDisable(GL_LINE_STIPPLE);
    //}
//
//
    //if(showDPhidt) {
    //    dPhiGraph.BindWindow();
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadMatrixf(zpr.getModelview());
    //    RtoR::FunctionRenderer::renderFunction(dPhi, xMin, xMin, xMax - xMin, 1024, 1024, GLOBAL_Z_SCALE_DDTPHI);
    //}
}

bool R2toR::OutputOpenGLShockwaveAt_t0::notifyKeyboardSpecial(int key, int x, int y) {
    //letc angle = 2.5e-3 * M_PI;
    if(key == GLUT_KEY_RIGHT) {
        //Rotation T(-angle);
        //for (auto &section : sections) {
        //    RtoR2::StraightLine &line = getLine(section);
        //    line = T * line;
        //}
    } else if(key == GLUT_KEY_LEFT){
        //Rotation T(angle);
        //for(auto &section : sections){
        //    RtoR2::StraightLine &line = getLine(section);
        //    line = T*line;
        //}
    }

    return true;
}

bool R2toR::OutputOpenGLShockwaveAt_t0::notifyKeyboard(unsigned char key, int x, int y) {
    if(key == '2'){
        showAnalytic = !showAnalytic;
        return true;
    }
    else if(key == ']'){
        yMin *= 1.1;
        yMax *= 1.1;
        return true;
    }
    else if(key == '['){
        yMin /= 1.1;
        yMax /= 1.1;
        return true;
    }
    else if(key == '}'){
        yMin *= 1.5;
        yMax *= 1.5;
        return true;
    }
    else if(key == '{'){
        yMin /= 1.5;
        yMax /= 1.5;
        return true;
    }
    else if(key == 'd'){
        showDPhidt = !showDPhidt;
        return true;
        //reshape(windowWidth, windowHeight);
    } else if(key == 13) { // enter
        // save snapshot to file
        this->_outputSnapshot();
        return true;
    }

    //for(auto& sec : sections){
    //    Base::GraphPair &graphs = sec.first;
//
    //    {
    //        auto &graph = graphs.first;
    //        graph.yMin = yMin;
    //        graph.yMax = yMax;
    //    }
//
    //    {
    //        auto &graph = graphs.second;
    //        graph.yMin = yMin;
    //        graph.yMax = yMax;
    //    }
    //}

    return false;
}

IntPair R2toR::OutputOpenGLShockwaveAt_t0::getWindowSizeHint() {
    return {1600, 1000};
}

bool R2toR::OutputOpenGLShockwaveAt_t0::notifyMouseButton(int button, int dir, int x, int y) {
    zpr.zprMouseButton(button, dir, x, y);

    return true;
}

bool R2toR::OutputOpenGLShockwaveAt_t0::notifyMouseMotion(int x, int y) {
    zpr.zprMouseMotion(x, y);

    return true;
}

void R2toR::OutputOpenGLShockwaveAt_t0::_outputSnapshot() {
    //letc *field = this->lastInfo.getFieldData<R2toR::FieldState>();
    //letc &phi = field->getPhi();
//
    ////letc t0 = ((ParameterTemplate<Real>*)userParamMap["sw_t0"])->val;
    //letc t = lastInfo.getT();
    //letc xMin = phi.getDomain().xMin;
    //letc yMin = phi.getDomain().yMin;
    //letc L = phi.getDomain().getLx();
//
    ////std::string sFileName = "./file-t0="+ToString(t0) + "-t=" + ToStr(t);
    //int count=0;
    //do count++; while(std::filesystem::exists(sFileName + (count>1?"-"+std::to_string(count):"") + ".dat"));
//
//
    //{
    //    letc N = phi.getN();
    //    letc dh = L/N;
//
    //    std::string ssFileName = sFileName + (count > 1 ? "-" + std::to_string(count) : "") + ".dat";
//
    //    std::ofstream file;
    //    file.open(ssFileName, std::ios::out);
//
    //    file << "# x   phi_#    phi_a" << std::endl;
//
    //    RtoR::AnalyticShockwave2DRadialSymmetry shockwave;
    //    shockwave.sett(t0+t);
//
    //    for (int i = 0; i < N; i++) {
    //        letc x = xMin + i * dh;
    //        letc f = phi({x, 0});
    //        file << x << " " << f << " " << shockwave(x) << std::endl;
    //    }
    //    file.close();
    //}
//
//
    //{
    //    letc dpi = 200;
    //    letc textWidthInches = 6.46;
    //    letc figWidthInches = textWidthInches/3;
    //    letc N = 200*textWidthInches;
    //    letc dh = L/N;
//
    //    std::string ssFileName = sFileName + (count > 1 ? "-" + std::to_string(count) : "") + "-fullSnapshot.dat";
//
    //    std::ofstream file;
    //    file.open(ssFileName, std::ios::out);
//
    //    file << "\n\n{\"L\":" << L << ", \"xMin\":" << xMin << ", \"N\": " << N << ",\n";
//
    //    file << "\"phi_n\": np.asarray((";
    //    for (int j = 0; j < N; j++) {
    //        file << "(";
    //        for (int i = 0; i < N; i++) {
    //            letc x = xMin+i*dh, y = yMin+j*dh;
    //            letc f = phi({x, y});
//
    //            file << f << ", ";
    //        }
    //        file << "), \n";
    //    }
//
    //    file << "))}" << std::endl;
    //    file.close();
    //}
}
