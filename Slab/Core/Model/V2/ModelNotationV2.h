#ifndef STUDIOSLAB_MODEL_NOTATION_V2_H
#define STUDIOSLAB_MODEL_NOTATION_V2_H

#include "ModelTypesV2.h"

#include <regex>

namespace Slab::Core::Model::V2 {

    struct FNotationErrorV2 {
        std::size_t Position = 0;
        Str Message;
    };

    template<typename TValue>
    struct FNotationParseResultV2 {
        TOptional<TValue> Value;
        FNotationErrorV2 Error;

        [[nodiscard]] auto IsOk() const -> bool {
            return Value.has_value();
        }

        static auto Ok(TValue value) -> FNotationParseResultV2<TValue> {
            FNotationParseResultV2<TValue> result;
            result.Value = std::move(value);
            return result;
        }

        static auto Fail(std::size_t position, Str message) -> FNotationParseResultV2<TValue> {
            FNotationParseResultV2<TValue> result;
            result.Error = FNotationErrorV2{position, std::move(message)};
            return result;
        }
    };

    struct FParsedDefinitionNotationV2 {
        Str CanonicalSymbol;
        Str SourceSymbolText;
        bool bMembership = false;
        TOptional<FSpaceExprV2> MembershipSpace;
        TOptional<FTypeExprV2> DeclaredType;
        Str SourceText;
    };

    inline auto NormalizeNotationSourceV2(Str source) -> Str {
        static const std::regex secondDerivativePattern(
            R"(\bd([0-9]+)\s*/\s*d([A-Za-z_][A-Za-z0-9_]*)\s*([0-9]+)\s+([A-Za-z_\\][A-Za-z0-9_\\]*)\b)");
        source = std::regex_replace(source, secondDerivativePattern, "\\\\frac{d^$1 $4}{d $2^$3}");

        static const std::regex firstDerivativePattern(
            R"(\bd\s*/\s*d([A-Za-z_][A-Za-z0-9_]*)\s+([A-Za-z_\\][A-Za-z0-9_\\]*)\b)");
        source = std::regex_replace(source, firstDerivativePattern, "\\\\frac{d $2}{d $1}");

        return source;
    }

    inline auto NormalizeSymbolAliasV2(Str alias) -> Str {
        return NormalizeSymbolAliasTextV2(std::move(alias));
    }

    class FNotationContextV2 {
        const FModelV2 *Model = nullptr;
        std::map<Str, FReferenceV2> BindingsByAlias;

        auto RegisterAlias(const Str &alias, const FDefinitionV2 &definition) -> void {
            const auto normalized = NormalizeSymbolAliasV2(alias);
            if (normalized.empty()) return;
            BindingsByAlias[normalized] = FReferenceV2{definition.DefinitionId, alias};
        }

        auto RegisterAlias(const Str &alias, const FBaseVocabularyEntryV2 &entry, const bool overwriteExisting = false) -> void {
            const auto normalized = NormalizeSymbolAliasV2(alias);
            if (normalized.empty()) return;
            if (!overwriteExisting && BindingsByAlias.contains(normalized)) return;
            BindingsByAlias[normalized] = FReferenceV2{entry.EntryId, alias};
        }

    public:
        FNotationContextV2() = default;

        explicit FNotationContextV2(const FModelV2 &model)
        : Model(&model) {
            for (const auto &definition : model.Definitions) {
                RegisterAlias(definition.Symbol, definition);
                if (!definition.PreferredNotation.empty()) RegisterAlias(definition.PreferredNotation, definition);
            }
            for (const auto &entry : ResolveBaseVocabularyPresetEntriesV2(model.BaseVocabulary.ActivePresetId)) {
                RegisterAlias(entry.Symbol, entry);
                if (!entry.PreferredNotation.empty()) RegisterAlias(entry.PreferredNotation, entry);
            }
        }

        static auto FromModel(const FModelV2 &model) -> FNotationContextV2 {
            return FNotationContextV2(model);
        }

        [[nodiscard]] auto FindReference(const Str &alias) const -> TOptional<FReferenceV2> {
            const auto normalized = NormalizeSymbolAliasV2(alias);
            const auto it = BindingsByAlias.find(normalized);
            if (it == BindingsByAlias.end()) return std::nullopt;

            auto reference = it->second;
            reference.SymbolText = alias;
            return reference;
        }

        [[nodiscard]] auto FindDefinition(const FReferenceV2 &reference) const -> const FDefinitionV2 * {
            if (Model == nullptr || !reference.IsBound()) return nullptr;
            return FindDefinitionByIdV2(*Model, reference.ReferenceId);
        }

        [[nodiscard]] auto FindSemanticEntry(const FReferenceV2 &reference) const -> FResolvedSemanticEntryV2 {
            if (Model == nullptr || !reference.IsBound()) return {};
            return ResolveSemanticEntryV2(*Model, reference);
        }

        [[nodiscard]] auto FindDefaultDerivativeVariable(const FReferenceV2 &reference) const -> TOptional<FReferenceV2> {
            const auto resolved = FindSemanticEntry(reference);
            if (resolved.Definition == nullptr) return std::nullopt;
            const auto *definition = resolved.Definition;
            if (definition->ArgumentDefinitionIds.empty()) return std::nullopt;

            const auto *coordinate = FindDefinitionByIdV2(*Model, definition->ArgumentDefinitionIds.front());
            if (coordinate == nullptr) return std::nullopt;
            return FReferenceV2{
                coordinate->DefinitionId,
                coordinate->PreferredNotation.empty() ? coordinate->Symbol : coordinate->PreferredNotation
            };
        }
    };

    namespace Detail {

        enum class ETokenKindV2 : unsigned char {
            Identifier,
            Number,
            Command,
            Colon,
            Arrow,
            In,
            Plus,
            Minus,
            Asterisk,
            Slash,
            Caret,
            Comma,
            Equal,
            LParen,
            RParen,
            LBrace,
            RBrace,
            End
        };

        struct FTokenV2 {
            ETokenKindV2 Kind = ETokenKindV2::End;
            Str Text;
            std::size_t Position = 0;
        };

        inline auto CanStartPrimaryV2(const FTokenV2 &token) -> bool {
            if (token.Kind == ETokenKindV2::Identifier) return true;
            if (token.Kind == ETokenKindV2::Number) return true;
            if (token.Kind == ETokenKindV2::Command) return true;
            if (token.Kind == ETokenKindV2::LParen) return true;
            return false;
        }

        class FTokenizerV2 {
            Str Source;
            std::size_t Cursor = 0;
            Vector<FTokenV2> Tokens;

            [[nodiscard]] auto PeekChar() const -> char {
                if (Cursor >= Source.size()) return '\0';
                return Source[Cursor];
            }

            auto Advance() -> char {
                if (Cursor >= Source.size()) return '\0';
                return Source[Cursor++];
            }

            auto Push(const ETokenKindV2 kind, const std::size_t position, Str text) -> void {
                Tokens.push_back(FTokenV2{kind, std::move(text), position});
            }

            auto ConsumeWhile(const auto &predicate) -> Str {
                Str text;
                while (Cursor < Source.size() && predicate(Source[Cursor])) {
                    text.push_back(Source[Cursor]);
                    ++Cursor;
                }
                return text;
            }

        public:
            explicit FTokenizerV2(Str source)
            : Source(std::move(source)) {
            }

            auto Tokenize() -> Vector<FTokenV2> {
                Tokens.clear();
                Cursor = 0;

                while (Cursor < Source.size()) {
                    const auto position = Cursor;
                    const auto c = PeekChar();
                    if (std::isspace(static_cast<unsigned char>(c))) {
                        ++Cursor;
                        continue;
                    }

                    if (c == '\\') {
                        Advance();
                        auto command = ConsumeWhile([](const unsigned char ch) {
                            return std::isalpha(ch) != 0;
                        });
                        if (command == "to") {
                            Push(ETokenKindV2::Arrow, position, "\\to");
                        } else if (command == "in") {
                            Push(ETokenKindV2::In, position, "\\in");
                        } else {
                            Push(ETokenKindV2::Command, position, "\\" + command);
                        }
                        continue;
                    }

                    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                        auto identifier = ConsumeWhile([](const unsigned char ch) {
                            return std::isalnum(ch) != 0 || ch == '_';
                        });
                        if (identifier == "in") Push(ETokenKindV2::In, position, identifier);
                        else Push(ETokenKindV2::Identifier, position, identifier);
                        continue;
                    }

                    if (std::isdigit(static_cast<unsigned char>(c)) != 0 || c == '.') {
                        auto number = ConsumeWhile([](const unsigned char ch) {
                            return std::isdigit(ch) != 0 || ch == '.';
                        });
                        Push(ETokenKindV2::Number, position, number);
                        continue;
                    }

                    if (c == '-' && Cursor + 1 < Source.size() && Source[Cursor + 1] == '>') {
                        Cursor += 2;
                        Push(ETokenKindV2::Arrow, position, "->");
                        continue;
                    }

                    Advance();
                    switch (c) {
                        case ':': Push(ETokenKindV2::Colon, position, ":"); break;
                        case '+': Push(ETokenKindV2::Plus, position, "+"); break;
                        case '-': Push(ETokenKindV2::Minus, position, "-"); break;
                        case '*': Push(ETokenKindV2::Asterisk, position, "*"); break;
                        case '/': Push(ETokenKindV2::Slash, position, "/"); break;
                        case '^': Push(ETokenKindV2::Caret, position, "^"); break;
                        case ',': Push(ETokenKindV2::Comma, position, ","); break;
                        case '=': Push(ETokenKindV2::Equal, position, "="); break;
                        case '(': Push(ETokenKindV2::LParen, position, "("); break;
                        case ')': Push(ETokenKindV2::RParen, position, ")"); break;
                        case '{': Push(ETokenKindV2::LBrace, position, "{"); break;
                        case '}': Push(ETokenKindV2::RBrace, position, "}"); break;
                        default:
                            Push(ETokenKindV2::Identifier, position, Str(1, c));
                            break;
                    }
                }

                Tokens.push_back(FTokenV2{ETokenKindV2::End, {}, Source.size()});
                return Tokens;
            }
        };

        class FParserV2 {
            const Vector<FTokenV2> Tokens;
            std::size_t Cursor = 0;
            const FNotationContextV2 *Context = nullptr;
            TOptional<FNotationErrorV2> Error;

            [[nodiscard]] auto Peek() const -> const FTokenV2 & {
                return Tokens[std::min(Cursor, Tokens.size() - 1)];
            }

            [[nodiscard]] auto Previous() const -> const FTokenV2 & {
                return Tokens[Cursor - 1];
            }

            [[nodiscard]] auto IsAtEnd() const -> bool {
                return Peek().Kind == ETokenKindV2::End;
            }

            auto Advance() -> const FTokenV2 & {
                if (!IsAtEnd()) ++Cursor;
                return Previous();
            }

            auto Match(const ETokenKindV2 kind) -> bool {
                if (Peek().Kind != kind) return false;
                Advance();
                return true;
            }

            auto MatchCommand(const char *commandText) -> bool {
                if (Peek().Kind != ETokenKindV2::Command) return false;
                if (Peek().Text != commandText) return false;
                Advance();
                return true;
            }

            auto FailHere(Str message) -> void {
                if (Error.has_value()) return;
                Error = FNotationErrorV2{Peek().Position, std::move(message)};
            }

            auto Expect(const ETokenKindV2 kind, Str message) -> bool {
                if (Match(kind)) return true;
                FailHere(std::move(message));
                return false;
            }

            [[nodiscard]] auto MakeReferenceFromToken(const FTokenV2 &token) const -> FReferenceV2 {
                if (Context != nullptr) {
                    if (const auto resolved = Context->FindReference(token.Text); resolved.has_value()) {
                        auto reference = *resolved;
                        reference.SymbolText = token.Text;
                        return reference;
                    }
                }
                return FReferenceV2{{}, token.Text};
            }

            [[nodiscard]] auto ParseReferenceToken(Str *pSourceText = nullptr) -> TOptional<FReferenceV2> {
                if (Peek().Kind != ETokenKindV2::Identifier && Peek().Kind != ETokenKindV2::Command) {
                    FailHere("expected symbol");
                    return std::nullopt;
                }

                const auto token = Advance();
                if (pSourceText != nullptr) *pSourceText = token.Text;
                return MakeReferenceFromToken(token);
            }

            auto ParseGroupedExpression(const ETokenKindV2 openKind,
                                        const ETokenKindV2 closeKind,
                                        const char *label) -> FExpressionPtrV2 {
                if (!Expect(openKind, Str("expected '") + label + "'")) return nullptr;
                auto expression = ParseExpression();
                (void) Expect(closeKind, Str("expected matching close for '") + label + "'");
                return expression;
            }

            auto ParseTypeExponent() -> TOptional<FExpressionPtrV2> {
                if (!Match(ETokenKindV2::Caret)) return std::nullopt;

                if (Match(ETokenKindV2::LBrace)) {
                    auto expression = ParseExpression();
                    (void) Expect(ETokenKindV2::RBrace, "expected '}' after type exponent");
                    return expression;
                }

                if (Match(ETokenKindV2::LParen)) {
                    auto expression = ParseExpression();
                    (void) Expect(ETokenKindV2::RParen, "expected ')' after type exponent");
                    return expression;
                }

                return ParsePrimaryExpression();
            }

            auto ParseSpaceExpr() -> TOptional<FSpaceExprV2> {
                FSpaceExprV2 space;
                space.SourceText = Peek().Text;

                if (MatchCommand("\\mathbb")) {
                    if (!Expect(ETokenKindV2::LBrace, "expected '{' after \\mathbb")) return std::nullopt;
                    if (Peek().Kind != ETokenKindV2::Identifier && Peek().Kind != ETokenKindV2::Command) {
                        FailHere("expected symbol inside \\mathbb{...}");
                        return std::nullopt;
                    }
                    const auto token = Advance();
                    if (!Expect(ETokenKindV2::RBrace, "expected '}' after \\mathbb payload")) return std::nullopt;

                    const auto payload = NormalizeSymbolAliasV2(token.Text);
                    if (payload == "R") {
                        space = MakeRealSpaceV2();
                    } else if (payload == "C") {
                        space = MakeComplexSpaceV2();
                    } else {
                        space = MakeNamedSpaceV2(payload);
                    }
                } else if (Peek().Kind == ETokenKindV2::Identifier || Peek().Kind == ETokenKindV2::Command) {
                    const auto token = Advance();
                    const auto payload = NormalizeSymbolAliasV2(token.Text);
                    if (payload == "R") {
                        space = MakeRealSpaceV2();
                    } else if (payload == "C") {
                        space = MakeComplexSpaceV2();
                    } else {
                        space = MakeNamedSpaceV2(payload);
                    }
                } else {
                    FailHere("expected space expression");
                    return std::nullopt;
                }

                if (const auto dimension = ParseTypeExponent(); dimension.has_value()) {
                    space.Dimension = *dimension;
                }

                return space;
            }

            auto ParseTypeExpr() -> TOptional<FTypeExprV2> {
                const auto firstSpace = ParseSpaceExpr();
                if (!firstSpace.has_value()) return std::nullopt;

                if (!Match(ETokenKindV2::Arrow)) {
                    auto type = MakeSpaceTypeV2(*firstSpace);
                    type.SourceText = RenderTypeExprV2(type);
                    return type;
                }

                const auto codomainSpace = ParseSpaceExpr();
                if (!codomainSpace.has_value()) return std::nullopt;

                auto type = MakeFunctionTypeV2({MakeSpaceTypeV2(*firstSpace)}, MakeSpaceTypeV2(*codomainSpace));
                type.SourceText = RenderTypeExprV2(type);
                return type;
            }

            auto ParsePrimaryExpression() -> FExpressionPtrV2 {
                if (Error.has_value()) return nullptr;

                if (Match(ETokenKindV2::Number)) {
                    return MakeLiteralExprV2(Previous().Text, Previous().Text);
                }

                if (Match(ETokenKindV2::LParen)) {
                    auto expression = ParseExpression();
                    (void) Expect(ETokenKindV2::RParen, "expected ')' after expression");
                    return expression;
                }

                if (Match(ETokenKindV2::LBrace)) {
                    auto expression = ParseExpression();
                    (void) Expect(ETokenKindV2::RBrace, "expected '}' after expression");
                    return expression;
                }

                if (MatchCommand("\\frac")) {
                    if (!Expect(ETokenKindV2::LBrace, "expected '{' after \\frac")) return nullptr;
                    if ((Peek().Kind != ETokenKindV2::Identifier && Peek().Kind != ETokenKindV2::Command) ||
                        NormalizeSymbolAliasV2(Peek().Text) != "d" &&
                        NormalizeSymbolAliasV2(Peek().Text) != "partial") {
                        FailHere("only derivative fractions are supported after \\frac");
                        return nullptr;
                    }

                    const auto derivativeToken = Advance();
                    const auto flavor = NormalizeSymbolAliasV2(derivativeToken.Text) == "partial"
                        ? EDerivativeFlavorV2::Partial
                        : EDerivativeFlavorV2::Ordinary;

                    int order = 1;
                    if (Match(ETokenKindV2::Caret)) {
                        if (Peek().Kind != ETokenKindV2::Number) {
                            FailHere("derivative order must be numeric");
                            return nullptr;
                        }
                        order = std::max(1, std::stoi(Advance().Text));
                    }

                    auto target = ParseExpression();
                    (void) Expect(ETokenKindV2::RBrace, "expected '}' after derivative numerator");
                    if (!Expect(ETokenKindV2::LBrace, "expected '{' before derivative denominator")) return nullptr;

                    if ((Peek().Kind != ETokenKindV2::Identifier && Peek().Kind != ETokenKindV2::Command) ||
                        NormalizeSymbolAliasV2(Peek().Text) != "d" &&
                        NormalizeSymbolAliasV2(Peek().Text) != "partial") {
                        FailHere("derivative denominator must start with d or \\partial");
                        return nullptr;
                    }
                    Advance();

                    Str variableSource;
                    const auto variable = ParseReferenceToken(&variableSource);
                    if (!variable.has_value()) return nullptr;

                    if (Match(ETokenKindV2::Caret)) {
                        if (Peek().Kind != ETokenKindV2::Number) {
                            FailHere("derivative denominator order must be numeric");
                            return nullptr;
                        }
                        order = std::max(1, std::stoi(Advance().Text));
                    }

                    (void) Expect(ETokenKindV2::RBrace, "expected '}' after derivative denominator");

                    Vector<FReferenceV2> variables;
                    variables.reserve(static_cast<std::size_t>(order));
                    for (int i = 0; i < order; ++i) variables.push_back(*variable);
                    return MakeDerivativeExprV2(flavor, std::move(variables), std::move(target), "\\frac");
                }

                if (MatchCommand("\\dot") || MatchCommand("\\ddot")) {
                    const auto derivativeCommand = Previous().Text;
                    const int order = derivativeCommand == "\\ddot" ? 2 : 1;
                    auto target = ParsePrimaryExpression();
                    if (target == nullptr) return nullptr;

                    Vector<FReferenceV2> variables;
                    if (target->Kind == EExpressionKindV2::Symbol ||
                        target->Kind == EExpressionKindV2::FunctionApplication) {
                        const auto defaultVariable = Context != nullptr
                            ? Context->FindDefaultDerivativeVariable(target->Reference)
                            : std::nullopt;
                        if (!defaultVariable.has_value()) {
                            FailHere("cannot infer derivative variable for '" + RenderExpressionCompactV2(target) + "'");
                            return nullptr;
                        }
                        for (int i = 0; i < order; ++i) variables.push_back(*defaultVariable);
                    } else {
                        FailHere("dot derivatives require a symbol or function target");
                        return nullptr;
                    }

                    return MakeDerivativeExprV2(EDerivativeFlavorV2::Ordinary, std::move(variables), std::move(target), derivativeCommand);
                }

                if (Peek().Kind == ETokenKindV2::Identifier || Peek().Kind == ETokenKindV2::Command) {
                    const auto token = Advance();
                    auto reference = MakeReferenceFromToken(token);

                    if (reference.IsBound() && Context != nullptr) {
                        const auto resolved = Context->FindSemanticEntry(reference);
                        const auto declaredKind = resolved.GetDeclaredKind();
                        if (resolved.IsResolved() &&
                            declaredKind.has_value() &&
                            *declaredKind == EDefinitionKindV2::OperatorSymbol &&
                            CanStartPrimaryV2(Peek())) {
                            Vector<FExpressionPtrV2> arguments;
                            const auto *declaredType = resolved.GetDeclaredType();
                            const auto domainCount = declaredType != nullptr &&
                                declaredType->Kind == ETypeExprKindV2::Function
                                ? declaredType->Domain.size()
                                : std::size_t{1};
                            for (std::size_t i = 0; i < std::max<std::size_t>(domainCount, 1); ++i) {
                                auto argument = ParseUnaryExpression();
                                if (argument == nullptr) return nullptr;
                                arguments.push_back(std::move(argument));
                            }
                            return MakeOperatorApplicationV2(reference, std::move(arguments), token.Text);
                        }
                    }

                    if (Match(ETokenKindV2::LParen)) {
                        Vector<FExpressionPtrV2> arguments;
                        if (!Match(ETokenKindV2::RParen)) {
                            do {
                                auto argument = ParseExpression();
                                if (argument == nullptr) return nullptr;
                                arguments.push_back(std::move(argument));
                            } while (Match(ETokenKindV2::Comma));
                            (void) Expect(ETokenKindV2::RParen, "expected ')' after argument list");
                        }
                        return MakeFunctionApplicationV2(reference, std::move(arguments), token.Text);
                    }

                    return MakeSymbolExprV2(reference, token.Text);
                }

                FailHere("expected primary expression");
                return nullptr;
            }

            auto ParseUnaryExpression() -> FExpressionPtrV2 {
                if (Match(ETokenKindV2::Plus)) {
                    return MakeUnaryExprV2(EUnaryOperatorV2::Plus, ParseUnaryExpression(), "+");
                }

                if (Match(ETokenKindV2::Minus)) {
                    return MakeUnaryExprV2(EUnaryOperatorV2::Minus, ParseUnaryExpression(), "-");
                }

                return ParsePrimaryExpression();
            }

            auto ParsePowerExpression() -> FExpressionPtrV2 {
                auto left = ParseUnaryExpression();
                if (left == nullptr) return nullptr;

                if (Match(ETokenKindV2::Caret)) {
                    auto right = ParsePowerExpression();
                    if (right == nullptr) return nullptr;
                    return PowExprV2(std::move(left), std::move(right));
                }

                return left;
            }

            auto ParseMultiplicativeExpression() -> FExpressionPtrV2 {
                auto expression = ParsePowerExpression();
                if (expression == nullptr) return nullptr;

                while (!Error.has_value()) {
                    if (Match(ETokenKindV2::Asterisk)) {
                        auto rhs = ParsePowerExpression();
                        expression = MulExprV2(std::move(expression), std::move(rhs), false);
                        continue;
                    }

                    if (Match(ETokenKindV2::Slash)) {
                        auto rhs = ParsePowerExpression();
                        expression = DivExprV2(std::move(expression), std::move(rhs));
                        continue;
                    }

                    if (CanStartPrimaryV2(Peek())) {
                        auto rhs = ParsePowerExpression();
                        expression = MulExprV2(std::move(expression), std::move(rhs), true);
                        continue;
                    }

                    break;
                }

                return expression;
            }

            auto ParseAdditiveExpression() -> FExpressionPtrV2 {
                auto expression = ParseMultiplicativeExpression();
                if (expression == nullptr) return nullptr;

                while (!Error.has_value()) {
                    if (Match(ETokenKindV2::Plus)) {
                        auto rhs = ParseMultiplicativeExpression();
                        expression = AddExprV2(std::move(expression), std::move(rhs));
                        continue;
                    }

                    if (Match(ETokenKindV2::Minus)) {
                        auto rhs = ParseMultiplicativeExpression();
                        expression = SubExprV2(std::move(expression), std::move(rhs));
                        continue;
                    }

                    break;
                }

                return expression;
            }

        public:
            FParserV2(Vector<FTokenV2> tokens, const FNotationContextV2 *context)
            : Tokens(std::move(tokens))
            , Context(context) {
            }

            auto ParseExpression() -> FExpressionPtrV2 {
                return ParseAdditiveExpression();
            }

            auto ParseEquation() -> TOptional<Pair<FExpressionPtrV2, FExpressionPtrV2>> {
                auto left = ParseExpression();
                if (left == nullptr) return std::nullopt;
                if (!Expect(ETokenKindV2::Equal, "expected '=' in relation")) return std::nullopt;
                auto right = ParseExpression();
                if (right == nullptr) return std::nullopt;
                if (!IsAtEnd()) {
                    FailHere("unexpected trailing tokens after relation");
                    return std::nullopt;
                }
                return Pair<FExpressionPtrV2, FExpressionPtrV2>{std::move(left), std::move(right)};
            }

            auto ParseDefinition() -> TOptional<FParsedDefinitionNotationV2> {
                Str sourceSymbol;
                const auto symbol = ParseReferenceToken(&sourceSymbol);
                if (!symbol.has_value()) return std::nullopt;

                FParsedDefinitionNotationV2 definition;
                definition.CanonicalSymbol = NormalizeSymbolAliasV2(sourceSymbol);
                definition.SourceSymbolText = sourceSymbol;

                if (Match(ETokenKindV2::In)) {
                    definition.bMembership = true;
                    const auto space = ParseSpaceExpr();
                    if (!space.has_value()) return std::nullopt;
                    definition.MembershipSpace = *space;
                } else if (Match(ETokenKindV2::Colon)) {
                    const auto type = ParseTypeExpr();
                    if (!type.has_value()) return std::nullopt;
                    definition.DeclaredType = *type;
                } else {
                    FailHere("expected ':' or '\\in' in definition notation");
                    return std::nullopt;
                }

                if (!IsAtEnd()) {
                    FailHere("unexpected trailing tokens after definition");
                    return std::nullopt;
                }

                return definition;
            }

            [[nodiscard]] auto GetError() const -> TOptional<FNotationErrorV2> {
                return Error;
            }
        };

    } // namespace Detail

    inline auto ParseDefinitionNotationV2(const Str &source,
                                          const FNotationContextV2 *context = nullptr)
        -> FNotationParseResultV2<FParsedDefinitionNotationV2> {
        const auto normalized = NormalizeNotationSourceV2(source);
        Detail::FTokenizerV2 tokenizer(normalized);
        Detail::FParserV2 parser(tokenizer.Tokenize(), context);
        const auto parsed = parser.ParseDefinition();
        if (!parsed.has_value()) {
            const auto error = parser.GetError().value_or(FNotationErrorV2{0, "failed to parse definition"});
            return FNotationParseResultV2<FParsedDefinitionNotationV2>::Fail(error.Position, error.Message);
        }

        auto definition = *parsed;
        definition.SourceText = source;
        return FNotationParseResultV2<FParsedDefinitionNotationV2>::Ok(std::move(definition));
    }

    inline auto ParseExpressionNotationV2(const Str &source,
                                          const FNotationContextV2 *context = nullptr)
        -> FNotationParseResultV2<FExpressionPtrV2> {
        const auto normalized = NormalizeNotationSourceV2(source);
        Detail::FTokenizerV2 tokenizer(normalized);
        Detail::FParserV2 parser(tokenizer.Tokenize(), context);
        auto expression = parser.ParseExpression();
        if (expression == nullptr || parser.GetError().has_value()) {
            const auto error = parser.GetError().value_or(FNotationErrorV2{0, "failed to parse expression"});
            return FNotationParseResultV2<FExpressionPtrV2>::Fail(error.Position, error.Message);
        }
        expression->SourceText = source;
        return FNotationParseResultV2<FExpressionPtrV2>::Ok(std::move(expression));
    }

    inline auto ParseRelationNotationV2(const Str &relationId,
                                        const ERelationKindV2 kind,
                                        const Str &source,
                                        const FNotationContextV2 *context = nullptr)
        -> FNotationParseResultV2<FRelationV2> {
        const auto normalized = NormalizeNotationSourceV2(source);
        Detail::FTokenizerV2 tokenizer(normalized);
        Detail::FParserV2 parser(tokenizer.Tokenize(), context);
        const auto equation = parser.ParseEquation();
        if (!equation.has_value()) {
            const auto error = parser.GetError().value_or(FNotationErrorV2{0, "failed to parse relation"});
            return FNotationParseResultV2<FRelationV2>::Fail(error.Position, error.Message);
        }

        FRelationV2 relation;
        relation.RelationId = relationId;
        relation.Kind = kind;
        relation.Left = equation->first;
        relation.Right = equation->second;
        relation.SourceText = source;
        relation.Name = source;
        return FNotationParseResultV2<FRelationV2>::Ok(std::move(relation));
    }

    inline auto MakeDefinitionFromParsedNotationV2(const Str &definitionId,
                                                   const EDefinitionKindV2 kind,
                                                   const FParsedDefinitionNotationV2 &parsed,
                                                   Str displayName = {},
                                                   Str description = {},
                                                   Vector<Str> argumentDefinitionIds = {},
                                                   Vector<Str> dependencyDefinitionIds = {}) -> FDefinitionV2 {
        FDefinitionV2 definition;
        definition.DefinitionId = definitionId;
        definition.Kind = kind;
        definition.Symbol = parsed.CanonicalSymbol;
        definition.PreferredNotation = parsed.SourceSymbolText;
        definition.DisplayName = displayName.empty() ? parsed.CanonicalSymbol : std::move(displayName);
        definition.Description = std::move(description);
        definition.ArgumentDefinitionIds = std::move(argumentDefinitionIds);
        definition.DependencyDefinitionIds = std::move(dependencyDefinitionIds);
        definition.SourceText = parsed.SourceText;

        if (parsed.bMembership && parsed.MembershipSpace.has_value()) {
            definition.DeclaredType = MakeSpaceTypeV2(*parsed.MembershipSpace);
        } else if (parsed.DeclaredType.has_value()) {
            definition.DeclaredType = *parsed.DeclaredType;
        }

        return definition;
    }

    inline auto RenderDialectSpaceExprV2(const FSpaceExprV2 &space, const FModelV2 *model = nullptr) -> Str;

    inline auto RenderDialectExpressionV2(const FExpressionPtrV2 &expression,
                                          const FModelV2 *model = nullptr) -> Str {
        const auto renderRef = [model](const FReferenceV2 &reference) -> Str {
            if (model != nullptr && reference.IsBound()) {
                const auto resolved = ResolveSemanticEntryV2(*model, reference);
                if (resolved.IsResolved()) {
                    return resolved.GetDisplayLabel();
                }
            }
            return reference.DisplayText();
        };

        if (expression == nullptr) return "<null>";

        switch (expression->Kind) {
            case EExpressionKindV2::Symbol:
                return renderRef(expression->Reference);
            case EExpressionKindV2::Literal:
                return expression->LiteralText;
            case EExpressionKindV2::Unary:
                if (expression->Children.empty()) return Str(ToString(expression->UnaryOperator)) + "?";
                return Str(ToString(expression->UnaryOperator)) + RenderDialectExpressionV2(expression->Children.front(), model);
            case EExpressionKindV2::Binary: {
                if (expression->Children.size() < 2) return "?";
                const auto left = RenderDialectExpressionV2(expression->Children[0], model);
                const auto right = RenderDialectExpressionV2(expression->Children[1], model);
                if (expression->BinaryOperator == EBinaryOperatorV2::Multiply && expression->bImplicitApplication) {
                    return left + " " + right;
                }
                if (expression->BinaryOperator == EBinaryOperatorV2::Power) {
                    return left + "^{" + right + "}";
                }
                return left + " " + ToString(expression->BinaryOperator) + " " + right;
            }
            case EExpressionKindV2::FunctionApplication: {
                Slab::StringStream ss;
                ss << renderRef(expression->Reference) << "(";
                for (std::size_t i = 0; i < expression->Children.size(); ++i) {
                    if (i != 0) ss << ", ";
                    ss << RenderDialectExpressionV2(expression->Children[i], model);
                }
                ss << ")";
                return ss.str();
            }
            case EExpressionKindV2::OperatorApplication: {
                if (model != nullptr && expression->Reference.IsBound()) {
                    if (const auto *definition = FindDefinitionByIdV2(*model, expression->Reference.ReferenceId);
                        definition != nullptr &&
                        definition->OperatorStyle == EOperatorApplicationStyleV2::FunctionLike) {
                        Slab::StringStream ss;
                        ss << renderRef(expression->Reference) << "(";
                        for (std::size_t i = 0; i < expression->Children.size(); ++i) {
                            if (i != 0) ss << ", ";
                            ss << RenderDialectExpressionV2(expression->Children[i], model);
                        }
                        ss << ")";
                        return ss.str();
                    }
                }

                Slab::StringStream ss;
                ss << renderRef(expression->Reference);
                if (!expression->Children.empty()) ss << " ";
                for (std::size_t i = 0; i < expression->Children.size(); ++i) {
                    if (i != 0) ss << ", ";
                    ss << RenderDialectExpressionV2(expression->Children[i], model);
                }
                return ss.str();
            }
            case EExpressionKindV2::Derivative: {
                if (expression->Children.empty()) return "d(?)";
                const auto target = RenderDialectExpressionV2(expression->Children.front(), model);
                const auto order = expression->DerivativeVariables.size();
                const auto variable = expression->DerivativeVariables.empty()
                    ? Str("?")
                    : renderRef(expression->DerivativeVariables.front());

                if (expression->DerivativeFlavor == EDerivativeFlavorV2::Ordinary &&
                    order == 1 &&
                    expression->Children.front()->Kind == EExpressionKindV2::Symbol) {
                    return "\\dot " + target;
                }
                if (expression->DerivativeFlavor == EDerivativeFlavorV2::Ordinary &&
                    order == 2 &&
                    expression->Children.front()->Kind == EExpressionKindV2::Symbol) {
                    return "\\ddot " + target;
                }

                const auto numeratorPrefix = expression->DerivativeFlavor == EDerivativeFlavorV2::Partial ? "\\partial" : "d";
                const auto denominatorPrefix = expression->DerivativeFlavor == EDerivativeFlavorV2::Partial ? "\\partial" : "d";
                if (order == 1) {
                    return Str("\\frac{") + numeratorPrefix + " " + target + "}{" + denominatorPrefix + " " + variable + "}";
                }
                return Str("\\frac{") + numeratorPrefix + "^{" + ToStr(order) + "} " + target + "}{" +
                    denominatorPrefix + " " + variable + "^{" + ToStr(order) + "}}";
            }
        }

        return "?";
    }

    inline auto RenderDialectSpaceExprV2(const FSpaceExprV2 &space, const FModelV2 *model) -> Str {
        Str base;
        if (space.Kind == ESpaceKindV2::RealNumbers) base = "\\mathbb{R}";
        else if (space.Kind == ESpaceKindV2::ComplexNumbers) base = "\\mathbb{C}";
        else base = space.Name;

        if (!space.Dimension.has_value() || *space.Dimension == nullptr) return base;
        return base + "^{" + RenderDialectExpressionV2(*space.Dimension, model) + "}";
    }

    inline auto RenderDialectTypeExprV2(const FTypeExprV2 &type, const FModelV2 *model = nullptr) -> Str {
        if (type.Kind == ETypeExprKindV2::Space) return RenderDialectSpaceExprV2(type.Space, model);

        Slab::StringStream ss;
        for (std::size_t i = 0; i < type.Domain.size(); ++i) {
            if (i != 0) ss << " \\times ";
            ss << RenderDialectTypeExprV2(type.Domain[i], model);
        }
        ss << " \\to ";
        if (type.Codomain != nullptr) ss << RenderDialectTypeExprV2(*type.Codomain, model);
        else ss << "?";
        return ss.str();
    }

    inline auto RenderDialectDefinitionV2(const FDefinitionV2 &definition, const FModelV2 *model = nullptr) -> Str {
        const auto label = RenderDefinitionLabelV2(definition);
        if (!definition.DeclaredType.has_value()) return label;

        if (definition.Kind == EDefinitionKindV2::Coordinate || definition.Kind == EDefinitionKindV2::ScalarParameter) {
            return label + " \\in " + RenderDialectTypeExprV2(*definition.DeclaredType, model);
        }

        return label + " : " + RenderDialectTypeExprV2(*definition.DeclaredType, model);
    }

    inline auto RenderDialectRelationV2(const FRelationV2 &relation, const FModelV2 *model = nullptr) -> Str {
        return RenderDialectExpressionV2(relation.Left, model) + " = " +
            RenderDialectExpressionV2(relation.Right, model);
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_NOTATION_V2_H
