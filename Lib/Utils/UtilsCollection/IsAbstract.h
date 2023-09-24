//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_ISABSTRACT_H
#define STUDIOSLAB_ISABSTRACT_H

template <typename T>
struct IsAbstract {
private:
    template <typename C>
    static char test(decltype(&C::foo)) { return char(0); };

    template <typename C>
    static int test(...) { return 0; };

public:
    static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

#endif //STUDIOSLAB_ISABSTRACT_H
