//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "Parameter.h"

#include <set>

class InterfaceManager;
class InterfaceOwner;
class InterfaceListener {
public:
    /**
     * Notify listeners that this interface has finished being set up from command line.
     */
    virtual auto notifyCLArgsSetupFinished()-> void { };

    /**
     * Notify listeners that all interfaces have finished being set up from command line.
     */
    virtual auto notifyAllCLArgsSetupFinished()-> void { };
};

class Interface final {
    const int priority;

    friend InterfaceManager;
    friend InterfaceOwner;

    InterfaceOwner *owner = nullptr;

    Str name;
    Str descr="<empty>";
    const Str delimiter=",";

    std::vector<InterfaceListener*> listeners;

public:
    typedef std::shared_ptr<Interface> Ptr;
    typedef std::shared_ptr<const Interface> ConstPtr;

private:
    std::set<Parameter::Ptr> parameters;
    std::set<Ptr> subInterfaces;

public:
    Interface(Str name, InterfaceOwner *owner, int priority);

    /**
     * Instantiate a new interface. It won't be registered in the InterfaceManager, but that can
     * be done manually through InterfaceManager::registerInterface(...) method.
     * @param name The name of the interface being created. Use ',' as a separator to add a description to
     * the interface. For example name="Render options,Some rendering options for the user."
     * @param owner The owner of this interface. It can be nullptr, but its up to the user to deal with that.
     * @return an std::shared_ptr to an Interface.
     */
    static Ptr New(Str name, InterfaceOwner *owner, int priority);

    ~Interface();

    auto getGeneralDescription() const -> Str;

    void addSubInterface(Ptr subInterface);
    auto addListener(InterfaceListener*) -> void;
    void addParameter(Parameter::Ptr parameter);
    void addParameters(std::initializer_list<Parameter::Ptr> parameters);
    void addParameters(std::initializer_list<Parameter*> parameters);

    auto getSubInterfaces() const -> std::vector<Interface::Ptr>;
    auto getParameters() const -> std::vector<Parameter::ConstPtr>;
    auto getParameter(Str key) const -> Parameter::Ptr;
    auto getOwner() const -> InterfaceOwner*;

    auto getName() const -> const Str&;
    /**
     *
     * @param paramNames Parameters to turn into 'separator' separated string. All parameters if none is specified.
     * @param separator Speparator string between argumens.
     * @param longName Whether to use long parameter name, if available.
     * @return A formated string, with all parameters and their values.
     */
    auto toString(const StrVector& paramNames={}, Str separator=" ", bool longName=true) const -> Str;
    void setup(CLVariablesMap vm);

    bool operator==(const Interface &rhs) const;
    bool operator==(Str val) const;
    bool operator!=(const Interface &rhs) const;
    bool operator< (const Interface& other) const;
};


#endif //FIELDS_INTERFACE_H
