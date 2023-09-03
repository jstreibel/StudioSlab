//
// Created by joao on 11/10/2019.
//

#include "CustomStringSeparatedSOF.h"

#include <utility>


CustomStringSeparatedSOF::CustomStringSeparatedSOF(std::string sep) : sep(std::move(sep)) { }

auto CustomStringSeparatedSOF::getFormatDescription() const -> Str {
    return Str("text");
}

auto CustomStringSeparatedSOF::operator()(const DiscreteSpace &fOut) const -> Numerics::ByteData {
    const auto &space = fOut;

    const RealArray& X = space.getHostData(true);

    std::ostringstream oss;

    for(const auto& x : X)
        oss << (std::isnan(x) ? .0 : x) << sep;

    oss << std::endl;

    const auto &s = oss.str();
    Numerics::ByteData data(s.size());

    std::copy(s.begin(), s.end(), data.begin());

    return data;
}

auto CustomStringSeparatedSOF::operator()(const Real &out) const -> Numerics::ByteData {
    std::ostringstream oss;

    oss << out << sep;

    auto s = oss.str();

    Numerics::ByteData data(s.size());
    std::copy(s.begin(), s.end(), data.begin());

    return data;
}

bool CustomStringSeparatedSOF::isBinary() const {
    return false;
}
