//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "Core/Controller/Parameter/Parameter.h"
#include "Core/Controller/Interface/Request.h"
#include "Core/Controller/Interface/Message.h"

#include <set>

namespace Slab::Core {

    class InterfaceManager;

    class CLInterfaceOwner;

    class CLInterfaceListener {
    public:
        /**
         * Notify listeners that this interface has finished being set up from command line.
         */
        virtual auto notifyCLArgsSetupFinished() -> void {};

        /**
         * Notify listeners that all interfaces have finished being set up from command line.
         */
        virtual auto notifyAllCLArgsSetupFinished() -> void {};

        virtual auto requestIssued(Request) -> Message {return {"[no answer]"};};
    };

    class CLInterface final {
        const int priority;

        friend InterfaceManager;
        friend CLInterfaceOwner;

        CLInterfaceOwner *owner = nullptr;

        Str name;
        Str descr = "<empty>";
        const Str delimiter = ",";

        Vector<CLInterfaceListener *> listeners;

        Vector<Request> protocols;
    private:
        std::set<Parameter_ptr> parameters;
        std::set<Pointer<CLInterface>> subInterfaces;

    public:
        CLInterface(Str name, CLInterfaceOwner *owner, int priority);

        /**
         * Instantiate a new interface. It won't be registered in the InterfaceManager, but that can
         * be done manually through InterfaceManager::registerInterface(...) method.
         * @param name The name of the interface being created. Use ',' as a separator to add a description to
         * the interface. For example name="Render options,Some rendering options for the user."
         * @param owner The owner of this interface. It can be nullptr, but its up to the user to deal with that.
         * @return an std::shared_ptr to an Interface.
         */

        ~CLInterface();

        Message sendRequest(Request);

        auto getGeneralDescription() const -> Str;

        void addSubInterface(const Pointer<CLInterface>& subInterface);

        auto addListener(CLInterfaceListener *) -> void;

        void addParameter(Parameter_ptr parameter);

        void addParameters(std::initializer_list<Parameter_ptr> parameters);

        void addParameters(std::initializer_list<Parameter *> parameters);

        auto getSubInterfaces() const -> Vector<Pointer<CLInterface>>;

        auto getParameters() const -> Vector<Parameter_constptr>;

        auto getParameter(Str key) const -> Parameter_ptr;

        auto getOwner() const -> CLInterfaceOwner *;

        auto getName() const -> const Str &;

        /**
         *
         * @param paramNames Parameters to turn into 'separator' separated string. All parameters if none is specified.
         * @param separator Speparator string between argumens.
         * @param longName Whether to use long parameter name, if available.
         * @return A formated string, with all parameters and their values.
         */
        auto toString(const StrVector &paramNames = {}, Str separator = " ", bool longName = true) const -> Str;

        void setupFromCommandLine(CLVariablesMap vm);

        bool operator==(const CLInterface &rhs) const;

        bool operator==(Str val) const;

        bool operator!=(const CLInterface &rhs) const;

        bool operator<(const CLInterface &other) const;
    };

    DefinePointers(CLInterface)

}

#endif //FIELDS_INTERFACE_H
