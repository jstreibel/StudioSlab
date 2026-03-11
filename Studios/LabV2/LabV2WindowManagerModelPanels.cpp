#include "LabV2WindowManager.h"

#include "Core/Model/V2/ModelSeedsV2.h"
#include "Graphics/Typesetting/ImGuiTypesetting.h"
#include "imgui.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>

namespace {

    namespace ModelV2 = Slab::Core::Model::V2;
    namespace Typesetting = Slab::Graphics::Typesetting;

    auto SetTextBufferFromString(const Slab::Str &value, char *buffer, const std::size_t size) -> void {
        if (buffer == nullptr || size == 0) return;
        std::snprintf(buffer, size, "%s", value.c_str());
    }

    auto DrawStringList(const char *label, const Slab::Vector<Slab::Str> &values) -> void {
        if (values.empty()) return;
        ImGui::TextDisabled("%s", label);
        for (const auto &value : values) {
            ImGui::BulletText("%s", value.c_str());
        }
    }

    auto DrawKeyValueMap(const char *label, const std::map<Slab::Str, Slab::Str> &values) -> void {
        if (values.empty()) return;
        ImGui::TextDisabled("%s", label);
        for (const auto &[key, value] : values) {
            ImGui::BulletText("%s = %s", key.c_str(), value.c_str());
        }
    }

    auto SeedScratchTextForModel(const Slab::Core::Model::V2::FModelV2 &model) -> Slab::Str {
        if (model.ModelId == "model.klein_gordon") {
            return "\\Box \\phi + m^2 \\phi = 0";
        }
        return "\\ddot x + \\omega^2 x = 0";
    }

    auto MakeTextStyle(const float fontPixels) -> Typesetting::FTypesetStyle {
        Typesetting::FTypesetStyle style;
        style.FontPixelSize = fontPixels;
        return style;
    }

    auto MakeMathRequest(const Slab::Str &latex, const float fontPixels) -> Typesetting::FTypesetRequest {
        return Typesetting::MakeMathRequest(latex, MakeTextStyle(fontPixels));
    }

    auto MakeTextRequest(const Slab::Str &text, const float fontPixels) -> Typesetting::FTypesetRequest {
        return Typesetting::MakeTextRequest(text, MakeTextStyle(fontPixels));
    }

    auto MakeDefinitionSubtitle(const ModelV2::FDefinitionV2 &definition) -> Slab::Str {
        Slab::Str subtitle = definition.DisplayName.empty() ? definition.DefinitionId : definition.DisplayName;
        subtitle += " | ";
        subtitle += ToString(definition.Kind);
        return subtitle;
    }

    auto MakeRelationSubtitle(const ModelV2::FRelationV2 &relation) -> Slab::Str {
        if (!relation.Name.empty()) return relation.Name + " | " + ToString(relation.Kind);
        return relation.RelationId + " | " + ToString(relation.Kind);
    }

    struct FModelInspectorTheme {
        float BaseFontSize = 0.0f;
        Typesetting::FImGuiTypesetDrawOptions SectionTextOptions;
        Typesetting::FImGuiTypesetDrawOptions SecondaryTextOptions;
        Typesetting::FImGuiTypesetDrawOptions DetailMathOptions;
    };

    auto MakeSelectableRowOptions(const Slab::Str &secondaryText,
                                  const ImVec4 &accentColor,
                                  const bool bSelected) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        Typesetting::FImGuiSelectableTypesetRowOptions rowOptions;
        rowOptions.bSelected = bSelected;
        rowOptions.SecondaryText = secondaryText;
        rowOptions.PrimaryTint = ImGui::GetColorU32(ImGuiCol_Text);
        rowOptions.SecondaryTint = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        rowOptions.HoveredBackgroundTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.12f));
        rowOptions.SelectedBackgroundTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.20f));
        rowOptions.HoveredBorderTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.70f));
        rowOptions.SelectedBorderTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.95f));
        rowOptions.HoveredAccentTint =
            ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.78f));
        rowOptions.SelectedAccentTint = ImGui::GetColorU32(accentColor);
        return rowOptions;
    }

    auto MakeDefinitionRowOptions(const ModelV2::FDefinitionV2 &definition,
                                  const bool bSelected) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeSelectableRowOptions(
            MakeDefinitionSubtitle(definition),
            ImVec4(0.29f, 0.60f, 0.45f, 1.0f),
            bSelected);
    }

    auto MakeRelationRowOptions(const ModelV2::FRelationV2 &relation,
                                const bool bSelected) -> Typesetting::FImGuiSelectableTypesetRowOptions {
        return MakeSelectableRowOptions(
            MakeRelationSubtitle(relation),
            ImVec4(0.31f, 0.49f, 0.74f, 1.0f),
            bSelected);
    }

    auto DrawDefinitionDetails(Typesetting::FTypesettingService &typesettingService,
                               const ModelV2::FModelV2 &model,
                               const ModelV2::FDefinitionV2 &definition,
                               const FModelInspectorTheme &theme) -> void {
        const auto rendered = RenderDialectDefinitionV2(definition, &model);
        const auto title = definition.DisplayName.empty() ? definition.DefinitionId : definition.DisplayName;

        Typesetting::ImGuiTypesetText(
            typesettingService,
            title,
            theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", definition.DefinitionId.c_str(), ToString(definition.Kind));
        Typesetting::ImGuiTypesetMath(typesettingService, rendered, theme.DetailMathOptions);

        if (!definition.Description.empty()) {
            ImGui::TextWrapped("%s", definition.Description.c_str());
        }
        if (definition.DeclaredType.has_value()) {
            const auto typeLabel = RenderDialectTypeExprV2(*definition.DeclaredType, &model);
            Typesetting::ImGuiTypesetLabelValue(
                typesettingService,
                "Type",
                MakeMathRequest(typeLabel, theme.BaseFontSize),
                theme.SecondaryTextOptions,
                theme.DetailMathOptions);
        }
        if (definition.Kind == ModelV2::EDefinitionKindV2::Coordinate) {
            ImGui::BulletText("Coordinate Role: %s", ToString(definition.CoordinateRole));
        }
        if (definition.Kind == ModelV2::EDefinitionKindV2::OperatorSymbol) {
            ImGui::BulletText("Application Style: %s", ToString(definition.OperatorStyle));
        }
        DrawStringList("Arguments", definition.ArgumentDefinitionIds);
        DrawStringList("Dependencies", definition.DependencyDefinitionIds);
        DrawStringList("Tags", definition.Tags);
        DrawKeyValueMap("Metadata", definition.Metadata);
        if (!definition.SourceText.empty()) {
            ImGui::TextDisabled("Source");
            Typesetting::ImGuiTypesetMath(typesettingService, definition.SourceText, theme.DetailMathOptions);
        }
    }

    auto DrawRelationDetails(Typesetting::FTypesettingService &typesettingService,
                             const ModelV2::FModelV2 &model,
                             const ModelV2::FRelationV2 &relation,
                             const FModelInspectorTheme &theme) -> void {
        const auto rendered = RenderDialectRelationV2(relation, &model);
        const auto title = relation.Name.empty() ? relation.RelationId : relation.Name;

        Typesetting::ImGuiTypesetText(
            typesettingService,
            title,
            theme.SectionTextOptions);
        ImGui::TextDisabled("%s | %s", relation.RelationId.c_str(), ToString(relation.Kind));
        Typesetting::ImGuiTypesetMath(typesettingService, rendered, theme.DetailMathOptions);

        if (!relation.Description.empty()) {
            ImGui::TextWrapped("%s", relation.Description.c_str());
        }
        Typesetting::ImGuiTypesetLabelValue(
            typesettingService,
            "Left",
            MakeMathRequest(RenderDialectExpressionV2(relation.Left, &model), theme.BaseFontSize),
            theme.SecondaryTextOptions,
            theme.DetailMathOptions);
        Typesetting::ImGuiTypesetLabelValue(
            typesettingService,
            "Right",
            MakeMathRequest(RenderDialectExpressionV2(relation.Right, &model), theme.BaseFontSize),
            theme.SecondaryTextOptions,
            theme.DetailMathOptions);
        DrawStringList("Tags", relation.Tags);
        DrawKeyValueMap("Metadata", relation.Metadata);
        if (!relation.SourceText.empty()) {
            ImGui::TextDisabled("Source");
            Typesetting::ImGuiTypesetMath(typesettingService, relation.SourceText, theme.DetailMathOptions);
        }
    }

} // namespace

auto FLabV2WindowManager::DrawModelInspectorPanel() -> void {
    using namespace Slab::Core::Model::V2;

    if (ModelDemoCatalog.empty()) {
        ModelDemoCatalog = BuildDemoModelsV2();
    }
    if (ModelDemoCatalog.empty()) {
        ImGui::TextDisabled("No demo models available.");
        return;
    }

    if (SelectedModelIndex < 0 || SelectedModelIndex >= static_cast<int>(ModelDemoCatalog.size())) {
        SelectedModelIndex = 0;
    }

    bool bModelChanged = false;
    if (ImGui::BeginCombo("Seed Model", ModelDemoCatalog[SelectedModelIndex].Name.c_str())) {
        for (int i = 0; i < static_cast<int>(ModelDemoCatalog.size()); ++i) {
            const bool bSelected = i == SelectedModelIndex;
            if (ImGui::Selectable(ModelDemoCatalog[i].Name.c_str(), bSelected)) {
                SelectedModelIndex = i;
                bModelChanged = true;
            }
            if (bSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    auto &model = ModelDemoCatalog[SelectedModelIndex];
    if (bModelChanged) {
        SelectedModelDefinitionId.clear();
        SelectedModelRelationId.clear();
        bSelectedModelDetailIsRelation = false;
        ModelScratchInput = SeedScratchTextForModel(model);
        ModelScratchMode = 1;
    }

    if (SelectedModelDefinitionId.empty() && !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (SelectedModelRelationId.empty() && !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }
    if (!SelectedModelDefinitionId.empty() && FindDefinitionByIdV2(model, SelectedModelDefinitionId) == nullptr && !model.Definitions.empty()) {
        SelectedModelDefinitionId = model.Definitions.front().DefinitionId;
    }
    if (!SelectedModelRelationId.empty() && FindRelationByIdV2(model, SelectedModelRelationId) == nullptr && !model.Relations.empty()) {
        SelectedModelRelationId = model.Relations.front().RelationId;
    }

    const auto validation = ValidateModelV2(model);
    const auto modelLabel = model.Name + " (" + model.ModelId + ")";
    auto &typesettingService = *UiTypesettingService;
    const auto baseFontSize = ImGui::GetFontSize();
    FModelInspectorTheme inspectorTheme;
    inspectorTheme.BaseFontSize = baseFontSize;
    inspectorTheme.SectionTextOptions.Style.FontPixelSize = baseFontSize * 1.05f;
    inspectorTheme.SectionTextOptions.Tint = ImGui::GetColorU32(ImGuiCol_Text);
    inspectorTheme.SecondaryTextOptions.Style.FontPixelSize = baseFontSize * 0.92f;
    inspectorTheme.SecondaryTextOptions.Tint = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    inspectorTheme.DetailMathOptions.Style.FontPixelSize = baseFontSize * 1.12f;
    inspectorTheme.DetailMathOptions.Tint = ImGui::GetColorU32(ImGuiCol_Text);

    ImGui::SeparatorText(modelLabel.c_str());
    ImGui::TextWrapped("%s", model.Description.c_str());
    ImGui::TextDisabled(
        "Definitions: %zu | Relations: %zu | Validation errors: %zu",
        model.Definitions.size(),
        model.Relations.size(),
        validation.ErrorCount());
    if (!model.Tags.empty()) {
        DrawStringList("Tags", model.Tags);
    }

    if (!validation.IsOk()) {
        ImGui::SeparatorText("Validation");
        for (const auto &message : validation.Messages) {
            const auto color = message.Severity == EValidationSeverityV2::Error
                ? ImVec4(0.82f, 0.26f, 0.26f, 1.0f)
                : ImVec4(0.87f, 0.67f, 0.22f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped(
                "[%s] %s (%s): %s",
                ToString(message.Severity),
                message.EntityId.c_str(),
                message.Context.c_str(),
                message.Message.c_str());
            ImGui::PopStyleColor();
        }
    }

    const auto context = FNotationContextV2::FromModel(model);
    const auto drawScratchpad = [&]() {
        ImGui::SeparatorText("Notation Scratchpad");
        {
            const char *modeLabels[] = {"Definition", "Relation"};
            ImGui::Combo("Scratch Mode", &ModelScratchMode, modeLabels, IM_ARRAYSIZE(modeLabels));
            ImGui::SameLine();
            if (ImGui::Button("Seed")) {
                if (ModelScratchMode == 0) {
                    ModelScratchInput = model.ModelId == "model.klein_gordon"
                        ? "\\phi : \\mathbb{R}^{d+1} \\to \\mathbb{R}"
                        : "x : \\mathbb{R} \\to \\mathbb{R}";
                } else {
                    ModelScratchInput = SeedScratchTextForModel(model);
                }
            }
        }

        char buffer[4096];
        SetTextBufferFromString(ModelScratchInput, buffer, sizeof(buffer));
        if (ImGui::InputTextMultiline(
                "##ModelScratchInput",
                buffer,
                sizeof(buffer),
                ImVec2(-FLT_MIN, 120.0f),
                ImGuiInputTextFlags_AllowTabInput)) {
            ModelScratchInput = buffer;
        }

        if (ModelScratchMode == 0) {
            const auto parsed = ParseDefinitionNotationV2(ModelScratchInput, &context);
            if (!parsed.IsOk()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.82f, 0.26f, 0.26f, 1.0f));
                ImGui::TextWrapped("Parse error at %zu: %s", parsed.Error.Position, parsed.Error.Message.c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.22f, 0.66f, 0.32f, 1.0f));
                ImGui::TextUnformatted("Parsed definition successfully.");
                ImGui::PopStyleColor();
                Typesetting::ImGuiTypesetMath(
                    typesettingService,
                    RenderDialectDefinitionV2(MakeDefinitionFromParsedNotationV2(
                        "scratch.definition",
                        parsed.Value->bMembership ? EDefinitionKindV2::Coordinate : EDefinitionKindV2::StateVariable,
                        *parsed.Value,
                        parsed.Value->CanonicalSymbol,
                        {}),
                        &model),
                    inspectorTheme.DetailMathOptions);
                ImGui::BulletText("Symbol: %s", parsed.Value->CanonicalSymbol.c_str());
                ImGui::BulletText("Kind: %s", parsed.Value->bMembership ? "Membership" : "Signature");
                if (parsed.Value->DeclaredType.has_value()) {
                    Typesetting::ImGuiTypesetLabelValue(
                        typesettingService,
                        "Type",
                        MakeMathRequest(RenderDialectTypeExprV2(*parsed.Value->DeclaredType, &model), baseFontSize),
                        inspectorTheme.SecondaryTextOptions,
                        inspectorTheme.DetailMathOptions);
                }
                if (parsed.Value->MembershipSpace.has_value()) {
                    Typesetting::ImGuiTypesetLabelValue(
                        typesettingService,
                        "Space",
                        MakeMathRequest(RenderDialectSpaceExprV2(*parsed.Value->MembershipSpace, &model), baseFontSize),
                        inspectorTheme.SecondaryTextOptions,
                        inspectorTheme.DetailMathOptions);
                }
            }
        } else {
            const auto parsed = ParseRelationNotationV2(
                "scratch.relation",
                ERelationKindV2::Equation,
                ModelScratchInput,
                &context);
            if (!parsed.IsOk()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.82f, 0.26f, 0.26f, 1.0f));
                ImGui::TextWrapped("Parse error at %zu: %s", parsed.Error.Position, parsed.Error.Message.c_str());
                ImGui::PopStyleColor();
            } else {
                auto scratchModel = model;
                scratchModel.Relations = {*parsed.Value};
                const auto scratchValidation = ValidateModelV2(scratchModel);

                if (scratchValidation.IsOk()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.22f, 0.66f, 0.32f, 1.0f));
                    ImGui::TextUnformatted("Parsed relation successfully.");
                    ImGui::PopStyleColor();
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.87f, 0.67f, 0.22f, 1.0f));
                    ImGui::Text("Parsed relation with %zu validation issue(s).", scratchValidation.Messages.size());
                    ImGui::PopStyleColor();
                }

                const auto rendered = RenderDialectRelationV2(*parsed.Value, &model);
                Typesetting::ImGuiTypesetMath(typesettingService, rendered, inspectorTheme.DetailMathOptions);
                Typesetting::ImGuiTypesetLabelValue(
                    typesettingService,
                    "Left",
                    MakeMathRequest(RenderDialectExpressionV2(parsed.Value->Left, &model), baseFontSize),
                    inspectorTheme.SecondaryTextOptions,
                    inspectorTheme.DetailMathOptions);
                Typesetting::ImGuiTypesetLabelValue(
                    typesettingService,
                    "Right",
                    MakeMathRequest(RenderDialectExpressionV2(parsed.Value->Right, &model), baseFontSize),
                    inspectorTheme.SecondaryTextOptions,
                    inspectorTheme.DetailMathOptions);
                for (const auto &message : scratchValidation.Messages) {
                    const auto color = message.Severity == EValidationSeverityV2::Error
                        ? ImVec4(0.82f, 0.26f, 0.26f, 1.0f)
                        : ImVec4(0.87f, 0.67f, 0.22f, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::BulletText("[%s] %s", ToString(message.Severity), message.Message.c_str());
                    ImGui::PopStyleColor();
                }
            }
        }
    };

    const FDefinitionV2 *hoveredDefinition = nullptr;
    const FRelationV2 *hoveredRelation = nullptr;
    const float layoutWidth = ImGui::GetContentRegionAvail().x;
    const float layoutHeight = std::max(ImGui::GetContentRegionAvail().y, 0.0f);
    const float listColumnWidth = std::clamp(layoutWidth * 0.32f, 260.0f, 380.0f);
    const float bottomPaneSpacing = ImGui::GetStyle().ItemSpacing.y;
    float scratchpadHeight = std::clamp(layoutHeight * 0.33f, 120.0f, 240.0f);
    if (layoutHeight > 0.0f) scratchpadHeight = std::min(scratchpadHeight, layoutHeight * 0.45f);
    const float topPaneHeight = std::max(0.0f, layoutHeight - scratchpadHeight - bottomPaneSpacing);

    ImGui::TextDisabled("Hover a row to preview it in the inspector. Click a row to pin selection.");

    if (topPaneHeight > 0.0f) {
        ImGui::BeginChild("ModelTopPane", ImVec2(0.0f, topPaneHeight), false, ImGuiWindowFlags_NoScrollbar);

        if (ImGui::BeginTable(
                "ModelCatalogLayout",
                2,
                ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Catalog", ImGuiTableColumnFlags_WidthFixed, listColumnWidth);
            ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextColumn();

            const float splitterThickness = 6.0f;
            const float catalogRailHeight = std::max(ImGui::GetContentRegionAvail().y, 0.0f);
            const float minCatalogPaneHeight = std::max(
                48.0f,
                std::min(
                    std::max(baseFontSize * 4.0f, 96.0f),
                    std::max(48.0f, (catalogRailHeight - splitterThickness) * 0.4f)));
            const float maxDefinitionsHeight =
                std::max(minCatalogPaneHeight, catalogRailHeight - minCatalogPaneHeight - splitterThickness);
            float definitionsPanelHeight =
                std::clamp(ModelCatalogDefinitionsHeight, minCatalogPaneHeight, maxDefinitionsHeight);
            float relationsPanelHeight =
                std::max(0.0f, catalogRailHeight - definitionsPanelHeight - splitterThickness);

            ImGui::BeginChild("ModelCatalogRail", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginChild("ModelDefinitionsList", ImVec2(0.0f, definitionsPanelHeight), true);
            {
                ImGui::SeparatorText("Definitions");
                for (const auto &definition : model.Definitions) {
                    const bool bSelected =
                        !bSelectedModelDetailIsRelation &&
                        definition.DefinitionId == SelectedModelDefinitionId;
                    auto rowOptions = MakeDefinitionRowOptions(definition, bSelected);

                    ImGui::PushID(definition.DefinitionId.c_str());
                    const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                        typesettingService,
                        "##ModelDefinitionRow",
                        MakeMathRequest(RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f),
                        rowOptions);
                    ImGui::PopID();

                    if (rowResult.bHovered) hoveredDefinition = &definition;
                    if (rowResult.bPressed) {
                        SelectedModelDefinitionId = definition.DefinitionId;
                        bSelectedModelDetailIsRelation = false;
                    }
                }
            }
            ImGui::EndChild();

            ImGui::InvisibleButton("##ModelCatalogSplitter", ImVec2(-FLT_MIN, splitterThickness));
            const bool bSplitterHovered = ImGui::IsItemHovered();
            const bool bSplitterActive = ImGui::IsItemActive();
            if (bSplitterHovered || bSplitterActive) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            }
            if (bSplitterActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
                definitionsPanelHeight = std::clamp(
                    definitionsPanelHeight + ImGui::GetIO().MouseDelta.y,
                    minCatalogPaneHeight,
                    maxDefinitionsHeight);
                ModelCatalogDefinitionsHeight = definitionsPanelHeight;
                relationsPanelHeight =
                    std::max(0.0f, catalogRailHeight - definitionsPanelHeight - splitterThickness);
            } else {
                ModelCatalogDefinitionsHeight = definitionsPanelHeight;
            }

            const auto splitterRectMin = ImGui::GetItemRectMin();
            const auto splitterRectMax = ImGui::GetItemRectMax();
            const float splitterCenterY = 0.5f * (splitterRectMin.y + splitterRectMax.y);
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(splitterRectMin.x + 2.0f, splitterCenterY),
                ImVec2(splitterRectMax.x - 2.0f, splitterCenterY),
                bSplitterActive ? IM_COL32(246, 202, 116, 210) : IM_COL32(96, 108, 124, 192),
                bSplitterActive ? 2.0f : 1.0f);

            ImGui::BeginChild("ModelRelationsList", ImVec2(0.0f, 0.0f), true);
            {
                ImGui::SeparatorText("Relations");
                for (const auto &relation : model.Relations) {
                    const bool bSelected =
                        bSelectedModelDetailIsRelation &&
                        relation.RelationId == SelectedModelRelationId;
                    auto rowOptions = MakeRelationRowOptions(relation, bSelected);

                    ImGui::PushID(relation.RelationId.c_str());
                    const auto rowResult = Typesetting::ImGuiTypesetSelectableRow(
                        typesettingService,
                        "##ModelRelationRow",
                        MakeMathRequest(RenderDialectRelationV2(relation, &model), baseFontSize * 1.04f),
                        rowOptions);
                    ImGui::PopID();

                    if (rowResult.bHovered) hoveredRelation = &relation;
                    if (rowResult.bPressed) {
                        SelectedModelRelationId = relation.RelationId;
                        bSelectedModelDetailIsRelation = true;
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndChild();

            const auto *selectedDefinition = FindDefinitionByIdV2(model, SelectedModelDefinitionId);
            const auto *selectedRelation = FindRelationByIdV2(model, SelectedModelRelationId);
            const bool bFocusFromHover = hoveredDefinition != nullptr || hoveredRelation != nullptr;
            const bool bFocusIsRelation = hoveredRelation != nullptr || (hoveredDefinition == nullptr && bSelectedModelDetailIsRelation);
            const auto *focusedDefinition = hoveredDefinition != nullptr ? hoveredDefinition : selectedDefinition;
            const auto *focusedRelation = hoveredRelation != nullptr ? hoveredRelation : selectedRelation;
            const auto focusedId = bFocusIsRelation
                ? (focusedRelation != nullptr ? focusedRelation->RelationId : Slab::Str())
                : (focusedDefinition != nullptr ? focusedDefinition->DefinitionId : Slab::Str());
            const auto pinnedId = bSelectedModelDetailIsRelation ? SelectedModelRelationId : SelectedModelDefinitionId;

            ImGui::TableNextColumn();
            ImGui::BeginChild("ModelFocusedDetailsPane", ImVec2(0.0f, 0.0f), true);

            ImGui::SeparatorText("Focused Details");
            if (!focusedId.empty()) {
                ImGui::TextDisabled(
                    "Focused: %s | Source: %s",
                    focusedId.c_str(),
                    bFocusFromHover ? "hover preview" : "pinned selection");
                if (bFocusFromHover && !pinnedId.empty()) {
                    ImGui::TextDisabled(
                        "Pinned %s: %s",
                        bSelectedModelDetailIsRelation ? "relation" : "definition",
                        pinnedId.c_str());
                }
            } else {
                ImGui::TextDisabled("Hover a row or pin a selection to inspect it.");
            }

            if (bFocusIsRelation) {
                if (focusedRelation != nullptr) {
                    DrawRelationDetails(typesettingService, model, *focusedRelation, inspectorTheme);
                } else {
                    ImGui::TextDisabled("No relation selected.");
                }
            } else {
                if (focusedDefinition != nullptr) {
                    DrawDefinitionDetails(typesettingService, model, *focusedDefinition, inspectorTheme);
                } else {
                    ImGui::TextDisabled("No definition selected.");
                }
            }

            ImGui::EndChild();
            ImGui::EndTable();
        }

        ImGui::EndChild();
        ImGui::Dummy(ImVec2(0.0f, bottomPaneSpacing));
    }

    ImGui::BeginChild("ModelScratchpadPane", ImVec2(0.0f, 0.0f), true);
    drawScratchpad();
    ImGui::EndChild();
}
