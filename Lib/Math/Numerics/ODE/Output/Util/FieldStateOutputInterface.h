//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_FSTATEOUTPUTINTERFACE_H
#define V_SHAPE_FSTATEOUTPUTINTERFACE_H

#include "Utils/Types.h"

#include <ostream>


namespace Slab::Math {

    class EqStateOutputInterface {
    public:
        virtual ~EqStateOutputInterface() = default;

        static enum Formats {
            SpaceSeparated,
            PythonDictionaryEntry
        } format;

        static enum FieldDataOutputTypes {
            Phi,
            PhiAndDPhiDt
        } fDataOutType;

        virtual void outputPhi(OStream &out,
                               Str separator) const { throw "EqStateOutputInterface::outputPhi(...) Not implemented."; }

        virtual void outputdPhiDt(OStream &out,
                                  Str separator) const { throw "EqStateOutputInterface::outputDPhiDt(...) not implemented."; }

        [[nodiscard]] virtual EqStateOutputInterface *Copy(UInt N) const = 0;

        friend OStream &operator<<(OStream &stream, const EqStateOutputInterface &oStreamReady) {
            Str sep = format == SpaceSeparated ? " " : ", ";

            if (format == PythonDictionaryEntry) stream << "\"phi\": (";
            oStreamReady.outputPhi(stream, sep);
            if (fDataOutType) {
                if (format == PythonDictionaryEntry) stream << "), \"dphidt\": (";
                oStreamReady.outputdPhiDt(stream, sep);
            }
            if (format == PythonDictionaryEntry) stream << ")";

            return stream;
        }
    };


}

#endif //V_SHAPE_FSTATEOUTPUTINTERFACE_H
