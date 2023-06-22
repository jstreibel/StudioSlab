//
// Created by joao on 17/10/2019.
//

#ifndef V_SHAPE_COMMANDLINEGENERAL_H
#define V_SHAPE_COMMANDLINEGENERAL_H

#include "Common/Workaround/ABIDef.h"

#include <boost/program_options.hpp>

// TODO if stuff is working, the below two lines can be deleted.
//#include <Controller/Interface/ParameterUtils.h>
//#include "Controller/Interface/ParameterBase.h"

namespace CLOptions = boost::program_options;
typedef CLOptions::variables_map                 CLVariablesMap;
typedef CLOptions::options_description           CLOptionsDescription;
typedef CLOptions::options_description_easy_init CLODEasyInit;
typedef CLOptions::variable_value                VariableValue;

#define GET(key, type) (vm[key].as<type>())
#define GET_FALLBACK(key, type, default) (vm.count(key) ? GET(key, type) : default)
#define GET_FLAG(key) (vm.count(key) != 0)

#endif //V_SHAPE_COMMANDLINEGENERAL_H
