//
// Created by joao on 17/10/2019.
//

#ifndef V_SHAPE_STDLIBINCLUDE_H
#define V_SHAPE_STDLIBINCLUDE_H

#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <ctime>

#include <iomanip>

#include <ostream>
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>
#include <map>
#include <set>

#include <string>

#include <limits>
#include <algorithm>
#include <utility>
#include <variant>

//#include <complex>

// abaixo uma tentativa que falhou e fica ai para posteridade.
#ifdef REMIND_ME_OF_AN_INTERSTING_FAILED_ATTEMPT_THAT_SHOULD_REMAIN_IN_MEMORY
// Se a linguagem for C++17, usamos stdlib. Se nao, usamos boost.
#if __cplusplus == 201703L // ver https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler
#include <variant>
typedef std::map<std::string, std::variant<std::string, int, Real>> UserParamMap;
#else
#include <boost/variant.hpp>
typedef std::map<std::string, boost::variant<std::string, int, Real>> UserParamMap;
#endif
#endif


#endif //V_SHAPE_STDLIBINCLUDE_H
