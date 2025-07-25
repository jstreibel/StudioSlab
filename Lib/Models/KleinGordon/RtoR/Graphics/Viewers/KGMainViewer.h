//
// Created by joao on 7/10/24.
//

#ifndef STUDIOSLAB_KGMAINVIEWER_H
#define STUDIOSLAB_KGMAINVIEWER_H

#include "Graphics/DataViewers/MainViewer.h"
#include "KGViewer.h"

namespace Slab::Models::KGRtoR {

    class KGMainViewer : public Graphics::MainViewer {
        Slab::Vector<Slab::TPointer<KGViewer>> kg_viewers;

        TPointer<R2toR::FNumericFunction> ddtbase_function;

    protected:
        bool setCurrentViewer(Slab::Index i) override;
        Slab::TPointer<KGViewer>
        getCurrentKGViewer();

    public:
        void addKGViewer(const Slab::TPointer<Slab::Models::KGRtoR::KGViewer>&);

        void
        setFunctionTimeDerivative(TPointer<R2toR::FNumericFunction>);
        virtual TPointer<R2toR::FNumericFunction>
        getFunctionTimeDerivative();
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_KGMAINVIEWER_H
