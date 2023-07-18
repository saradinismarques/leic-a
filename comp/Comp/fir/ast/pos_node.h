#ifndef COMPILADORES_IDENTITY_NODE_H
#define COMPILADORES_IDENTITY_NODE_H

#include <cdk/ast/unary_operation_node.h>

namespace fir {

    /**
     * Class for describing identity nodes.
     */
    class pos_node : public cdk::unary_operation_node {

    public:
        inline pos_node(int lineno, expression_node* expr) :
                cdk::unary_operation_node(lineno, expr) {
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_pos_node(this, level);
        }

    };

} // fir

#endif
