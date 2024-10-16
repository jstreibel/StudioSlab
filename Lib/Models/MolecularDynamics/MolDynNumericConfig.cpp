//
// Created by joao on 10/16/24.
//

#include "MolDynNumericConfig.h"

namespace Slab {
    namespace Models {
        namespace MolecularDynamics {
            UInt MolDynNumericConfig::getn() const {
                NOT_IMPLEMENTED
                return 0;
            }

            Str MolDynNumericConfig::to_string() const {
                NOT_IMPLEMENTED
                return Slab::Str();
            }

            auto MolDynNumericConfig::getN() const -> Count {
                NOT_IMPLEMENTED
                return 0;
            }

            auto MolDynNumericConfig::getL() const -> Real {
                NOT_IMPLEMENTED
                return 0;
            }

            auto MolDynNumericConfig::gett() const -> Real {
                NOT_IMPLEMENTED
                return 0;
            }

            auto MolDynNumericConfig::getr() const -> Real {
                return .1;
            }

            auto MolDynNumericConfig::getdt() const -> Real {
                NOT_IMPLEMENTED
                return 0;
            }

            auto MolDynNumericConfig::sett(Real) -> void {
                NOT_IMPLEMENTED;
            }
        } // Slab
    } // Models
} // MolecularDynamics