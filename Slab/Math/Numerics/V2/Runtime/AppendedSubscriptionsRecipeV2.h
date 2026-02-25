#ifndef STUDIOSLAB_APPENDED_SUBSCRIPTIONS_RECIPE_V2_H
#define STUDIOSLAB_APPENDED_SUBSCRIPTIONS_RECIPE_V2_H

#include "SimulationRecipeV2.h"

namespace Slab::Math::Numerics::V2 {

    class FAppendedSubscriptionsRecipeV2 final : public FSimulationRecipeV2 {
        TPointer<FSimulationRecipeV2> InnerRecipe;
        Vector<FSubscriptionV2> ExtraSubscriptions;

    public:
        FAppendedSubscriptionsRecipeV2(const TPointer<FSimulationRecipeV2> &innerRecipe,
                                      Vector<FSubscriptionV2> extraSubscriptions)
        : InnerRecipe(innerRecipe)
        , ExtraSubscriptions(std::move(extraSubscriptions)) {
            if (InnerRecipe == nullptr) throw Exception("FAppendedSubscriptionsRecipeV2 requires a valid inner recipe.");
        }

        auto SetupForCurrentThread() -> void override {
            InnerRecipe->SetupForCurrentThread();
        }

        auto BuildSession() -> TPointer<FSimulationSessionV2> override {
            return InnerRecipe->BuildSession();
        }

        auto BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> override {
            auto subscriptions = InnerRecipe->BuildDefaultSubscriptions();
            subscriptions.insert(subscriptions.end(), ExtraSubscriptions.begin(), ExtraSubscriptions.end());
            return subscriptions;
        }

        [[nodiscard]] auto GetRunLimits() const -> FRunLimitsV2 override {
            return InnerRecipe->GetRunLimits();
        }

        [[nodiscard]] auto GetInnerRecipe() const -> TPointer<FSimulationRecipeV2> {
            return InnerRecipe;
        }
    };

    DefinePointers(FAppendedSubscriptionsRecipeV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_APPENDED_SUBSCRIPTIONS_RECIPE_V2_H
