#ifndef STUDIOSLAB_SIMULATIONS_V2_ANALYSIS_ATTACHMENT_V2_H
#define STUDIOSLAB_SIMULATIONS_V2_ANALYSIS_ATTACHMENT_V2_H

#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"

#include <functional>

namespace Slab::Studios::Common::Simulations::V2 {

    struct FAnalysisAttachmentV2 {
        Vector<Math::Numerics::V2::FSubscriptionV2> ExtraSubscriptions;
        std::function<void()> PostRunSummary = {};

        [[nodiscard]] auto HasExtraSubscriptions() const -> bool {
            return !ExtraSubscriptions.empty();
        }

        [[nodiscard]] auto HasPostRunSummary() const -> bool {
            return static_cast<bool>(PostRunSummary);
        }
    };

    [[nodiscard]] inline auto WrapRecipeWithExtraSubscriptionsV2(
            const TPointer<Math::Numerics::V2::FSimulationRecipeV2> &recipe,
            Vector<Math::Numerics::V2::FSubscriptionV2> extraSubscriptions)
            -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        if (recipe == nullptr) throw Exception("WrapRecipeWithExtraSubscriptionsV2 requires a valid recipe.");
        if (extraSubscriptions.empty()) return recipe;
        return New<Math::Numerics::V2::FAppendedSubscriptionsRecipeV2>(recipe, std::move(extraSubscriptions));
    }

    [[nodiscard]] inline auto WrapRecipeWithAnalysisAttachmentV2(
            const TPointer<Math::Numerics::V2::FSimulationRecipeV2> &recipe,
            const FAnalysisAttachmentV2 &attachment)
            -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        return WrapRecipeWithExtraSubscriptionsV2(recipe, attachment.ExtraSubscriptions);
    }

    inline auto InvokePostRunSummaryV2(const FAnalysisAttachmentV2 &attachment) -> void {
        if (attachment.PostRunSummary) attachment.PostRunSummary();
    }

} // namespace Slab::Studios::Common::Simulations::V2

#endif // STUDIOSLAB_SIMULATIONS_V2_ANALYSIS_ATTACHMENT_V2_H
