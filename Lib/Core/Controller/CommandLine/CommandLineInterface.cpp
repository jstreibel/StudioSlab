//
// Created by joao on 10/13/21.
//

#include "CommandLineInterface.h"
#include "CommandLineInterfaceOwner.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"

#include <iomanip>


namespace Slab::Core {

    FCommandLineInterface::FCommandLineInterface(const Str& Name, FCommandLineInterfaceOwner *pOwner, int Priority)
            : pOwner(pOwner), Priority(Priority), Protocols(pOwner->GetProtocols()) {


        auto tokens = Common::SplitString(Name, Delimiter, 2);
        this->Name = tokens[0];
        this->Description = tokens.size() > 1 ? tokens[1] : this->Description;

        AddListener(pOwner);

        Log::Note() << "Interface '" << Log::FGGreen << Name << Log::ResetFormatting << "' created. " << Log::Flush;
    }

    auto FCommandLineInterface::GetParameters() const -> Vector<TPointer<const FCommandLineParameter>> {
        Vector<TPointer<const FCommandLineParameter>> constParameters;

        std::copy(Parameters.begin(), Parameters.end(), std::back_inserter(constParameters));

        return constParameters;
    }

    auto FCommandLineInterface::GetSubInterfaces() const -> Vector<TPointer<FCommandLineInterface>> {
        Vector<TPointer<FCommandLineInterface>> interfaces;

        std::copy(SubInterfaces.begin(), SubInterfaces.end(), std::back_inserter(interfaces));

        return interfaces;
    }

    void FCommandLineInterface::AddParameter(const TPointer<FCommandLineParameter>& parameter) {
        auto insertionSuccessful = Parameters.insert(parameter).second;

        if (!insertionSuccessful) {
            throw Exception("Error while inserting parameter in interface.");
        }

        auto quotename = Str("\"") + parameter->getFullCommandLineName() + "\"";
        Log::Note() << "Parameter " << std::setw(25) << std::left << quotename << " registered to interface \"" << GetName()
                    << "\".";
    }

    void FCommandLineInterface::AddParameters(const TList<TPointer<FCommandLineParameter>>& parametersList) {
        for (const auto& param: parametersList)
            AddParameter(param);
    }

    void FCommandLineInterface::AddParameters(const TList<FCommandLineParameter *>& parametersList) {
        for (const auto param: parametersList)
            AddParameter(Naked(*param));
    }


    void FCommandLineInterface::AddSubInterface(const TPointer<FCommandLineInterface>& subInterface) {
        if (Contains(SubInterfaces, subInterface))
            throw Exception(Str("Error while inserting sub-interface '") + subInterface->GetName()
                  + Str("' in interface '") + this->GetName() + Str("': interface contains sub interface already"));

        if (!SubInterfaces.insert(subInterface).second) {
            throw Exception(Str("Error while inserting sub-interface '") + subInterface->GetName()
                  + Str("' in interface '") + this->GetName() + Str("': not specified"));
        }
    }

    auto FCommandLineInterface::GetGeneralDescription() const -> Str {

        return Description != "<empty>" ? Description : "";
    }

    auto FCommandLineInterface::GetParameter(const Str& key) const -> TPointer<FCommandLineParameter> {
        auto compareFunc = [key](const TPointer<FCommandLineParameter>& parameter) {
            return *parameter == key;
        };

        fix result = std::ranges::find_if(Parameters, compareFunc);

        return *result;
    }

    auto FCommandLineInterface::ToString(const StrVector &ParamNames, const Str& Separator, bool LongName) const -> Str {
        std::stringstream ss("");

        std::map<Str, int> paramCount;
        for (auto &p: ParamNames) paramCount[p] = 0;

        fix LONG_NAME = true;
        fix SHORT_NAME = false;

        for (auto &param: Parameters) {
            auto nameShort = param->getCommandLineArgumentName(SHORT_NAME);
            auto nameLong = param->getCommandLineArgumentName(LONG_NAME);

            if (Contains(ParamNames, nameShort) || Contains(ParamNames, nameLong) ||
                ParamNames.empty()) {
                bool isLong = !nameLong.empty() && LongName;
                ss << param->getCommandLineArgumentName(isLong) << "=" << param->ValueToString() << Separator;

                paramCount[isLong ? nameLong : nameShort]++;
            }
        }

        for (auto &pCount: paramCount)
            if (pCount.second == 0)
                Log::Warning() << __PRETTY_FUNCTION__ << " could not find parameter " << pCount.first << Log::Flush;

        auto str = ss.str();
        if (!Parameters.empty())
            for (int i = 0; i < Separator.size(); i++) str.pop_back(); // remove trailing sparator

        return str;
    }

    void FCommandLineInterface::SetupFromCommandLine(CLVariablesMap vm) {
        try {
            for (auto param: Parameters) {
                auto key = param->getCommandLineArgumentName(true);
                auto val = vm[key];

                param->SetValueFromCommandLine(val);
            }

            for (auto listener: Listeners)
                listener->NotifyCLArgsSetupFinished();

        } catch (cxxopts::exceptions::exception &exception) {
            Log::Error() << "Exception happened in Interface \"" << GetGeneralDescription() << "\"" << Log::Flush;
            throw exception;
        }
    }

    bool FCommandLineInterface::operator==(const FCommandLineInterface &rhs) const {
        return std::tie(Name, Parameters, SubInterfaces) ==
               std::tie(rhs.Name, rhs.Parameters, rhs.SubInterfaces);
    }

    bool FCommandLineInterface::operator==(Str str) const {
        return Name == str;
    }

    bool FCommandLineInterface::operator!=(const FCommandLineInterface &RHS) const {
        return !(RHS == *this);
    }

    FCommandLineInterface::~FCommandLineInterface() {

    }

    auto FCommandLineInterface::AddListener(FCommandLineInterfaceListener *newListener) -> void {
        Listeners.emplace_back(newListener);
    }

    auto FCommandLineInterface::GetOwner() const -> FCommandLineInterfaceOwner * {
        return pOwner;
    }

    auto FCommandLineInterface::GetName() const -> const Str & {
        return Name;
    }

    bool FCommandLineInterface::operator<(const FCommandLineInterface &RHS) const {
        return Priority < RHS.Priority;
    }

    FMessage FCommandLineInterface::SendRequest(FRequest Request) const
    {
        if(!Contains(Protocols, Request)) {
            return FMessage{"[unknown request]"};
        }

        return pOwner->requestIssued(Request);
    }


}
