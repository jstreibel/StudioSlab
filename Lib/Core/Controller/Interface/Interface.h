//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_INTERFACE_H
#define STUDIOSLAB_INTERFACE_H

#include "Parameter.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "Utils/Sets.h"
#include "Utils/List.h"

namespace Slab::Core {

    class FInterfaceManager;

    using UniqueID = UInt;

    class FInterface {
    public:
        explicit FInterface(const Str& Name);
        virtual ~FInterface();

        [[nodiscard]] UniqueID GetUniqueID() const;

        virtual
        FMessage SendRequest(FRequest);

        [[nodiscard]] auto GetParameters() const -> Vector<Pointer<const FParameter>>;
        [[nodiscard]] auto GetParameter(const Str& Key) const -> Pointer<FParameter>;

    protected:
        void AddParameter(const Pointer<FParameter>& Parameter);
        void AddParameters(BasicList<Pointer<FParameter>> parameters);

    private:
        friend FInterfaceManager;
        const UniqueID Id;
        Str Name;
        Set<Pointer<FParameter>> Parameters;

    };

} // Slab::Core

#endif //STUDIOSLAB_INTERFACE_H
