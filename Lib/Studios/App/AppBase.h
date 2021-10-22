//
// Created by joao on 29/08/2021.
//

#ifndef V_SHAPE_APPBASE_H
#define V_SHAPE_APPBASE_H

class AppBase {
protected:
    AppBase(int argc, const char *argv[]) : argc(argc), argv(argv) {};
    int argc;
    const char **argv;
public:
    virtual int run() = 0;
};


#endif //V_SHAPE_APPBASE_H
