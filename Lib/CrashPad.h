//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_SAFETYNET_H
#define V_SHAPE_SAFETYNET_H

#include "Application.h"

namespace Slab::SafetyNet {
    int jump(int (*callback)(int, const char**), int argc=0, const char *argv[]= nullptr);

    int Jump(Application& app);
}

#endif //V_SHAPE_SAFETYNET_H
