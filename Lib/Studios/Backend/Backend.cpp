#include "Backend.h"

Backend *Backend::myInstance = nullptr;

Backend::Backend(Backend *instance)
{
    if(Backend::myInstance != nullptr) throw "Backend already initialized";

    Backend::myInstance = instance;
}

Backend::~Backend() = default;

Backend *Backend::GetInstance()
{
    return Backend::myInstance;
}

void Backend::Destroy() {
    delete Backend::myInstance;
}
