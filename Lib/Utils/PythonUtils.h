//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_PYTHONUTILS_H
#define STUDIOSLAB_PYTHONUTILS_H

#include "Types.h"
#include <map>

namespace PythonUtils {
    enum PyType {
        Integer,
        Float,
        Bool,
        String
    };

    typedef Str Key;
    typedef Str StringValue;
    typedef std::pair<StringValue, PyType> Value;
    typedef std::map<Key, Value> PyDict;

    Str PyTypeToString(PyType);

    bool BadPythonDictionary(const Str& pyDict);

    /**
     * Very light-weight not-solid parser for python dictionary strings;
     * Doesn't check much for errors and, in fact, is made to work on Python dictionaries as output
     * in the header of StudioSlab output files (.snapshot, .osc, etc.)
     * @param pyDict
     * @return
     */
    PyDict ParsePythonDict(const std::string& pyDict);

}

#endif //STUDIOSLAB_PYTHONUTILS_H
