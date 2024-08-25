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
    int utf8_characterByteSize(const char *character);

    const Str &mapToSuperscript(char c);

    Str &elegantScientific(Str &stringScientificValue);

}

#endif //STUDIOSLAB_ENCODINGUTILS_H
