//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_SAFETYNET_H
#define V_SHAPE_SAFETYNET_H

#include "AppBase.h"


namespace Slab::Core::SafetyNet {
    int jump(int (*callback)(int, const char**), int argc=0, const char *argv[]= nullptr);
}

#endif //V_SHAPE_SAFETYNET_H
