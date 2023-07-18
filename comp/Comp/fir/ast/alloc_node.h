#ifndef COMPILADORES_ALLOC_NODE_H
#define COMPILADORES_ALLOC_NODE_H

#include <cdk/ast/unary_operation_node.h>

namespace fir {

    class alloc_node: public cdk::unary_operation_node {
    public:
        alloc_node(int lineno, cdk::expression_node *argument) :
                cdk::unary_operation_node(lineno, argument) {
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_alloc_node(this, level);
        }

    };

} // fir

#endif
