//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_SYSTEMWINDOW_H
#define STUDIOSLAB_SYSTEMWINDOW_H

#include "Utils/Numbers.h"
#include "Utils/String.h"
#include "Utils/Arrays.h"

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Backend/Events/MouseState.h"
#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"
#include "Graphics/Modules/GUIModule/GUIContext.h"

namespace Slab::Graphics {

    using RawPlatformWindow_Ptr = void*;

    class FPlatformWindow {
        friend class GraphicBackend;

        Vector<TPointer<FPlatformWindowEventListener>> Stash;
        TPointer<GUIContext> GuiContext = nullptr;

        TVolatile<FPlatformWindow> SelfReference;

    protected:
        virtual bool ProvideSelfReference(const TVolatile<FPlatformWindow>&);

        TPointer<FEventTranslator> EventTranslator;
        RawPlatformWindow_Ptr r_Window;

        virtual void ClearListeners();

        TPointer<FMouseState> MouseState = nullptr;

        virtual void Cycle() = 0;
    public:
        FPlatformWindow() = delete;
        explicit FPlatformWindow(void *window_ptr, TPointer<FEventTranslator>);

        virtual ~FPlatformWindow() = default;

        void Render();

        [[nodiscard]] virtual Int GetHeight() const = 0;
        [[nodiscard]] virtual Int GetWidth() const = 0;

        virtual void SignalClose() = 0;
        [[nodiscard]] virtual bool ShouldClose() const = 0;

        TPointer<GUIContext> GetGUIContext();

        RawPlatformWindow_Ptr GetRawPlatformWindowPointer() const;

        auto AddEventListener(const TVolatile<FPlatformWindowEventListener> &Listener) const -> bool;
        auto AddAndOwnEventListener(const TPointer<FPlatformWindowEventListener> &Listener) -> bool;

        [[nodiscard]] auto GetMouseState() const -> TPointer<const FMouseState>;
        virtual void SetMouseCursor(FMouseCursor);

        virtual
        void SetSystemWindowTitle(const Str& title);
    };

} // Slab::Core

#endif //STUDIOSLAB_SYSTEMWINDOW_H
