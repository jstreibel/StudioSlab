//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_FSTATEOUTPUTINTERFACE_H
#define V_SHAPE_FSTATEOUTPUTINTERFACE_H

#include <Lib/Util/MathTypes.h>

class FStateOutputInterface {
public:
    static enum Formats {
        SpaceSeparated,
        PythonDictionaryEntry
    } format;

    static enum FieldDataOutputTypes {
        Phi,
        PhiAndDPhiDt
    } fDataOutType;

    virtual void outputPhi(std::ostream &out, String separator) const { throw "FStateOutputInterface::outputPhi(...) Not implemented."; }
    virtual void outputdPhiDt(std::ostream &out, String separator) const { throw "FStateOutputInterface::outputDPhiDt(...) not implemented."; }
    virtual FStateOutputInterface* Copy(PosInt N) const = 0;

    friend std::ostream& operator<< (std::ostream& stream, const FStateOutputInterface& oStreamReady) {
        String sep = format == SpaceSeparated ? " " : ", ";

        if(format == PythonDictionaryEntry) stream << "\"phi\": (";
        oStreamReady.outputPhi(stream, sep);
        if(fDataOutType) {
            if (format == PythonDictionaryEntry) stream << "), \"dphidt\": (";
            oStreamReady.outputdPhiDt(stream, sep);
        }
        if(format == PythonDictionaryEntry) stream << ")";

        return stream;
    }
};

#endif //V_SHAPE_FSTATEOUTPUTINTERFACE_H
