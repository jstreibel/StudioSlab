//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "Parameter.h"

#include <set>

class InterfaceManager;
class InterfaceOwner;
class InterfaceListener { public: virtual auto notifyCLArgsSetupFinished()-> void = 0; };

class Interface final {
    const int priority;

public:
    typedef std::shared_ptr<Interface> Ptr;
    typedef std::shared_ptr<const Interface> ConstPtr;

private:
    Interface(const String& generalDescription, InterfaceOwner *owner, int priority);

    friend InterfaceManager;
    friend InterfaceOwner;

    InterfaceOwner *owner = nullptr;

    String name;
    String descr="<empty>";
    const String delimiter=",";

    std::vector<InterfaceListener*> listeners;

    std::set<Parameter::Ptr> parameters;
    std::set<Ptr> subInterfaces;

public:
    /**
     * Instantiate a new interface. It won't be registered in the InterfaceManager, but that can
     * be done manually through InterfaceManager::registerInterface(...) method.
     * @param name The name of the interface being created. Use ',' as a separator to add a description to
     * the interface. For example name="Render options,Some rendering options for the user."
     * @param owner The owner of this interface. It can be nullptr, but its up to the user to deal with that.
     * @return an std::shared_ptr to an Interface.
     */
    static Ptr New(String name, InterfaceOwner *owner, int priority);

    ~Interface();

    auto getGeneralDescription() const -> String;

    void addSubInterface(Ptr subInterface);
    auto addListener(InterfaceListener*) -> void;
    void addParameter(Parameter::Ptr parameter);
    void addParameters(std::initializer_list<Parameter::Ptr> parameters);

    auto getSubInterfaces() const -> std::vector<Interface::Ptr>;
    auto getParameters() const -> std::vector<Parameter::ConstPtr>;
    auto getParameter(String key) const -> Parameter::Ptr;
    auto getOwner() const -> InterfaceOwner*;

    auto getName() const -> const String&;
    auto toString() const -> String;
    void setup(CLVariablesMap vm);

    bool operator==(const Interface &rhs) const;
    bool operator==(String val) const;
    bool operator!=(const Interface &rhs) const;
    bool operator< (const Interface& other) const;


};


#endif //FIELDS_INTERFACE_H
