//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_INTERFACE_H
#define STUDIOSLAB_INTERFACE_H

#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Utils/Sets.h"
#include "Utils/List.h"

namespace Slab::Core {

    class InterfaceManager;

    class Interface {
        friend InterfaceManager;

        Str name;
        Set<Pointer<CLParameter>> parameters;

    protected:
        void addParameter(Pointer<CLParameter> parameter);
        void addParameters(BasicList<Pointer<CLParameter>> parameters);

    public:
        Interface(Str name);
        ~Interface();

        virtual
        Message sendRequest(Request);

        auto getParameters() const -> Vector<Pointer<const CLParameter>>;
        auto getParameter(Str key) const -> Pointer<CLParameter>;

    };

} // Slab::Core

#endif //STUDIOSLAB_INTERFACE_H
