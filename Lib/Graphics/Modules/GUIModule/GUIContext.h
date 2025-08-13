//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_GUICONTEXT_H
#define STUDIOSLAB_GUICONTEXT_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Slab::Graphics {

    using FDrawCall = std::function<void(void)>;

    using MainMenuAction = std::function<void(Str leaf_entry)>;
    using MainMenuLocation = Vector<Str>;
    struct MainMenuLeafEntry {
        Str label;
        Str shortcut;
        bool selected=false;
        bool enabled=true;
    };
    struct MainMenuItem {
        MainMenuLocation Location;
        Vector<MainMenuLeafEntry> SubItems;
        MainMenuAction Action=[](const Str&){};
    };

    class GUIContext : public FPlatformWindowEventListener {
    protected:
        Vector<FDrawCall> DrawCalls;

        void FlushDrawCalls();


    public:
        explicit GUIContext();

        virtual void Bind() = 0;
        virtual void NewFrame() = 0;
        virtual void Render() const = 0;

        void AddDrawCall(const FDrawCall&);

        virtual void AddMainMenuItem(MainMenuItem) = 0;

        virtual
        void* GetContextPointer() = 0;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUICONTEXT_H
