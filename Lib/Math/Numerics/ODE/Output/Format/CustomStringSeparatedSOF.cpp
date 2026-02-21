//
// Created by joao on 11/10/2019.
//

#include "CustomStringSeparatedSOF.h"

#include <utility>


namespace Slab::Math {

    FCustomStringSeparatedSOF::FCustomStringSeparatedSOF(std::string sep) : sep(std::move(sep)) {}

    auto FCustomStringSeparatedSOF::getFormatDescription() const -> Str {
        return Str("text");
    }

    auto FCustomStringSeparatedSOF::operator()(const DiscreteSpace &fOut) const -> ByteData {
        const auto &space = fOut;

        const RealArray &X = space.getHostData(true);

        std::ostringstream oss;

        for (const auto &x: X)
            oss << (std::isnan(x) ? .0 : x) << sep;

        oss << std::endl;

        const auto &s = oss.str();
        ByteData data(s.size());

        std::copy(s.begin(), s.end(), data.begin());

        return data;
    }

    auto FCustomStringSeparatedSOF::operator()(const DevFloat &out) const -> ByteData {
        std::ostringstream oss;

        oss << out << sep;

        auto s = oss.str();

        ByteData data(s.size());
        std::copy(s.begin(), s.end(), data.begin());

        return data;
    }

    bool FCustomStringSeparatedSOF::isBinary() const {
        return false;
    }


}
