#ifndef OUTPUTINFO_H
#define OUTPUTINFO_H

#include "FStateOutputInterface.h"
#include "Lib/Fields/Model/DiscreteSpace/DiscreteSpace.h"


typedef std::unique_ptr<FStateOutputInterface*> FieldDataPtr;

class OutputPacket{
public:
    OutputPacket() : spaceData(DiscreteSpacePair(nullptr, nullptr)), currStep(0), t(0) {
    }

    OutputPacket(const OutputPacket &o) : OutputPacket(o.fieldData, o.spaceData, o.currStep, o.t) {}
    OutputPacket(const void* fieldData, const DiscreteSpacePair space, size_t currStep, floatt t)
    : fieldData(fieldData), spaceData(space), currStep(currStep), t(t) {}

    /*!
     * Make a copy of this class. Note that the FStateOutputInterface is _not_ copied (it is not owned by this class
     * anyway).
     * @param oi The OutputInfo to copy.
     * @return LHS of equality.
     */
    OutputPacket &operator=(const OutputPacket &oi){
        fieldData = oi.fieldData;
        spaceData = oi.spaceData;
        currStep = oi.currStep;
        t = oi.t;

        return *this;
    }

    bool hasValidData() { return spaceData.first != nullptr && spaceData.second != nullptr && fieldData != nullptr; }

    template<class FieldStateType>
    [[nodiscard]] const FieldStateType* getFieldData() const {return (FieldStateType*)fieldData; }
    /*!
     * Get the actual field state data.
     * @return The field state data, of whichever model is being integrated. Note that this pointer should not be
     * disposed of since it is properly owned somewhere upstream.
     */
    [[nodiscard]] inline DiscreteSpacePair getSpaceData() const {return spaceData;}
    [[nodiscard]] inline size_t getSteps() const {return currStep;}
    [[nodiscard]] inline double getT() const {return t;}

private:
    const void *fieldData;
    /*!
     * This is the field space data. It is not owned by this class, and nobody down the line should worry about this.
     */
    DiscreteSpacePair spaceData;
    size_t currStep;
    double t;
};

#endif // OUTPUTINFO_H
