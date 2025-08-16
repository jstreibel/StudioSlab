//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "CommandLineParameter.h"
#include "Core/Controller/Interface/Payload.h"
#include "Core/Controller/Interface/Message.h"
#include "Utils/List.h"

#include <set>

namespace Slab::Math::Base
{
    class FNumericalRecipe;
}

namespace Slab::Core {

    class FCommandLineInterfaceManager;

    class FCommandLineInterfaceOwner;

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

    class FCommandLineInterface final {
    public:
        /**
         * Instantiate a new interface. It won't be registered in the InterfaceManager, but that can
         * be done manually through InterfaceManager::registerInterface(...) method.
         * @param Name The name of the interface being created. Use ',' as a separator to add a description to
         * the interface. For example name="Render options,Some rendering options for the user."
         * @param pOwner The owner of this interface. It can be nullptr, but its up to the user to deal with that.
         * @return an std::shared_ptr to an Interface.
         */
        FCommandLineInterface(const Str& Name, FCommandLineInterfaceOwner *pOwner, int Priority);

        ~FCommandLineInterface();

        void SendRequest(const FPayload&) const;

        [[nodiscard]] auto GetGeneralDescription() const -> Str;

        void AddSubInterface(const TPointer<FCommandLineInterface>& subInterface);

        auto AddListener(FCommandLineInterfaceListener *) -> void;

        void AddParameter(const TPointer<FCommandLineParameter>& parameter);

        void AddParameters(const TList<TPointer<FCommandLineParameter>>& parameters);

        void AddParameters(const TList<FCommandLineParameter *>& parameters);

        [[nodiscard]] auto GetSubInterfaces() const -> Vector<TPointer<FCommandLineInterface>>;

        [[nodiscard]] auto GetParameters() const -> Vector<TPointer<const FCommandLineParameter>>;

        [[nodiscard]] auto GetParameter(const Str& key) const -> TPointer<FCommandLineParameter>;

        [[nodiscard]] auto GetOwner() const -> FCommandLineInterfaceOwner *;

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

        bool operator==(const FCommandLineInterface &rhs) const;

        bool operator==(Str Value) const;

        bool operator!=(const FCommandLineInterface &RHS) const;

        bool operator<(const FCommandLineInterface &RHS) const;

    private:
        friend class Math::Base::FNumericalRecipe;

        void SetGeneralDescription(const Str& str);

        const int Priority;

        friend FCommandLineInterfaceManager;
        friend FCommandLineInterfaceOwner;

        // TODO: make pOwner a weak pointer
        FCommandLineInterfaceOwner *pOwner = nullptr;

        Str Name;
        Str Description = "<empty>";
        const Str Delimiter = ",";

        Vector<FCommandLineInterfaceListener *> Listeners;

        Vector<FPayload> Protocols;

        std::set<TPointer<FCommandLineParameter>> Parameters;
        std::set<TPointer<FCommandLineInterface>> SubInterfaces;
    };

    DefinePointers(FCommandLineInterface)

}

#endif //FIELDS_INTERFACE_H
