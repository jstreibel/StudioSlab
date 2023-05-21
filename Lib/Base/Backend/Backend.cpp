#include "Backend.h"

Backend *Backend::myInstance = nullptr;

Backend::Backend(Backend *instance, String name) : backendName(name)
{
    if(Backend::myInstance != nullptr) throw "Backend already initialized";

    Backend::myInstance = instance;
}

Backend::~Backend() = default;

Backend *Backend::GetInstance()
{
    if(Backend::myInstance == nullptr) throw "Backend not initialized";

    return Backend::myInstance;
}

void Backend::Destroy() {
    delete Backend::myInstance;
}
