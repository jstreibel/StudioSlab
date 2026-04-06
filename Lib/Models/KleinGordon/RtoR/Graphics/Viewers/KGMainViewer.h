//
// Created by joao on 7/10/24.
//

#ifndef STUDIOSLAB_KGMAINVIEWER_H
#define STUDIOSLAB_KGMAINVIEWER_H

#include "Graphics/DataViewers/MainViewer.h"
#include "KGViewer.h"

namespace Slab::Models::KGRtoR {

    class FKGMainViewer : public Graphics::FMainViewer {
        Slab::Vector<Slab::TPointer<FKGViewer>> kg_viewers;

        TPointer<R2toR::FNumericFunction> ddtbase_function;

    protected:
        bool setCurrentViewer(Slab::Index i) override;
        Slab::TPointer<FKGViewer>
        getCurrentKGViewer();

    public:
        void addKGViewer(const Slab::TPointer<Slab::Models::KGRtoR::FKGViewer>&);

        void
        setFunctionTimeDerivative(TPointer<R2toR::FNumericFunction>);
        virtual TPointer<R2toR::FNumericFunction>
        getFunctionTimeDerivative();
    };

    using KGMainViewer [[deprecated("Use FKGMainViewer")]] = FKGMainViewer;

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_KGMAINVIEWER_H
