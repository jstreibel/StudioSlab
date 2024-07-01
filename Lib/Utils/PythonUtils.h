//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_PYTHONUTILS_H
#define STUDIOSLAB_PYTHONUTILS_H

#include "Types.h"
#include <map>
#include <variant>

namespace Slab::PythonUtils {

    enum PyType {
        Integer,
        Float,
        Bool,
        String
    };

    typedef Str Key;
    typedef Str StringValue;
    typedef Pair<StringValue, PyType> Value;
    typedef std::map<Key, Value> PyDict;

    class PyDictException : public Exception {
    public:
        explicit PyDictException(const Str &msg="no details provided");
    };

    Str PyTypeToString(PyType);

    bool BadPythonDictionary(const Str& pyDict);

    using ReturnType = std::variant<long, float, double, bool, Str>;

    template<typename T>
    ReturnType Get(Key key, const PyDict &from) {
        if(!Contains(from, key)) throw PyDictException(Str("Dictionary does not contain key '") + key + "'.");

        auto value = from.at(key);
        char *endPtr;

        if constexpr (std::is_same_v<T, int>){
            if(value.second != Integer)
                throw PyDictException(Str("Error trying to convert access value '") + value.first + "' (key '" + key + "') to type Integer." );

            return strtol(value.first.c_str(), &endPtr, 10);
        }

        if constexpr (std::is_same_v<T, double>){
            if(value.second != Float)
                throw PyDictException(Str("Error trying to convert access value '") + value.first + "' (key '" + key + "') to type Float." );

            return strtod(value.first.c_str(), &endPtr);
        }

        if constexpr (std::is_same_v<T, float>){
            if(value.second != Float)
                throw PyDictException(Str("Error trying to convert access value '") + value.first + "' (key '" + key + "') to type Float." );

            return strtof(value.first.c_str(), &endPtr);
        }

        if constexpr (std::is_same_v<T, bool>){
            if(value.second != Bool)
                throw PyDictException(Str("Error trying to convert access value '") + value.first + "' (key '" + key + "') to type Bool." );

            NOT_IMPLEMENTED
        }

        if constexpr (std::is_same_v<T, Str>){
            if(value.second != String)
                throw PyDictException(Str("Error trying to convert access value '") + value.first + "' (key '" + key + "') to type String." );

            return value.first;
        }
    }

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
