//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "Parameter.h"

#include <set>

class InterfaceManager;

class Interface {
public:
    typedef std::shared_ptr<Interface> Ptr;
    typedef std::weak_ptr<Interface> WeakPtr;
    typedef std::shared_ptr<const Interface> ConstPtr;

protected:
    WeakPtr me; // This guy is the way to self-registry.

    friend InterfaceManager;
    String description;


    std::set<Parameter::Ptr> parameters;
    void addParameter(Parameter::Ptr parameter);
    void addParameters(std::initializer_list<Parameter::Ptr> parameters);

    std::set<Ptr> subInterfaces;
    void addSubInterface(Ptr subInterface);

public:

    Interface(const String& generalDescription, bool dontRegisterInterface=false);
    ~Interface();

    auto getGeneralDescription() const -> String;

    auto getParameters() const -> std::vector<Parameter::ConstPtr>;
    auto getParameter(String key) const -> Parameter::Ptr;

    auto getSubInterfaces() const -> std::vector<Interface::Ptr>;

    virtual auto toString() const -> String;
    virtual void setup(CLVariablesMap vm);

    virtual bool operator==(const Interface &rhs) const;
    virtual bool operator==(String val) const;


    virtual bool operator!=(const Interface &rhs) const;


};


#endif //FIELDS_INTERFACE_H
