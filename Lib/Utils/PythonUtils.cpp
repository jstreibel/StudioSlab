//
// Created by joao on 23/09/23.
//

#include "PythonUtils.h"

#include <iostream>
#include <string>
#include <stack>
#include <sstream>
#include <utility>

namespace PythonUtils {

    Str PyTypeToString(PyType type){
        switch (type) {
            case Integer:   return "Integer";
            case Float:     return "Float";
            case Bool:      return "Bool";
            case String:    return "String";
            default:        return "<unmapped>";
        }
    }

    bool BadPythonDictionary(const Str& pyDict) {
        std::stack<char> brackets;
        bool inQuotes = false;
        bool expectColon = false;
        bool expectComma = false;

        for (char c : pyDict) {
            if (c == ' ') continue;

            if (inQuotes) {
                if (c == '\'') {
                    inQuotes = false;
                    if (expectColon) expectColon = false;
                    else expectComma = true;
                }
                continue;
            }

            switch (c) {
                case '{':
                    if (expectColon || expectComma) return false;
                    brackets.push(c);
                    break;
                case '}':
                    if (brackets.empty() || brackets.top() != '{' || expectColon) return false;
                    brackets.pop();
                    break;
                case '\'':
                    inQuotes = true;
                    if (expectComma) expectComma = false;
                    break;
                case ':':
                    if (!expectColon || expectComma) return false;
                    expectColon = false;
                    break;
                case ',':
                    if (expectColon || !expectComma) return false;
                    expectComma = false;
                    expectColon = true;
                    break;
                default:
                    if (expectColon || expectComma) return false;
                    break;
            }
        }

        return brackets.empty() && !expectColon && !expectComma;
    }

    PyDict ParsePythonDict(const std::string& pyDict) {
        PyDict resultMap;
        Str key;
        StringValue value;
        bool isKey = true;

        std::stringstream ss(pyDict);
        char c;

        while (ss >> c) {
            if (c == '{' || c == '}' || c == ' ' || c == ',') {
                continue;
            } else if (c == ':') {
                isKey = false;
                continue;
            } else if (c == '\"' || c == '\'') {
                Str temp;
                std::getline(ss, temp, c); // find the matching (double-) quotation mark.

                if (isKey)
                    key = temp;
                else {
                    value = temp;
                    resultMap[key] = {value, PyType::String};
                    isKey = true;
                }
                continue;
            } else {
                Str temp;
                std::getline(ss, temp, ',');
                value = c + temp;

                PyType type;
                if (value.find('.') != std::string::npos)
                    type = PyType::Float;
                else if(value=="True" || value=="False")
                    type = PyType::Bool;
                else
                    type = PyType::Integer;

                resultMap[key] = {value, type};
                isKey = true;
            }
        }

        return resultMap;
    }
}
