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

    class Application : public Graphics::SystemWindowEventListener {
        Str m_Name;
        Pointer<Platform> m_Platform;

    protected:
        virtual
        Pointer<Platform> CreatePlatform() = 0;

        virtual void OnStart() { };
        virtual bool CanClose() { return true; };

    public:
        Application(Str name, int arcg, char *argv[]);
        ~Application() override;

        bool Create(Resolution width=UndefinedResolution, Resolution height=UndefinedResolution);
        Int Run();

        void SetTitle(Str title);

        auto GetName() const -> Str;

    private:


    };

} // Slab

#endif //STUDIOSLAB_APPLICATION_H
