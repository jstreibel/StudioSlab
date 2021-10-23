//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_CUSTOMSTRINGSEPARATEDSOF_H
#define V_SHAPE_CUSTOMSTRINGSEPARATEDSOF_H

#include "OutputFormatterBase.h"

class CustomStringSeparatedSOF : public OutputFormatterBase {
public:
    explicit CustomStringSeparatedSOF(std::string sep = " ");

    auto operator()(const DiscreteSpace &fOut) const -> ByteData override;
    auto operator()(const Real &out) const -> ByteData override;

    auto getFormatDescription() const -> String override;

private:
    const std::string sep;
};


#endif //V_SHAPE_CUSTOMSTRINGSEPARATEDSOF_H