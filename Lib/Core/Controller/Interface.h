//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "Parameter.h"
#include "Core/Controller/Payload.h"
#include "Message.h"
#include "Utils/List.h"

#include <set>

#include "Utils/Sets.h"

namespace Slab::Math::Base
{
    class FNumericalRecipe;
}

namespace Slab::Core {

    class FInterfaceManager;

    class FInterfaceOwner;

    class FCommandLineInterfaceListener {
    public:
        virtual ~FCommandLineInterfaceListener() = default;

        /**
         * Notify listeners that this interface has finished being set up from command line.
         */
        // virtual auto NotifyCLArgsSetupFinished() -> void {};

        /**
         * Notify listeners that all interfaces have finished being set up from command line.
         */
        // virtual auto NotifyAllCLArgsSetupFinished() -> void {};

        virtual auto SendMessage(FPayload) -> void { };
    };

    class FInterface final {
    public:
        /**
         * Instantiate a new interface. It won't be registered in the InterfaceManager, but that can
         * be done manually through InterfaceManager::registerInterface(...) method.
         * @param Name The name of the interface being created. Use ',' as a separator to add a description to
         * the interface. For example name="Render options,Some rendering options for the user."
         * @param pOwner The owner of this interface. It can be nullptr, but its up to the user to deal with that.
         * @return an std::shared_ptr to an Interface.
         */
        FInterface(const Str& Name, FInterfaceOwner *pOwner, int Priority);

        ~FInterface();

        void SendRequest(const FPayload&) const;

        [[nodiscard]] auto GetGeneralDescription() const -> Str;

        void AddSubInterface(const TPointer<FInterface>& subInterface);

        auto AddListener(FCommandLineInterfaceListener *) -> void;

        void AddParameter(const TPointer<FParameter>& parameter);

        void AddParameters(const TList<TPointer<FParameter>>& parameters);

        void AddParameters(const TList<FParameter *>& parameters);

        [[nodiscard]] auto GetSubInterfaces() const -> Vector<TPointer<FInterface>>;

        [[nodiscard]] auto GetParameters() const -> Vector<TPointer<const FParameter>>;

        [[nodiscard]] auto GetParameters() -> Set<TPointer<FParameter>>;

        [[nodiscard]] auto GetParameter(const Str& key) const -> TPointer<FParameter>;

        [[nodiscard]] auto GetOwner() const -> FInterfaceOwner *;

        [[nodiscard]] auto GetName() const -> const Str &;

        /**
         *
         * @param ParamNames Parameters to turn into 'separator' separated string. All parameters if none is specified.
         * @param Separator Speparator string between argumens.
         * @param LongName Whether to use long parameter name, if available.
         * @return A formated string, with all parameters and their values.
         */
        [[nodiscard]] auto ToString(const StrVector &ParamNames = {}, const Str& Separator = " ", bool LongName = true) const -> Str;

        void SetupFromCommandLine(CLVariablesMap vm);

        bool operator==(const FInterface &rhs) const;

        bool operator==(Str Value) const;

        bool operator!=(const FInterface &RHS) const;

        bool operator<(const FInterface &RHS) const;

    private:
        friend class Math::Base::FNumericalRecipe;

        void SetGeneralDescription(const Str& str);

        const int Priority;

        friend FInterfaceManager;
        friend FInterfaceOwner;

        // TODO: make pOwner a weak pointer
        FInterfaceOwner *pOwner = nullptr;

        Str Name;
        Str Description = "<empty>";
        const Str Delimiter = ",";

        Vector<FCommandLineInterfaceListener *> Listeners;

        Vector<FPayload> Protocols;

        std::set<TPointer<FParameter>> Parameters;
        std::set<TPointer<FInterface>> SubInterfaces;
    };

    DefinePointers(FInterface)

}

#endif //FIELDS_INTERFACE_H
