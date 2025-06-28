//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_INTERFACE_H
#define STUDIOSLAB_INTERFACE_H

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

        [[nodiscard]] auto GetParameters() const -> Vector<Pointer<const FCommandLineParameter>>;
        [[nodiscard]] auto GetParameter(const Str& Key) const -> Pointer<FCommandLineParameter>;

    protected:
        void AddParameter(Pointer<FCommandLineParameter> Parameter);
        void AddParameters(BasicList<Pointer<FCommandLineParameter>> parameters);

    private:
        friend FInterfaceManager;
        const UniqueID Id;
        Str Name;
        Set<Pointer<FCommandLineParameter>> Parameters;

    };

} // Slab::Core

#endif //STUDIOSLAB_INTERFACE_H
