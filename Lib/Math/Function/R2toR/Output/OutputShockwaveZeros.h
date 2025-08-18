//
// Created by joao on 30/05/2021.
//

#ifndef V_SHAPE_OUTPUTSHOCKWAVEZEROS_H
#define V_SHAPE_OUTPUTSHOCKWAVEZEROS_H


#include "Math/Numerics/OutputChannel.h"
#include <fstream>


namespace Slab::Math::R2toR {

    class OutputShockwaveZeros /*: public OutputChannel */{
    private:
        std::fstream file;
        int outputResolution;
        DevFloat t0;

    public:
        OutputShockwaveZeros(int N, int outputResolution, DevFloat t0=0);
        // ~OutputShockwaveZeros() override;

        // auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;


    protected:
        // void _out(const OutputPacket &OutputInfo"") override;
    };

}


#endif //V_SHAPE_OUTPUTSHOCKWAVEZEROS_H
