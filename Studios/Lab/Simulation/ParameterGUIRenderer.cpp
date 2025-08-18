//
// Created by joao on 8/17/25.
//

#include "ParameterGUIRenderer.h"

#include "imgui.h"
#include "Core/Controller/CommonParameters.h"

#include <unordered_map>
#include <typeindex>
#include <functional>

using Handler = std::function<void(Slab::Core::FParameter&)>;
static const std::unordered_map<std::type_index, Handler> table = {
    { typeid(Slab::Core::RealParameter),    [](Slab::Core::FParameter& Parameter){ auto& RealParameter    = static_cast<Slab::Core::RealParameter&>(Parameter);    ImGui::TextColored(ImVec4(0, 1, 0, 1), (Slab::Str("%.3f")).c_str(),  RealParameter.GetValue()); } },
    { typeid(Slab::Core::IntegerParameter), [](Slab::Core::FParameter& Parameter){ auto& IntegerParameter = static_cast<Slab::Core::IntegerParameter&>(Parameter); ImGui::TextColored(ImVec4(0, 1, 0, 1), (Slab::Str("%i")).c_str(), IntegerParameter.GetValue()); } },
    { typeid(Slab::Core::StringParameter),  [](Slab::Core::FParameter& Parameter){ auto& StringParameter  = static_cast<Slab::Core::StringParameter&>(Parameter);  ImGui::TextColored(ImVec4(0, 1, 0, 1), (Slab::Str("\"%s\"")).c_str(),  StringParameter.GetValue().c_str()); } },
    { typeid(Slab::Core::BoolParameter),    [](Slab::Core::FParameter& Parameter){ auto& BoolParameter    = static_cast<Slab::Core::BoolParameter&>(Parameter);    ImGui::TextColored(ImVec4(0, 1, 0, 1), (Slab::Str("%s")).c_str(),  BoolParameter.GetValue() ? "True" : "False"); } },
};

void Handle(Slab::Core::FParameter& Parameter) {
    if (auto ParamEntry = table.find(typeid(Parameter)); ParamEntry != table.end()) { ParamEntry->second(Parameter); }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), (Slab::Str("Unknown parameter type")).c_str());
    }
}

void ParameterGUIRenderer::RenderParameter(const Slab::TPointer<Slab::Core::FParameter>& Parameter)
{
    auto ParamName = Parameter->GetName();
    auto ParamDescr = Parameter->GetDescription();

    ImGui::Text(". %s: ", ParamName.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("%s", ParamDescr.c_str());

    Handle(*Parameter);
}
