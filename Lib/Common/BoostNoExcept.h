//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_BOOSTNOEXCEPT_H
#define STUDIOSLAB_BOOSTNOEXCEPT_H

#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
void boost::throw_exception(std::exception const & e){
    //do nothing
}

#endif //STUDIOSLAB_BOOSTNOEXCEPT_H
