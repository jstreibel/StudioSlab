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
static const std::unordered_map<std::type_index, Handler> GTable = {
    {
        typeid(Slab::Core::RealParameter), [](Slab::Core::FParameter& Parameter)
        {
            auto& RealParameter = static_cast<Slab::Core::RealParameter&>(Parameter);
            float Value = RealParameter.GetValue();
            float Speed = Value*1e-2f;

            if (ImGui::DragFloat(Parameter.GetName().c_str(), &Value, Speed < 1.e-5f ? 1.e-5f : Speed))
                RealParameter.SetValue(Value);

            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text("%s", Parameter.GetDescription().c_str());
                ImGui::EndTooltip();
            }
        }
    },
    {
        typeid(Slab::Core::IntegerParameter), [](Slab::Core::FParameter& Parameter)
        {
            auto& IntegerParameter = static_cast<Slab::Core::IntegerParameter&>(Parameter);
            int Value = IntegerParameter.GetValue();
            float Speed = Value*1e-2f;
            if (ImGui::DragInt(Parameter.GetName().c_str(), &Value, Speed < 1.e-1f ? 1.e-1f : Speed))
                IntegerParameter.SetValue(Value);

            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text("%s", Parameter.GetDescription().c_str());
                ImGui::EndTooltip();
            }
        }
    },
    {
        typeid(Slab::Core::StringParameter), [](Slab::Core::FParameter& Parameter)
        {
            auto& StringParameter = static_cast<Slab::Core::StringParameter&>(Parameter);
            auto &Value = StringParameter.GetValue();
            ImGui::InputText(Parameter.GetName().c_str(), Value.data(), Value.size());

            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text("%s", Parameter.GetDescription().c_str());
                ImGui::EndTooltip();
            }
        }
    },
    {
        typeid(Slab::Core::BoolParameter), [](Slab::Core::FParameter& Parameter)
        {
            auto& BoolParameter = static_cast<Slab::Core::BoolParameter&>(Parameter);
            ImGui::Checkbox((Parameter.GetName()).c_str(), &BoolParameter.GetValue());

            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text("%s", Parameter.GetDescription().c_str());
                ImGui::EndTooltip();
            }
        }
    },
};

void Handle(Slab::Core::FParameter& Parameter) {
    if (auto ParamEntry = GTable.find(typeid(Parameter)); ParamEntry != GTable.end())
    {
        ParamEntry->second(Parameter);
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), (Slab::Str("Unknown parameter type")).c_str());
    }
}

void ParameterGUIRenderer::RenderParameter(const Slab::TPointer<Slab::Core::FParameter>& Parameter)
{
    Handle(*Parameter);
}
