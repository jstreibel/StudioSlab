//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_ENCODINGUTILS_H
#define STUDIOSLAB_ENCODINGUTILS_H

#include "Types.h"
#include <map>

namespace Slab {

    extern std::map<Str, Str> char_map;

    extern const char *glyphsToLoad;

/**
 * Calculates the utf8 size (in bytes) of a character
 * @param character the charater to compute the size
 * @return the character size, in bytes. Returns 0 (zero)
 * if character is a sequence character, and negative value
 * if unknown.
 */
    int Utf8CharacterByteSize(const char *character);
    [[deprecated("Use Utf8CharacterByteSize")]]
    inline int utf8_characterByteSize(const char *character) {
        return Utf8CharacterByteSize(character);
    }

    const Str &MapToSuperscript(char c);
    [[deprecated("Use MapToSuperscript")]]
    inline const Str &mapToSuperscript(char c) {
        return MapToSuperscript(c);
    }

    Str &ElegantScientific(Str &stringScientificValue);
    [[deprecated("Use ElegantScientific")]]
    inline Str &elegantScientific(Str &stringScientificValue) {
        return ElegantScientific(stringScientificValue);
    }

}

#endif //STUDIOSLAB_ENCODINGUTILS_H
