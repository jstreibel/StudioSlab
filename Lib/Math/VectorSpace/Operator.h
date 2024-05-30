//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_OPERATOR_H
#define STUDIOSLAB_OPERATOR_H

namespace Slab::Math {

    template<typename Ty>
    class NumericAlgebra;


    template<typename Ty>
    class Operator {
        friend class NumericAlgebra<Ty>;

        class OperatorActionProxy {
            friend class NumericAlgebra<Ty>;
            friend class Operator;
            OperatorActionProxy(const Operator &oppy, const Ty &elly) : O(oppy), g(elly) {};
            const Operator &O;
            const Ty &g;
        };

    protected:
        virtual Ty& operateAndStoreResult(Ty& output, const Ty& input) const = 0;

    public:
        virtual OperatorActionProxy operator *(const Ty &elleyOh) const {
            return OperatorActionProxy(*this, elleyOh);
        };
    };

}

#endif //STUDIOSLAB_OPERATOR_H
