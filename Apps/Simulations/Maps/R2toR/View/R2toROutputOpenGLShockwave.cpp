//

#include <Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h>
#include "Apps/Simulations/Maps/R2toR/Model/Transform.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "R2toROutputOpenGLShockwave.h"
#include "Apps/Simulations/Maps/R2toR/Model/FieldState.h"

#include "View/Graphic/Tools/FunctionRenderer.h"

#include <filesystem>

R2toR::OutputOpenGLShockwave::OutputOpenGLShockwave(UserParamMap userParamMap)
                                  : Base::OutputOpenGL(),
                                    userParamMap(userParamMap)
{
    yMax=1;yMin=-1;

    std::cout << "Initialized R2toRMap::OutputOpenGL." << std::endl;
}

void R2toR::OutputOpenGLShockwave::draw() {
    if(!lastInfo.hasValidData()) return;

    const Real E_input = ((ParameterTemplate<double>*)userParamMap["sw_E"])->val;
    const Real eps = ((ParameterTemplate<double>*)userParamMap["sw_eps"])->val;
    const Real a = sqrt((4./3)*pi*eps*eps*E_input);
    const Real t = lastInfo.getT();
    const Real L = Allocator::getInstance().getNumericParams().getL();
    const Real coneRadius = t;


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

        const Real s = coneRadius/L + .5;
        const Real H = 20;

        if(showAnalytic)
        {
            RtoR::AnalyticShockwave2DRadialSymmetry shockwave; shockwave.sett(t);
            auto shockwave2d = R2toR::FunctionAzimuthalSymmetry(&shockwave, 1., 0., false,false);
            RtoR::FunctionRenderer::renderSection(shockwave2d, line, Color(0.5, .0, .0, 1.0), true, 1024);



            // ********** CONE DE LUZ **********
            glBegin(GL_LINES);
            {
                glVertex2d(1-s, -H);
                glVertex2d(1-s,  H);

                glVertex2d(     s, -H);
                glVertex2d(     s,  H);
            }
            glEnd();
        }

        getDtPhiGraph(section).DrawAxes();
        RtoR::FunctionRenderer::renderSection(dPhi, line, color, true, phi.getN());
        if(showAnalytic)
        {
            RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtShockwave; ddtShockwave.sett(t);
            auto ddtShockwave2d = R2toR::FunctionAzimuthalSymmetry(&ddtShockwave, 0., 0., false,
                    false);

            RtoR::FunctionRenderer::renderSection(ddtShockwave2d, line, Color(.0, .5, .0, 1.0),
                    true, 1024);

            // ********** CONE DE LUZ **********
            glBegin(GL_LINES);
            {
                glVertex2d(1-s, -H);
                glVertex2d(1-s,  H);

                glVertex2d(s, -H);
                glVertex2d(s,  H);
            }
            glEnd();
        }
    }

    // *************************************************************************************************
    // ********** CAMPO INTEIRO (3D) ************************************************************************
    const Real GLOBAL_Z_SCALE_PHI = 0.1;
    const Real GLOBAL_Z_SCALE_DDTPHI = 0.1;
    //glEnable(GL_DEPTH_TEST);
    phiGraph.BindWindow();
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(zpr.getModelview());

    xMin = phi.getDomain().xMin; xMax = phi.getDomain().xMax;
    // yMin = phi.getDomain().yMin; yMax = phi.getDomain().yMax;
    RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, 250, 500, GLOBAL_Z_SCALE_PHI);
    //RtoR::AnalyticShockwave2DRadialSymmetry shockwave; shockwave.sett(t);
    //auto shockwave2d = R2toR::FunctionAzimuthalSymmetry(&shockwave, 1., 0., false,false);
    //RtoR::FunctionRenderer::renderFunction(shockwave2d, xMin, xMin, xMax - xMin, 250, 500, GLOBAL_Z_SCALE_PHI);

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
        glLineWidth(1.5);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xAAAA);
        glColor4d(0.5, 0, 0, 0.8);
        glBegin(GL_LINE_LOOP);
        for (Real theta = 0.0; theta < 2 * M_PI; theta += M_PI / 3000) {
            const Real x = coneRadius * cos(theta), y = coneRadius * sin(theta);
            glVertex3d(x, y, GLOBAL_Z_SCALE_PHI * phi({x, y}));
        }
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }


    if(showDPhidt) {
        dPhiGraph.BindWindow();
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(zpr.getModelview());
        RtoR::FunctionRenderer::renderFunction(dPhi, xMin, xMin, xMax - xMin, 100, 100, GLOBAL_Z_SCALE_DDTPHI);
    }

    // OUTPUT TEXTO DADOS DA SIMULACAO
    {
        std::ostringstream stat;

        addVolatileStat("");
        stat << "L=" << L;
        addVolatileStat(stat.str());

        stat.str("");
        stat << "a=" << a;
        addVolatileStat(stat.str());

        stat.str("");
        stat << "a=" << eps;
        addVolatileStat(stat.str());

    }

    // OUTPUT DA ENERGIA
    if(1)
    {
        std::ostringstream stat;
        #define sq(a) ((a)*(a))
        // Coordenadas cartesianas, numerico

        addVolatileStat("");
        addVolatileStat("Energia numerica:");
        if(1)
        {
            const auto N = phi.getSpace().getDim().getN(0);
            const auto M = phi.getSpace().getDim().getN(1);
            const auto h = phi.getSpace().geth();
            const auto invh_2=.5/h;
            const auto dx=h, dy=h;
            const auto invdx_2=.5/dx, invdy_2=.5/dy;

            Real E = .0;
            // Desconsideramos as bordas do espaco de simulacao porque elas devem ser sempre zero e, quando isso nao
            // acontece, a simulacao passa a ser invalida (se por nenhum outro motivo, por definicao).
            for(auto x=-.5*L; x<.5*L; x+=dx){
                for(auto y=-.5*L; y<.5*L; y+=dy){
                    const Real DtPhi = dPhi({x, y});
                    const Real DxPhi = invdx_2 * (-phi({x-dx, y}) + phi({x+dx, y}));
                    const Real DyPhi = invdy_2 * (-phi({x,y-dy}) + phi({x,y+dy}));
                    const Real V = abs(phi({x,y}));

                    E += .5*(sq(DtPhi) + sq(DxPhi) + sq(DyPhi)) + V;
                }
            }
            E *= dx*dy;

            stat << "Cartesian integration: E=" << E;
            addVolatileStat(stat.str());

            // NumericalIntegration test. We know this should integrate to 1

            auto f = R2toR::R2toRRegularDelta(eps, a);
            auto A1=0.0, A2=0.0;
            for(auto x=-.5*L; x<.5*L; x+=dx){
                for(auto y=-.5*L; y<.5*L; y+=dy){
                    const auto v = f({x,y});
                    A1 += v*v;
                    A2 += v;
                }
            }
            A1 *= .5*dx*dy;
            A2 *= dx*dy;
            stat.str("");
            stat << "Test integration A1=" << A1;
            addVolatileStat(stat.str());
            stat.str("");
            stat << "Test integration A2=" << A2;
            addVolatileStat(stat.str());
            stat.str("");
            stat << "a=" << a;
            addVolatileStat(stat.str());

        }

        // Coordenadas radiais, computado da sol. comp. analitica
        if(0)
        {
            auto t = lastInfo.getT();
            auto shockwave = RtoR::AnalyticShockwave2DRadialSymmetry();
            auto ddtShockwave = RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB();
            shockwave.sett(t); ddtShockwave.sett(t);

            Real E = 0.0;
            const Real dr = 1.e-5;
            for(Real r=0; r<=6; r+=dr){
                const Real DtPhi = ddtShockwave(r);
                const Real DrPhi = (.5/dr) * (-shockwave(r-dr) + shockwave(r));

                const Real V = abs(shockwave(r));

                E += r*(.5 * (sq(DtPhi) + sq(DrPhi)) + V);
            }
            E *= 2*M_PI*dr;
            std::ostringstream stat;
            stat << "E_rad  = " << E;
            addVolatileStat(stat.str());

        }

        addVolatileStat("");
        stat.str("");
        addVolatileStat("Energia teorica:");
        stat << "E0 = " << (2./(9.*eps*eps));
        addVolatileStat(stat.str());
        stat.str("");
        stat << "E_interior  = " << (2./3)*M_PI*t*t;
        addVolatileStat(stat.str());
        stat.str("");
        stat << "E_input  = " << E_input;
        addVolatileStat(stat.str());
        #undef sq
    }
}

void R2toR::OutputOpenGLShockwave::reshape(int width, int height) {
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

void R2toR::OutputOpenGLShockwave::notifyGLUTKeyboardSpecial(int key, int x, int y) {
    const Real angle = 2.5e-3 * M_PI;
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

void R2toR::OutputOpenGLShockwave::notifyGLUTKeyboard(unsigned char key, int x, int y) {
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
    }
    else if(key == 13){
        this->_outputSnapshot();
    }

    std::cout << yMin << "   " << yMax << std::endl;

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

IntPair R2toR::OutputOpenGLShockwave::getWindowSizeHint() {
    return {1600, 1000};
}

void R2toR::OutputOpenGLShockwave::addSection(const RtoR2::StraightLine &section, String name) {
    const Real rangeField = 0.15;
    const Real rangeDdtField = 1.5;

    const Color color = Base::colors[sections.size()];

    Graph fieldGraph = Graph(phiGraph.winX + phiGraph.winW, .0,
            windowWidth - statsWindowWidth, windowHeight, .0, 1., -rangeField, rangeField, name + " \\phi");
    Graph ddtFieldGraph = Graph(phiGraph.winX + phiGraph.winW, .0,
            windowWidth - statsWindowWidth, windowHeight, .0, 1., -rangeDdtField, rangeDdtField, name + " D_t \\phi");
    sections.emplace_back(Base::GraphPair(fieldGraph, ddtFieldGraph), Base::LineAndColor(section, color));

}

void R2toR::OutputOpenGLShockwave::notifyGLUTMouseButton(int button, int dir, int x, int y) {
    zpr.zprMouseButton(button, dir, x, y);
}

void R2toR::OutputOpenGLShockwave::notifyGLUTMouseMotion(int x, int y) {
    zpr.zprMouseMotion(x, y);
}



void R2toR::OutputOpenGLShockwave::_outputSnapshot() {
    const auto *field = this->lastInfo.getFieldData<R2toR::FieldState>();
    const auto &phi = field->getPhi();

    const auto t0 = 0;
    const auto t = lastInfo.getT();
    const auto xMin = phi.getDomain().xMin;
    const auto yMin = phi.getDomain().yMin;
    const auto L = phi.getDomain().getLx();

    std::string sFileName = "./file-t0="+ToString(t0) + "-t=" + ToString(t);
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
        const auto dh = L/N;

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
