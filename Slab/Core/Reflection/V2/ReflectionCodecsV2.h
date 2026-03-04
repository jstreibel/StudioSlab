#ifndef STUDIOSLAB_REFLECTION_CODECS_V2_H
#define STUDIOSLAB_REFLECTION_CODECS_V2_H

#include "ReflectionTypesV2.h"
#include "Core/Controller/Parameter/Parameter.h"

#include <cctype>
#include <cmath>
#include <map>
#include <sstream>

namespace Slab::Core::Reflection::V2 {

    inline constexpr auto CTypeIdScalarBool = "slab.scalar.bool";
    inline constexpr auto CTypeIdScalarInt32 = "slab.scalar.int32";
    inline constexpr auto CTypeIdScalarFloat64 = "slab.scalar.float64";
    inline constexpr auto CTypeIdScalarString = "slab.scalar.string";
    inline constexpr auto CTypeIdScalarStringList = "slab.scalar.string_list";
    inline constexpr auto CTypeIdObjectRef = "slab.object.ref";
    inline constexpr auto CTypeIdFunctionRtoR = "slab.math.function.r_to_r";
    inline constexpr auto CTypeIdFunctionR2toR = "slab.math.function.r2_to_r";

    struct FFunctionDescriptorV2 {
        Str Family = "analytic";
        Str Expression = "sin(x)";
        DevFloat DomainMin = -1.0;
        DevFloat DomainMax = 1.0;
    };

    inline auto EscapeCodecToken(const Str &raw) -> Str {
        Str out;
        out.reserve(raw.size());

        for (const char ch : raw) {
            if (ch == '\\' || ch == ';' || ch == '=') {
                out.push_back('\\');
            }
            out.push_back(ch);
        }

        return out;
    }

    inline auto UnescapeCodecToken(const Str &raw) -> Str {
        Str out;
        out.reserve(raw.size());

        bool escape = false;
        for (const char ch : raw) {
            if (escape) {
                out.push_back(ch);
                escape = false;
                continue;
            }

            if (ch == '\\') {
                escape = true;
                continue;
            }

            out.push_back(ch);
        }

        if (escape) out.push_back('\\');
        return out;
    }

    inline auto SplitEscaped(const Str &raw, const char delimiter) -> StrVector {
        StrVector tokens;
        Str token;
        token.reserve(raw.size());

        bool escape = false;
        for (const char ch : raw) {
            if (escape) {
                token.push_back(ch);
                escape = false;
                continue;
            }

            if (ch == '\\') {
                escape = true;
                continue;
            }

            if (ch == delimiter) {
                tokens.push_back(token);
                token.clear();
                continue;
            }

            token.push_back(ch);
        }

        tokens.push_back(token);
        return tokens;
    }

    inline auto EncodeFunctionDescriptorCLI(const FFunctionDescriptorV2 &descriptor) -> Str {
        StringStream ss;
        ss << "family=" << EscapeCodecToken(descriptor.Family)
           << ";expr=" << EscapeCodecToken(descriptor.Expression)
           << ";domain_min=" << descriptor.DomainMin
           << ";domain_max=" << descriptor.DomainMax;

        return ss.str();
    }

    inline auto DecodeFunctionDescriptorCLI(const Str &encoded,
                                            FFunctionDescriptorV2 &out,
                                            Str *pErrorMessage = nullptr) -> bool {
        std::map<Str, Str> kv;

        const auto pairs = SplitEscaped(encoded, ';');
        for (const auto &pair : pairs) {
            if (pair.empty()) continue;

            const auto split = SplitEscaped(pair, '=');
            if (split.empty()) continue;
            if (split.size() < 2) {
                if (pErrorMessage != nullptr) {
                    *pErrorMessage = "Invalid token '" + pair + "' while decoding function descriptor.";
                }
                return false;
            }

            const Str key = split[0];
            Str value;
            for (std::size_t i = 1; i < split.size(); ++i) {
                value += split[i];
                if (i + 1 < split.size()) value += "=";
            }

            kv[key] = UnescapeCodecToken(value);
        }

        if (kv.contains("family")) out.Family = kv["family"];
        if (kv.contains("expr")) out.Expression = kv["expr"];

        try {
            if (kv.contains("domain_min")) out.DomainMin = std::stod(kv["domain_min"]);
            if (kv.contains("domain_max")) out.DomainMax = std::stod(kv["domain_max"]);
        } catch (const std::exception &e) {
            if (pErrorMessage != nullptr) {
                *pErrorMessage = Str("Failed to decode function domain bounds: ") + e.what();
            }
            return false;
        }

        if (out.DomainMax < out.DomainMin) {
            if (pErrorMessage != nullptr) {
                *pErrorMessage = "Function descriptor domain_max must be >= domain_min.";
            }
            return false;
        }

        return true;
    }

    inline auto MakeEncodedValue(Str TypeId, Str Encoded) -> FReflectionValueV2 {
        return FReflectionValueV2(std::move(TypeId), std::move(Encoded));
    }

    inline auto MakeStringValue(const Str &value) -> FReflectionValueV2 {
        return FReflectionValueV2(CTypeIdScalarString, value);
    }

    inline auto MakeBoolValue(const bool value) -> FReflectionValueV2 {
        return FReflectionValueV2(CTypeIdScalarBool, value ? "true" : "false");
    }

    inline auto MakeIntValue(const int value) -> FReflectionValueV2 {
        return FReflectionValueV2(CTypeIdScalarInt32, ToStr(value));
    }

    inline auto MakeFloatValue(const DevFloat value) -> FReflectionValueV2 {
        return FReflectionValueV2(CTypeIdScalarFloat64, ToStr(value));
    }

    inline auto MakeStringListValue(const StrVector &items) -> FReflectionValueV2 {
        StringStream ss;
        for (std::size_t i = 0; i < items.size(); ++i) {
            ss << EscapeCodecToken(items[i]);
            if (i + 1 < items.size()) ss << ",";
        }

        return FReflectionValueV2(CTypeIdScalarStringList, ss.str());
    }

    inline auto ParseBoolValue(const Str &raw, bool &out) -> bool {
        Str lowered;
        lowered.reserve(raw.size());
        for (const char ch : raw) lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));

        if (lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on") {
            out = true;
            return true;
        }

        if (lowered == "0" || lowered == "false" || lowered == "no" || lowered == "off") {
            out = false;
            return true;
        }

        return false;
    }

    inline auto ParseStringListValue(const Str &raw) -> StrVector {
        StrVector out;
        const auto parts = SplitEscaped(raw, ',');
        out.reserve(parts.size());

        for (const auto &part : parts) {
            out.push_back(UnescapeCodecToken(part));
        }

        return out;
    }

    inline auto InferTypeIdFromLegacyParameterType(const Core::EParameterType type) -> Str {
        switch (type) {
            case Core::EParameterType::ParameterType_Integer:
                return CTypeIdScalarInt32;
            case Core::EParameterType::ParameterType_Float:
                return CTypeIdScalarFloat64;
            case Core::EParameterType::ParameterType_String:
                return CTypeIdScalarString;
            case Core::EParameterType::ParameterType_MultiString:
                return CTypeIdScalarStringList;
            case Core::EParameterType::ParameterType_Bool:
                return CTypeIdScalarBool;
            case Core::EParameterType::ParameterType_Uninformed:
                break;
        }

        return CTypeIdObjectRef;
    }

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_REFLECTION_CODECS_V2_H
