//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERUTILS_H
#define FIELDS_PARAMETERUTILS_H

#include "ParameterBase.h"

#define EXPOSE_TYPE(Type, Name) public: typedef Type Name;
#define EXPOSE_BASE(Type) public: typedef Type BaseType;

class Parameter;

/**
 * Mapa relacionando strings com tipos arbitrarios. */
//typedef std::map<String, Parameter*> UserParamMap;

#endif //FIELDS_PARAMETERUTILS_H
