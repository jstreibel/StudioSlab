//
// Created by joao on 7/10/24.
//

#ifndef STUDIOSLAB_KGMAINVIEWER_H
#define STUDIOSLAB_KGMAINVIEWER_H

#include "Graphics/Viewers/MainViewer.h"
#include "KGViewer.h"

namespace Slab::Models::KGRtoR {

    class KGMainViewer : public Graphics::MainViewer {
        Slab::Vector<Slab::Pointer<KGViewer>> kg_viewers;

        Pointer<R2toR::NumericFunction> ddtbase_function;

    protected:
        bool setCurrentViewer(Slab::Index i) override;
        Slab::Pointer<KGViewer>
        getCurrentKGViewer();

    public:
        void addKGViewer(const Slab::Pointer<Slab::Models::KGRtoR::KGViewer>&);

        void
        setFunctionTimeDerivative(Pointer<R2toR::NumericFunction>);
        virtual Pointer<R2toR::NumericFunction>
        getFunctionTimeDerivative();


    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_KGMAINVIEWER_H
