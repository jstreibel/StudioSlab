//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_OPERATOR_H
#define STUDIOSLAB_OPERATOR_H

namespace Slab::Math {

    template<typename Group>
    class NumericAlgebra;


    template<typename Group>
    class Operator {
        friend class NumericAlgebra<Group>;

        class OperatorActionProxy {
            friend class NumericAlgebra<Group>;
            friend class Operator;

            OperatorActionProxy
            (const Operator &oppy, const Group &elly)
            : O(oppy), g(elly) {};

            const Operator &O;
            const Group &g;
        };

    protected:
        virtual Group&
        operateAndStoreResult
        (Group& output, const Group& input)
        const = 0;

    public:
        virtual ~Operator() = default;

        /**
         *
         * @param elleyOh the group element(s) this operator is acting upon.
         * @return an OperatorActionProxy. The reason for this is that the operation is only executed (only
         * gets CPU time) when its output memory location is provided.
         */
        virtual OperatorActionProxy
        operator *
        (const Group &elleyOh) const
        {
            return OperatorActionProxy(*this, elleyOh);
        };
    };

}

#endif //STUDIOSLAB_OPERATOR_H
