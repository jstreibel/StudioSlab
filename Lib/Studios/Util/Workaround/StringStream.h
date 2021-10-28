#ifndef MY_STRING_STREAM
#define MY_STRING_STREAM

//
// Created by joao on 01/10/2019.
//

#include "Studios/Util/STDLibInclude.h"

class StringStream{
public:

    template<class T>
    std::ostringstream&operator << (const T& obj) {
        oStream << obj;

        return this->oStream;
    }

    void str(const std::string string){
        oStream.str(string);
    }

    std::string str(){
        return oStream.str();
    }

    std::ostringstream oStream;
};

#endif