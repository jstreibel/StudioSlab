//
// Created by joao on 10/18/24.
//

#ifndef STUDIOSLAB_DATAALLOCATOR_H
#define STUDIOSLAB_DATAALLOCATOR_H

#include "Utils/Pointer.h"
#include "DataManager.h"

namespace Slab::Math {

    template<typename T, typename... Args>
    Pointer<T> DataAlloc(const DataName &name, Args&&... args) {
        auto data = New<T>(std::forward<Args>(args)...);

        DataManager::RegisterData(name, data);

        return data;
    }

}

#endif //STUDIOSLAB_DATAALLOCATOR_H
