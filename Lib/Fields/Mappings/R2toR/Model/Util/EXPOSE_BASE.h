//
// Created by joao on 10/22/21.
//

#ifndef STUDIOSLAB_EXPOSE_BASE_H
#define STUDIOSLAB_EXPOSE_BASE_H

#define EXPOSE_TYPE(Type, Name) public: typedef Type Name;
#define EXPOSE_BASE(Type) public: typedef Type BaseType;

#endif //STUDIOSLAB_EXPOSE_BASE_H
