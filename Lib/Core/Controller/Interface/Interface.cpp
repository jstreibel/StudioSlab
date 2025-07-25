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

    auto FInterface::GetParameters() const -> Vector<TPointer<const FParameter>> {
        Vector<TPointer<const FParameter>> ConstParameters;

        std::copy(Parameters.begin(), Parameters.end(), std::back_inserter(ConstParameters));

        return ConstParameters;
    }

    void FInterface::AddParameter(const TPointer<FParameter>& Parameter) {
        auto insertionSuccessful = Parameters.insert(Parameter).second;

        if (!insertionSuccessful) {
            throw Exception("Error while inserting parameter in interface.");
        }
    }
    void FInterface::AddParameters(std::initializer_list<TPointer<FParameter>> ParametersList) {
        for (const auto& Param: ParametersList)
            AddParameter(Param);
    }

    auto FInterface::GetParameter(const Str& Key) const -> TPointer<FParameter> {
        auto compareFunc = [Key](const TPointer<FParameter>& Parameter) {
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