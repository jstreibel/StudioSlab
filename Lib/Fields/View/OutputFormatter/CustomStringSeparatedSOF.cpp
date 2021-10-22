//
// Created by joao on 11/10/2019.
//

#include "CustomStringSeparatedSOF.h"

#include <utility>


CustomStringSeparatedSOF::CustomStringSeparatedSOF(std::string sep) : sep(std::move(sep)) { }

auto CustomStringSeparatedSOF::getFormatDescription() const -> String {
    return String("text");
}

auto CustomStringSeparatedSOF::operator()(const DiscreteSpace &fOut) const -> ByteData {
    const auto &space = fOut;

    const VecFloat& X = space.getHostData(true);

    std::ostringstream oss;

    for(const auto& x : X)
        oss << (std::isnan(x) ? .0 : x) << sep;

    oss << std::endl;

    const auto &s = oss.str();
    ByteData data(s.size());

    std::copy(s.begin(), s.end(), data.begin());

    return data;
}

auto CustomStringSeparatedSOF::operator()(const Real &out) const -> ByteData {
    std::ostringstream oss;

    oss << out << sep;

    auto s = oss.str();

    ByteData data(s.size());
    std::copy(s.begin(), s.end(), data.begin());

    return data;
}