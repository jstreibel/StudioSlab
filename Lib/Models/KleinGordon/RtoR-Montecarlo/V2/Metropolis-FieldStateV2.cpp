#include "Metropolis-FieldStateV2.h"

namespace Slab::Models::KGRtoR::Metropolis::V2 {

    auto FMetropolisFieldStateV2::Cast(const Math::Base::EquationState &state) -> const FMetropolisFieldStateV2 & {
        return dynamic_cast<const FMetropolisFieldStateV2 &>(state);
    }

    auto FMetropolisFieldStateV2::Cast(Math::Base::EquationState &state) -> FMetropolisFieldStateV2 & {
        return dynamic_cast<FMetropolisFieldStateV2 &>(state);
    }

    FMetropolisFieldStateV2::FMetropolisFieldStateV2(
        const TPointer<Math::RtoR::NumericFunction_CPU> &phiField,
        const TPointer<Math::RtoR::NumericFunction_CPU> &piField)
    : PhiField(phiField)
    , PiField(piField) {
        if (PhiField == nullptr || PiField == nullptr) {
            throw Exception("FMetropolisFieldStateV2 requires non-null phi/pi fields.");
        }
    }

    auto FMetropolisFieldStateV2::EnsureCompatible(const FMetropolisFieldStateV2 &other) const -> void {
        if (PhiField == nullptr || PiField == nullptr || other.PhiField == nullptr || other.PiField == nullptr) {
            throw Exception("FMetropolisFieldStateV2 is missing phi/pi fields.");
        }
        if (PhiField->N != other.PhiField->N || PiField->N != other.PiField->N) {
            throw Exception("FMetropolisFieldStateV2 incompatible field sizes.");
        }
        if (PhiField->xMin != other.PhiField->xMin || PhiField->xMax != other.PhiField->xMax ||
            PiField->xMin != other.PiField->xMin || PiField->xMax != other.PiField->xMax) {
            throw Exception("FMetropolisFieldStateV2 incompatible field domains.");
        }
    }

    auto FMetropolisFieldStateV2::GetPhiField() const -> TPointer<Math::RtoR::NumericFunction_CPU> {
        return PhiField;
    }

    auto FMetropolisFieldStateV2::GetPiField() const -> TPointer<Math::RtoR::NumericFunction_CPU> {
        return PiField;
    }

    auto FMetropolisFieldStateV2::Replicate(std::optional<Str> name) const -> TPointer<Math::Base::EquationState> {
        auto phiClone = DynamicPointerCast<Math::RtoR::NumericFunction_CPU>(PhiField->Clone());
        auto piClone = DynamicPointerCast<Math::RtoR::NumericFunction_CPU>(PiField->Clone());
        if (phiClone == nullptr || piClone == nullptr) {
            throw Exception("FMetropolisFieldStateV2 failed to clone phi/pi fields.");
        }
        if (name.has_value()) {
            phiClone->change_data_name(*name + "_phi");
            piClone->change_data_name(*name + "_pi");
        }

        return New<FMetropolisFieldStateV2>(phiClone, piClone);
    }

    auto FMetropolisFieldStateV2::category() const -> Str {
        return "mc-kgrtor|R->R";
    }

    auto FMetropolisFieldStateV2::setData(const Math::Base::EquationState &state) -> void {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        PhiField->getSpace().setToValue(other.PhiField->getSpace());
        PiField->getSpace().setToValue(other.PiField->getSpace());
    }

    auto FMetropolisFieldStateV2::Add(const Math::Base::EquationState &state) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        PhiField->Add(*other.PhiField);
        PiField->Add(*other.PiField);
        return *this;
    }

    auto FMetropolisFieldStateV2::Subtract(const Math::Base::EquationState &state) -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        PhiField->Subtract(*other.PhiField);
        PiField->Subtract(*other.PiField);
        return *this;
    }

    auto FMetropolisFieldStateV2::StoreAddition(const Math::Base::EquationState &state1,
                                                const Math::Base::EquationState &state2)
        -> Math::Base::EquationState & {
        const auto &a = Cast(state1);
        const auto &b = Cast(state2);
        EnsureCompatible(a);
        EnsureCompatible(b);

        PhiField->StoreAddition(*a.PhiField, *b.PhiField);
        PiField->StoreAddition(*a.PiField, *b.PiField);
        return *this;
    }

    auto FMetropolisFieldStateV2::StoreSubtraction(const Math::Base::EquationState &state1,
                                                   const Math::Base::EquationState &state2)
        -> Math::Base::EquationState & {
        const auto &a = Cast(state1);
        const auto &b = Cast(state2);
        EnsureCompatible(a);
        EnsureCompatible(b);

        PhiField->StoreSubtraction(*a.PhiField, *b.PhiField);
        PiField->StoreSubtraction(*a.PiField, *b.PiField);
        return *this;
    }

    auto FMetropolisFieldStateV2::StoreScalarMultiplication(const Math::Base::EquationState &state,
                                                            const DevFloat a)
        -> Math::Base::EquationState & {
        const auto &other = Cast(state);
        EnsureCompatible(other);

        PhiField->StoreScalarMultiplication(*other.PhiField, a);
        PiField->StoreScalarMultiplication(*other.PiField, a);
        return *this;
    }

    auto FMetropolisFieldStateV2::Multiply(const DevFloat a) -> Math::Base::EquationState & {
        PhiField->Multiply(a);
        PiField->Multiply(a);
        return *this;
    }

} // namespace Slab::Models::KGRtoR::Metropolis::V2
