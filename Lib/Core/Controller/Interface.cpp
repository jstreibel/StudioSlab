//
// Created by joao on 10/13/21.
//

#include "Interface.h"
#include "InterfaceOwner.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"

#include <iomanip>

#include "Utils/Sets.h"


namespace Slab::Core {

    FInterface::FInterface(const Str& Name, FInterfaceOwner *pOwner, int Priority)
            : pOwner(pOwner), Priority(Priority), Protocols(pOwner->GetProtocols())
    {
        auto tokens = Common::SplitString(Name, Delimiter, 2);
        this->Name = tokens[0];
        this->Description = tokens.size() > 1 ? tokens[1] : this->Description;

        AddListener(pOwner);

        Log::Note() << "Interface '" << Log::FGGreen << Name << Log::ResetFormatting << "' created. " << Log::Flush;
    }

    auto FInterface::GetParameters() const -> Vector<TPointer<const FParameter>> {
        Vector<TPointer<const FParameter>> constParameters;

        std::copy(Parameters.begin(), Parameters.end(), std::back_inserter(constParameters));

        return constParameters;
    }

    auto FInterface::GetParameters() -> Set<TPointer<FParameter>>
    {
        return Parameters;
    }

    auto FInterface::GetSubInterfaces() const -> Vector<TPointer<FInterface>> {
        Vector<TPointer<FInterface>> interfaces;

        std::copy(SubInterfaces.begin(), SubInterfaces.end(), std::back_inserter(interfaces));

        return interfaces;
    }

    void FInterface::AddParameter(const TPointer<FParameter>& parameter) {
        auto insertionSuccessful = Parameters.insert(parameter).second;

        if (!insertionSuccessful) {
            throw Exception("Error while inserting parameter in interface.");
        }

        auto quotename = Str("\"") + parameter->GetFullCommandLineName() + "\"";
        Log::Note() << "Parameter " << std::setw(25) << std::left << quotename << " registered to interface \"" << GetName()
                    << "\".";
    }

    void FInterface::AddParameters(const TList<TPointer<FParameter>>& parametersList) {
        for (const auto& param: parametersList)
            AddParameter(param);
    }

    void FInterface::AddParameters(const TList<FParameter *>& parametersList) {
        for (const auto param: parametersList)
            AddParameter(Naked(*param));
    }


    void FInterface::AddSubInterface(const TPointer<FInterface>& subInterface) {
        if (Contains(SubInterfaces, subInterface))
            throw Exception(Str("Error while inserting sub-interface '") + subInterface->GetName()
                  + Str("' in interface '") + this->GetName() + Str("': interface contains sub interface already"));

        if (!SubInterfaces.insert(subInterface).second) {
            throw Exception(Str("Error while inserting sub-interface '") + subInterface->GetName()
                  + Str("' in interface '") + this->GetName() + Str("': not specified"));
        }
    }

    auto FInterface::GetGeneralDescription() const -> Str {

        return Description != "<empty>" ? Description : "";
    }

    auto FInterface::GetParameter(const Str& key) const -> TPointer<FParameter> {
        auto compareFunc = [key](const TPointer<FParameter>& parameter) {
            return *parameter == key;
        };

        fix result = std::ranges::find_if(Parameters, compareFunc);

        return *result;
    }

    auto FInterface::ToString(const StrVector &ParamNames, const Str& Separator, bool LongName) const -> Str {
        std::stringstream StringStream("");

        std::vector ParamCount(ParamNames.size(), 0);

        fix LONG_NAME = true;
        fix SHORT_NAME = false;

        for (auto &param: Parameters) {
            auto nameShort = param->GetCommandLineArgumentName(SHORT_NAME);
            auto nameLong = param->GetCommandLineArgumentName(LONG_NAME);

            if (Contains(ParamNames, nameShort) || Contains(ParamNames, nameLong) ||
                ParamNames.empty()) {
                bool isLong = !nameLong.empty() && LongName;
                StringStream << param->GetCommandLineArgumentName(isLong) << "=" << param->ValueToString() << Separator;
            }
        }

        auto ReturnString = StringStream.str();
        if (ReturnString.back() == Separator.back() && ReturnString.size() > Separator.size())
            for (int i = 0; i < Separator.size(); i++) ReturnString.pop_back(); // remove trailing separator


        return ReturnString;
    }

    void FInterface::SetupFromCommandLine(CLVariablesMap vm) {
        try {
            for (auto param: Parameters) {
                auto key = param->GetCommandLineArgumentName(true);
                auto val = vm[key];

                param->SetValueFromCommandLine(val);
            }

            for (auto Listener: Listeners)
                Listener->SendMessage(FPayload::CommandLineParsingFinished);

        } catch (cxxopts::exceptions::exception &exception) {
            Log::Error() << "Exception happened in Interface \"" << GetGeneralDescription() << "\"" << Log::Flush;
            throw;
        }
    }

    bool FInterface::operator==(const FInterface &rhs) const {
        return std::tie(Name, Parameters, SubInterfaces) ==
               std::tie(rhs.Name, rhs.Parameters, rhs.SubInterfaces);
    }

    bool FInterface::operator==(Str str) const {
        return Name == str;
    }

    bool FInterface::operator!=(const FInterface &RHS) const {
        return !(RHS == *this);
    }

    FInterface::~FInterface() {

    }

    auto FInterface::AddListener(FCommandLineInterfaceListener *newListener) -> void {
        Listeners.emplace_back(newListener);
    }

    auto FInterface::GetOwner() const -> FInterfaceOwner * {
        return pOwner;
    }

    auto FInterface::GetName() const -> const Str & {
        return Name;
    }

    bool FInterface::operator<(const FInterface &RHS) const {
        return Priority < RHS.Priority;
    }

    void FInterface::SetGeneralDescription(const Str& str)
    {
        Description = str;
    }

    void FInterface::SendRequest(const FPayload& Payload) const
    {
        if(!Contains(Protocols, Payload)) {

        }

        pOwner->SendMessage(Payload);
    }


}
