

def ListProperties(SomeClass, classInstance):
    TheClass = SomeClass
    property_names=[p for p in dir(TheClass) if isinstance(getattr(TheClass, p),property)]

    print(TheClass)
    for prop in property_names:
        print(prop, getattr(classInstance, prop))

    print('Ok')
