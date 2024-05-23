//
// Created by joao on 30/05/2021.
//

#include <Core/Math/Maps/R2toR/Model/FieldState.h>
#include <filesystem>
#include "OutputShockwaveZeros.h"
#include "View/Util/Printing.h"

using namespace Slab::Math::R2toR;

typedef R2toR::FunctionArbitrary Func;

OutputShockwaveZeros::OutputShockwaveZeros(int N, int outputResolution, Real t0) : outputResolution(outputResolution), t0(t0)
{
    std::string sFileName = "./swzeros";

    int count=0;
    do count++; while(std::filesystem::exists(sFileName + (count>1?"-"+std::to_string(count):"") + ".dat"));

    sFileName += (count>1 ? "-"+std::to_string(count) : "");
    sFileName += ".dat";

    file.open(sFileName, std::ios::out);

    if(!file.is_open()) throw "Could not open file to output shockwave zeros.";

    file << "{\'description\': \'Zeroes of shockwave along section y=0 of 2d shockwave.\',\n"
         << " \'variables\': \'t,x\',\n"
         << " \'points\': np.array((\n";



    // TODO: we need the information below for output, not just hard-coded the way it is. But right here, the ModelBuilder is not initialized yet.
    const auto a = 0.1;

    /*
    const auto dx = L/double(N);
    const auto dt = a*dx;
    const auto NTotalSteps = t/dt;

    const auto dxs = L/double(NPoints);
    const auto dts = dxs;
    const auto NOutSteps = t/dts;

    const auto _nStepsBetweenRecordings = NTotalSteps/NOutSteps; // = (t/dt)/(t/dts) = dts/dt = dxs/(a*dx)
                                                                 // = (L/Ndpi)/(a*L/N) = (1/a)*(N/Ndpi)
    */

    letc NPoints = outputResolution;
    const auto _nStepsBetweenRecordings = int((Real(N)/NPoints)/a);
    this->nStepsBetweenRecordings = _nStepsBetweenRecordings > 0 ? _nStepsBetweenRecordings : 1;
}


OutputShockwaveZeros::~OutputShockwaveZeros() {
    file.flush();
    file.close();
}


let OutputShockwaveZeros::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    file << "))}" << std::endl;
    file.flush();

    return true;
}

let OutputShockwaveZeros::description() const -> String {
    return "output of the zeros of the shockwave, together with the zeros of the (expected) numerical shockwave";
}

void OutputShockwaveZeros::_out(const OutputPacket &OutputInfoOStream) {

    let &phi = OutputInfo.getFieldData<R2toR::FieldState>()->getPhi();
    let &dphidt = OutputInfo.getFieldData<R2toR::FieldState>()->getDPhiDt();

    let domain = phi.getDomain();

    let xMin = domain.xMin;
    let L = domain.getLx();
    let NPoints = outputResolution;
    let dx = L/NPoints;
    let dt = dx;

    let t = this->lastInfo.getT();

    for(auto i=0; i<NPoints; ++i){
        auto x = xMin + i*dx;

        if(fabs(x)>t+t0) continue; // do not consider regions outside of the light cone

        //if(x<0) dx = -dx;

        let dfdt = dphidt({x,0});
        //let fx = phi({x-dx, 0});
        //let ft = phi({x,0}) - dfdt*dt;
        let f = phi({x, 0});
        //let dfdx = (f2x-f1x)/dx;

        // ESSE SERIA O IDEAL:
        // df = dfdx*dx + dfdt*dt
        // df equiv f(x,t) - f(x-dfdx*dx, t-dfdt*dt) = f2 - f1
        // f2-f1 =  dfdx*dx + dfdt*dt
        // f1 = f2 - (dfdx*dx + dfdt*dt)
        //
        // COMO NAO TEMOS ISSO, ENTAO USAMOS SEMPRE NA DIAGONAL
        // f1 = f(x-dx. t-dt)

        let f2 = f + dfdt*dt;
        let f1 = f - dfdt*dt;

        // TODO: implement signbit the proper way
        if(std::signbit(f2) != std::signbit(f1))
            file << "(" << x << "," << t << "), ";
    }
}

