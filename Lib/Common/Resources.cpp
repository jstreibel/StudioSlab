//
// Created by joao on 01/06/23.
//

#include "Resources.h"

String Resources::fontFileName(int index) {
    return FontsFolder + fonts[index];
}
