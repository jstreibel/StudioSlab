//
// Created by joao on 7/09/23.
//

#include "EncodingUtils.h"
#include "Types.h"

#include <map>

namespace Slab {

    const char *glyphsToLoad = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"
                               "a b c d e f g h i j k l m n o p q r s t u v w x y z"
                               "𝔞 𝔟 𝔠 𝔡 𝔢 𝔣 𝔤 𝔥 𝔦 𝔧 𝔨 𝔩 𝔪 𝔫 𝔬 𝔭 𝔮 𝔯 𝔰 𝔱 𝔲 𝔳 𝔴 𝔵 𝔶 𝔷"
                               "𝖆 𝖇 𝖈 𝖉 𝖊 𝖋 𝖌 𝖍 𝖎 𝖏 𝖐 𝖑 𝖒 𝖓 𝖔 𝖕 𝖖 𝖗 𝖘 𝖙 𝖚 𝖛 𝖜 𝖝 𝖞 𝖟"
                               "ᵃ ᵇ ᶜ ᵈ ᵉ ᶠ ᵍ ʰ ⁱ ʲ ᵏ ˡ ᵐ ⁿ ᵒ ᵖ   ʳ ˢ ᵗ ᵘ ᵛ ʷ ˣ ʸ ᶻ "
                               "ₐ       ₑ     ₕ ᵢ ⱼ ₖ ₗ ₘ ₙ ₒ ₚ   ᵣ ₛ ₜ ᵤ ᵥ   ₓ     "
                               "𝒜ℬ𝒞 𝒟 ℰℱ    ℐ                ℛ                 "
                               "𝕬𝕭𝕮 𝕯𝕰 𝕱𝕲𝕳 𝕴𝕵 𝕶𝕷 𝕸𝕹 𝕺𝕻𝕼𝕽𝕾 𝕿𝖀  𝖁𝖂𝖃 𝖄 𝖅 "
                               "𝔄𝔅ℭ 𝔇𝔈 𝔉𝔊 ℌ ℑ𝔍 𝔎𝔏 𝔐𝔑 𝔒𝔓𝔔ℜ𝔖 𝔗𝔘 𝔙𝔚𝔛 𝔜 ℨ "
                               "              ℑ             ℜ "
                               "        ℯ               𝓂                           "
                               "¹ ² ³ ⁴ ⁵ ⁶ ⁷ ⁸ ⁹ ⁰ "
                               "₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉ ₀ "

                               "Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ Σ Τ Υ Φ Χ Ψ Ω "
                               "α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω "
                               "      𝜕 ϵ     ϑ   ϰ           ϖ ϱ ς     ϕ       "
                               "1234567890'\"!@#$%¨&*()_+-=`'{[^~}]\\|<,>.:;?/"
                               "○ ¼ ½ ⁄"
                               "×⁻⁺ ∫";

        std::map<Str, Str> char_map = {
            {"^a", "ᵃ"},
            {"^b", "ᵇ"},
            {"^c", "ᶜ"},
            {"^d", "ᵈ"},
            {"^e", "ᵉ"},
            {"^f", "ᶠ"},
            {"^g", "ᵍ"},
            {"^h", "ʰ"},
            {"^i", "ⁱ"},
            {"^j", "ʲ"},
            {"^k", "ᵏ"},
            {"^l", "ˡ"},
            {"^m", "ᵐ"},
            {"^n", "ⁿ"},
            {"^o", "ᵒ"},
            {"^p", "ᵖ"},
            {"^q", "⬜"},
            {"^r", "ʳ"},
            {"^s", "ˢ"},
            {"^t", "ᵗ"},
            {"^u", "ᵘ"},
            {"^v", "ᵛ"},
            {"^w", "ʷ"},
            {"^x", "ˣ"},
            {"^y", "ʸ"},
            {"^z", "ᶻ"},

            {"_a", "ₐ"},
            {"_b", " "},
            {"_c", " "},
            {"_d", " "},
            {"_e", "ₑ"},
            {"_f", " "},
            {"_g", " "},
            {"_h", "ₕ"},
            {"_i", "ᵢ"},
            {"_j", "ⱼ"},
            {"_k", "ₖ"},
            {"_l", "ₗ"},
            {"_m", "ₘ"},
            {"_n", "ₙ"},
            {"_o", "ₒ"},
            {"_p", "ₚ"},
            {"_q", " "},
            {"_r", "ᵣ"},
            {"_s", "ₛ"},
            {"_t", "ₜ"},
            {"_u", "ᵤ"},
            {"_v", "ᵥ"},
            {"_w", " "},
            {"_x", "ₓ"},
            {"_y", " "},
            {"_z", " "}
        };

/**
 * Calculates the utf8 size (in bytes) of a character
 * @param character the charater to compute the size
 * @return the character size, in bytes. Returns 0 (zero)
 * if character is a sequence character, and negative value
 * if unknown.
 */
    int Utf8CharacterByteSize(const char *character) {
        auto ch = character[0];

        const unsigned char codes[] = {0b11000000, 0b10000000,  // Sequence char
                                       0b10000000, 0b00000000,  // 1 byte
                                       0b11100000, 0b11000000,  // 2 bytes
                                       0b11110000, 0b11100000,  // 3 bytes
                                       0b11111000, 0b11110000,  // 4 bytes
                                       0b11111100, 0b11111000}; // 5 bytes

        for (int i = 0; i < 6; ++i) {
            fix c1 = codes[2 * i];
            fix c2 = codes[2 * i + 1];
            if ((ch & c1) == c2)
                return i;
        }

        return -1;
    }

    std::map<const char, const Str> map = {
            {'+', "⁺"},
            {'-', "⁻"},
            {'0', "⁰"},
            {'1', "¹"},
            {'2', "²"},
            {'3', "³"},
            {'4', "⁴"},
            {'5', "⁵"},
            {'6', "⁶"},
            {'7', "⁷"},
            {'8', "⁸"},
            {'9', "⁹"},
    };

    const Str &MapToSuperscript(char c) {
        // auto newChar = Str("⁺ ⁻  ⁰¹²³⁴⁵⁶⁷⁸⁹").substr(c-'+', 1);

        // FLog::Info() << "Mapping '" << c << "' (" << int(c) << ") ↦ " << int(c-'+') << ": " << newChar << FLog::Flush;

        return map[c];
    }

    Str &ElegantScientific(Str &stringScientificValue) {
        Str &original = stringScientificValue;

        fix pos = original.find("e");

        if (pos != std::string::npos) {
            std::string newString = original.substr(0, pos + 1);  // Copy up to and including 'e'
            // Start from the character after 'e'
            for (std::size_t i = pos + 1; i < original.size(); ++i) {

                newString += MapToSuperscript(original[i]);
            }
            original = newString;

            original.replace(pos, 1, "×10");
        }

        return stringScientificValue;
    }


}
