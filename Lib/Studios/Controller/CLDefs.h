//
// Created by joao on 17/10/2019.
//

#ifndef V_SHAPE_COMMANDLINEGENERAL_H
#define V_SHAPE_COMMANDLINEGENERAL_H

#include <boost/program_options.hpp>

// TODO if stuff is working, the below two lines can be deleted.
//#include <Controller/Interface/ParameterUtils.h>
//#include "Controller/Interface/ParameterBase.h"

namespace po = boost::program_options;
typedef po::variables_map CLVariablesMap;
typedef po::options_description CLOptionsDescription;
typedef po::options_description_easy_init CLODEasyInit;
typedef po::variable_value VariableValue;

#define GET(key, type) (vm[key].as<type>())
#define GET_FALLBACK(key, type, default) (vm.count(key) ? GET(key, type) : default)
#define GET_FLAG(key) (vm.count(key) != 0)

#endif //V_SHAPE_COMMANDLINEGENERAL_H
