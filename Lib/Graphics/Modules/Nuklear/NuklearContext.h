//
// Created by joao on 11/18/24.
//

#ifndef STUDIOSLAB_NUKLEARCONTEXT_H
#define STUDIOSLAB_NUKLEARCONTEXT_H

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "3rdParty/NuklearInclude.h"

namespace Slab::Graphics {

    class NuklearContext : public GUIContext {
        nk_context *context;
    public:
        void Bind() override;

        void NewFrame() override;

        void Render() const override;

        void *GetContextPointer() override;

        void AddMainMenuItem(MainMenuItem item) override;

    protected:
        void SetParentPlatformWindow(FOwnerPlatformWindow) override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_NUKLEARCONTEXT_H
