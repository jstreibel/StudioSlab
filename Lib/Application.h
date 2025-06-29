//
// Created by joao on 11/1/24.
//

#ifndef STUDIOSLAB_APPLICATION_H
#define STUDIOSLAB_APPLICATION_H

#include "Utils/String.h"
#include "Utils/Numbers.h"
#include "Graphics/Backend/GraphicBackend.h"

namespace Slab {

    using Platform = Graphics::GraphicBackend;

    class FApplication : public Graphics::FSystemWindowEventListener {
        const int arg_count;
        const char **arg_values;

        Str m_Name;
        Pointer<Platform> p_Platform;

    protected:
        virtual
        Pointer<Platform> CreatePlatform();

        virtual void OnStart() { };
        virtual bool CanClose() { return true; };

    public:
        FApplication(Str name, const int argc, const char *argv[]);
        ~FApplication() override;

        bool Create(Resolution width=UndefinedResolution, Resolution height=UndefinedResolution);
        Int Run();

        void SetTitle(Str title) const;

        auto GetName() const -> Str;

    private:


    };

} // Slab

#endif //STUDIOSLAB_APPLICATION_H
