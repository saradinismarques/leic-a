#ifndef COMPILADORES_ADDRESSOF_NODE_H
#define COMPILADORES_ADDRESSOF_NODE_H

#include <cdk/ast/expression_node.h>

namespace fir {
    class addressof_node: public cdk::expression_node {
        cdk::lvalue_node *_lvalue;

    public:
        addressof_node(int lineno, cdk::lvalue_node *argument) :
                cdk::expression_node(lineno), _lvalue(argument) {
        }

        cdk::lvalue_node* lvalue() {
            return _lvalue;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_addressof_node(this, level);
        }

    };
}

#endif
