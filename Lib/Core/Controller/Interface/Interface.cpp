//
// Created by joao on 10/14/24.
//

#include "Interface.h"
#include "InterfaceManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    FInterface::FInterface(const Str& Name)
            : Name(Name), Id(FInterfaceManager::GenerateUniqueID()) {

    }

    FInterface::~FInterface() = default;

    auto FInterface::GetParameters() const -> Vector<FCommandLineParameter_constptr> {
        Vector<FCommandLineParameter_constptr> constParameters;

        std::copy(Parameters.begin(), Parameters.end(), std::back_inserter(constParameters));

        return constParameters;
    }

    void FInterface::AddParameter(FCommandLineParameter_ptr Parameter) {
        auto insertionSuccessful = Parameters.insert(Parameter).second;

        if (!insertionSuccessful) {
            throw Exception("Error while inserting parameter in interface.");
        }

        auto quotename = Str("\"") + Parameter->getFullCommandLineName() + "\"";
    }
    void FInterface::AddParameters(std::initializer_list<FCommandLineParameter_ptr> ParametersList) {
        for (const auto& Param: ParametersList)
            AddParameter(Param);
    }

    auto FInterface::GetParameter(const Str& Key) const -> FCommandLineParameter_ptr {
        auto compareFunc = [Key](const FCommandLineParameter_ptr& Parameter) {
            return *Parameter == Key;
        };

        auto result = std::find_if(Parameters.begin(), Parameters.end(), compareFunc);

        return *result;
    }

    FMessage FInterface::SendRequest(FRequest Request)
    {
        return FMessage{"[unknown request]"};
    }

    UniqueID FInterface::GetUniqueID() const {
        return Id;
    }


} // Slab::Core