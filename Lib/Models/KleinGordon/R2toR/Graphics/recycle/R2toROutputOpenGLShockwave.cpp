//


#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Graphics/Utils.h"
#include "Graphics/Plot2D/FunctionGraph_depr.h"
#include "Math/Function/RtoR2/StraightLine.h"
#include "R2toROutputOpenGLShockwave.h"

#include <filesystem>

R2toR::OutputOpenGLShockwave::OutputOpenGLShockwave(const NumericConfig &params)
                                  : Graphics::OpenGLMonitor(params, "ℝ² ↦ ℝ signum-Gordon shockwave graphic monitor")
{}

void R2toR::OutputOpenGLShockwave::draw() {
    if(!lastData.hasValidData()) return;

    //const DevFloat E_input = ((ParameterTemplate<Real>*)userParamMap["sw_E"])->val;
    //const DevFloat eps = ((ParameterTemplate<Real>*)userParamMap["sw_eps"])->val;
    //const DevFloat a = sqrt((4./3)*pi*eps*eps*E_input);
    //const DevFloat t = lastInfo.getT();
    //const DevFloat L = Allocator::getInstance().getNumericParams().getL();
    //const DevFloat coneRadius = t;
//
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
        //RtoR::FunctionRenderer::renderSection(phi, line, color, true, phi.getN());
//
    //    const DevFloat s = coneRadius/L + .5;
    //    const DevFloat H = 20;
//
    //    if(showAnalytic)
    //    {
    //        RtoR::AnalyticShockwave2DRadialSymmetry shockwave; shockwave.sett(t);
    //        auto shockwave2d = R2toR::FunctionAzimuthalSymmetry(&shockwave, 1., 0., false,false);
    //        RtoR::FunctionRenderer::renderSection(shockwave2d, line, Color(0.5, .0, .0, 1.0), true, 1024);
//
//
//
    //        // ********** CONE DE LUZ **********
    //        glBegin(GL_LINES);
    //        {
    //            glVertex2d(1-s, -H);
    //            glVertex2d(1-s,  H);
//
    //            glVertex2d(     s, -H);
    //            glVertex2d(     s,  H);
    //        }
    //        glEnd();
    //    }
//
    //    getDtPhiGraph(section).DrawAxes();
    //    RtoR::FunctionRenderer::renderSection(dPhi, line, color, true, phi.getN());
    //    if(showAnalytic)
    //    {
    //        RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtShockwave; ddtShockwave.sett(t);
    //        auto ddtShockwave2d = R2toR::FunctionAzimuthalSymmetry(&ddtShockwave, 0., 0., false,
    //                false);
//
    //        RtoR::FunctionRenderer::renderSection(ddtShockwave2d, line, Color(.0, .5, .0, 1.0),
    //                true, 1024);
//
    //        // ********** CONE DE LUZ **********
    //        glBegin(GL_LINES);
    //        {
    //            glVertex2d(1-s, -H);
    //            glVertex2d(1-s,  H);
//
    //            glVertex2d(s, -H);
    //            glVertex2d(s,  H);
    //        }
    //        glEnd();
    //    }
    //}

    // *************************************************************************************************
    // ********** CAMPO INTEIRO (3D) ************************************************************************
    //const DevFloat GLOBAL_Z_SCALE_PHI = 0.1;
    //const DevFloat GLOBAL_Z_SCALE_DDTPHI = 0.1;
    ////glEnable(GL_DEPTH_TEST);
    //phiGraph.BindWindow();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixf(zpr.getModelview());
//
    //xMin = phi.getDomain().xMin; xMax = phi.getDomain().xMax;
    //// yMin = phi.getDomain().yMin; yMax = phi.getDomain().yMax;
    //RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, 250, 500, GLOBAL_Z_SCALE_PHI);
    ////RtoR::AnalyticShockwave2DRadialSymmetry shockwave; shockwave.sett(t);
    ////auto shockwave2d = R2toR::FunctionAzimuthalSymmetry(&shockwave, 1., 0., false,false);
    ////RtoR::FunctionRenderer::renderFunction(shockwave2d, xMin, xMin, xMax - xMin, 250, 500, GLOBAL_Z_SCALE_PHI);
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
    //        const DevFloat step = 1. / resolution;
    //        for(DevFloat s=0; s<=1; s+=step) {
    //            const Real2D x = line(s);
    //            DevFloat val = phi(x);
    //            glVertex3d(x.x, x.y, GLOBAL_Z_SCALE_PHI*val);
    //        }
    //    }
    //    glEnd();
    //}
//
    //// Cone de luz
    //if(1) {
    //    glLineWidth(1.5);
    //    glEnable(GL_LINE_STIPPLE);
    //    glLineStipple(1, 0xAAAA);
    //    glColor4d(0.5, 0, 0, 0.8);
    //    glBegin(GL_LINE_LOOP);
    //    for (DevFloat theta = 0.0; theta < 2 * M_PI; theta += M_PI / 3000) {
    //        const DevFloat x = coneRadius * cos(theta), y = coneRadius * sin(theta);
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
    //    RtoR::FunctionRenderer::renderFunction(dPhi, xMin, xMin, xMax - xMin, 100, 100, GLOBAL_Z_SCALE_DDTPHI);
    //}
//
    //// OUTPUT TEXTO DADOS DA SIMULACAO
    //{
    //    std::ostringstream stat;
//
    //    addVolatileStat("");
    //    stat << "L=" << L;
    //    addVolatileStat(stat.str());
//
    //    stat.str("");
    //    stat << "a=" << a;
    //    addVolatileStat(stat.str());
//
    //    stat.str("");
    //    stat << "a=" << eps;
    //    addVolatileStat(stat.str());
//
    //}
//
    //// OUTPUT DA ENERGIA
    //if(1)
    //{
    //    std::ostringstream stat;
    //    #define sq(a) ((a)*(a))
    //    // Coordenadas cartesianas, numerico
//
    //    addVolatileStat("");
    //    addVolatileStat("Energia numerica:");
    //    if(1)
    //    {
    //        const auto N = phi.getSpace().getDim().getN(0);
    //        const auto M = phi.getSpace().getDim().getN(1);
    //        const auto h = phi.getSpace().geth();
    //        const auto invh_2=.5/h;
    //        const auto dx=h, dy=h;
    //        const auto invdx_2=.5/dx, invdy_2=.5/dy;
//
    //        DevFloat E = .0;
    //        // Desconsideramos as bordas do espaco de simulacao porque elas devem ser sempre zero e, quando isso nao
    //        // acontece, a simulacao passa a ser invalida (se por nenhum outro motivo, por definicao).
    //        for(auto x=-.5*L; x<.5*L; x+=dx){
    //            for(auto y=-.5*L; y<.5*L; y+=dy){
    //                const DevFloat DtPhi = dPhi({x, y});
    //                const DevFloat DxPhi = invdx_2 * (-phi({x-dx, y}) + phi({x+dx, y}));
    //                const DevFloat DyPhi = invdy_2 * (-phi({x,y-dy}) + phi({x,y+dy}));
    //                const DevFloat V = abs(phi({x,y}));
//
    //                E += .5*(sq(DtPhi) + sq(DxPhi) + sq(DyPhi)) + V;
    //            }
    //        }
    //        E *= dx*dy;
//
    //        stat << "Cartesian integration: E=" << E;
    //        addVolatileStat(stat.str());
//
    //        // NumericalIntegration test. We know this should integrate to 1
//
    //        auto f = R2toR::R2toRRegularDelta(eps, a);
    //        auto A1=0.0, A2=0.0;
    //        for(auto x=-.5*L; x<.5*L; x+=dx){
    //            for(auto y=-.5*L; y<.5*L; y+=dy){
    //                const auto v = f({x,y});
    //                A1 += v*v;
    //                A2 += v;
    //            }
    //        }
    //        A1 *= .5*dx*dy;
    //        A2 *= dx*dy;
    //        stat.str("");
    //        stat << "Test integration A1=" << A1;
    //        addVolatileStat(stat.str());
    //        stat.str("");
    //        stat << "Test integration A2=" << A2;
    //        addVolatileStat(stat.str());
    //        stat.str("");
    //        stat << "a=" << a;
    //        addVolatileStat(stat.str());
//
    //    }
//
    //    // Coordenadas radiais, computado da sol. comp. analitica
    //    if(0)
    //    {
    //        auto t = lastInfo.getT();
    //        auto shockwave = RtoR::AnalyticShockwave2DRadialSymmetry();
    //        auto ddtShockwave = RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB();
    //        shockwave.sett(t); ddtShockwave.sett(t);
//
    //        DevFloat E = 0.0;
    //        const DevFloat dr = 1.e-5;
    //        for(DevFloat r=0; r<=6; r+=dr){
    //            const DevFloat DtPhi = ddtShockwave(r);
    //            const DevFloat DrPhi = (.5/dr) * (-shockwave(r-dr) + shockwave(r));
//
    //            const DevFloat V = abs(shockwave(r));
//
    //            E += r*(.5 * (sq(DtPhi) + sq(DrPhi)) + V);
    //        }
    //        E *= 2*M_PI*dr;
    //        std::ostringstream stat;
    //        stat << "E_rad  = " << E;
    //        addVolatileStat(stat.str());
//
    //    }
//
    //    addVolatileStat("");
    //    stat.str("");
    //    addVolatileStat("Energia teorica:");
    //    stat << "E0 = " << (2./(9.*eps*eps));
    //    addVolatileStat(stat.str());
    //    stat.str("");
    //    stat << "E_interior  = " << (2./3)*M_PI*t*t;
    //    addVolatileStat(stat.str());
    //    stat.str("");
    //    stat << "E_input  = " << E_input;
    //    addVolatileStat(stat.str());
    //    #undef sq
    //}
}

bool R2toR::OutputOpenGLShockwave::notifyKeyboardSpecial(int key, int x, int y) {
    const DevFloat angle = 2.5e-3 * M_PI;
    if(key == GLUT_KEY_RIGHT) {
        Rotation T(-angle);
        //for (auto &section : sections) {
        //    RtoR2::StraightLine &line = getLine(section);
        //    line = T * line;
        //}
    } else if(key == GLUT_KEY_LEFT){
        Rotation T(angle);
        //for(auto &section : sections){
        //    RtoR2::StraightLine &line = getLine(section);
        //    line = T*line;
        //}
    }

    return false;
}

bool R2toR::OutputOpenGLShockwave::notifyKeyboard(unsigned char key, int x, int y) {
    //if(key == '2'){
    //    showAnalytic = !showAnalytic;
    //    return;
    //}
    //else if(key == ']'){
    //    yMin *= 1.1;
    //    yMax *= 1.1;
    //}
    //else if(key == '['){
    //    yMin /= 1.1;
    //    yMax /= 1.1;
    //}
    //else if(key == '}'){
    //    yMin *= 1.5;
    //    yMax *= 1.5;
    //}
    //else if(key == '{'){
    //    yMin /= 1.5;
    //    yMax /= 1.5;
    //}
    //else if(key == 'd'){
    //    showDPhidt = !showDPhidt;
    //    //reshape(windowWidth, windowHeight);
    //}
    //else if(key == 13){
    //    this->_outputSnapshot();
    //}
//
    //std::cout << yMin << "   " << yMax << std::endl;
//
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

IntPair R2toR::OutputOpenGLShockwave::getWindowSizeHint() {
    return {1600, 1000};
}

void R2toR::OutputOpenGLShockwave::addSection(const RtoR2::StraightLine &section, Str name) {
    // const DevFloat rangeField = 0.15;
    // const DevFloat rangeDdtField = 1.5;

    //const Color color = Base::colors[sections.size()];

    //Graph fieldGraph = Graph(phiGraph.winX + phiGraph.winW, .0,
    //        windowWidth - statsWindowWidth, windowHeight, .0, 1., -rangeField, rangeField, name + " \\phi");
    //Graph ddtFieldGraph = Graph(phiGraph.winX + phiGraph.winW, .0,
    //        windowWidth - statsWindowWidth, windowHeight, .0, 1., -rangeDdtField, rangeDdtField, name + " D_t \\phi");
    //sections.emplace_back(Base::GraphPair(fieldGraph, ddtFieldGraph), Base::LineAndColor(section, color));

}

bool R2toR::OutputOpenGLShockwave::notifyMouseButton(int button, int dir, int x, int y) {
    zpr.zprMouseButton(button, dir, x, y);

    return true;
}

bool R2toR::OutputOpenGLShockwave::notifyMouseMotion(int x, int y, int dx, int dy) {
    zpr.zprMouseMotion(x, y);

    return true;
}



void R2toR::OutputOpenGLShockwave::_outputSnapshot() {
    const auto *field = this->lastData.getEqStateData<R2toR::EquationState>();
    const auto &phi = field->getPhi();

    const auto t0 = 0;
    const auto t = lastData.getSimTime();
    const auto xMin = phi.getDomain().xMin;
    const auto yMin = phi.getDomain().yMin;
    const auto L = phi.getDomain().getLx();

    std::string sFileName = "./file-t0=" + ToStr(t0) + "-t=" + ToStr(t);
    int count=0;
    do count++; while(std::filesystem::exists(sFileName + (count>1?"-"+std::to_string(count):"") + ".dat"));


    {
        const auto N = phi.getN();
        const auto dh = L/N;

        std::string ssFileName = sFileName + (count > 1 ? "-" + std::to_string(count) : "") + ".dat";

        std::ofstream file;
        file.open(ssFileName, std::ios::out);

        file << "# x   phi_#    phi_a" << std::endl;

        RtoR::AnalyticShockwave2DRadialSymmetry shockwave;
        shockwave.sett(t0+t);

        for (int i = 0; i < N; i++) {
            const auto x = xMin + i * dh;
            const auto f = phi({x, 0});
            file << x << " " << f << " " << shockwave(x) << std::endl;
        }
        file.close();
    }


    {
        const auto dpi = 200;
        const auto textWidthInches = 6.46;
        const auto figWidthInches = textWidthInches/3;
        const auto N = 200*textWidthInches;
        const auto dh = (DevFloat)L/N;

        std::string ssFileName = sFileName + (count > 1 ? "-" + std::to_string(count) : "") + "-fullSnapshot.dat";

        std::ofstream file;
        file.open(ssFileName, std::ios::out);

        file << "\n\n{\"L\":" << L << ", \"xMin\":" << xMin << ", \"N\": " << N << ",\n";

        file << "\"phi_n\": np.asarray((";
        for (int j = 0; j < N; j++) {
            file << "(";
            for (int i = 0; i < N; i++) {
                const auto x = xMin+i*dh, y = yMin+j*dh;
                const auto f = phi({x, y});

                file << f << ", ";
            }
            file << "), \n";
        }

        file << "))}" << std::endl;
        file.close();
    }
}

bool
R2toR::OutputOpenGLShockwave::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
    return GUIEventListener::notifyMouseButton(button, state, keys);
}

bool R2toR::OutputOpenGLShockwave::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
    return OpenGLMonitor::notifyKeyboard(key, state, modKeys);
}
