#ifndef COMPILADORES_NULLPTR_NODE_H
#define COMPILADORES_NULLPTR_NODE_H

#include <cdk/ast/expression_node.h>

namespace fir {
    class nullptr_node : public cdk::expression_node {
    public:
        nullptr_node(int lineno) :
                cdk::expression_node(lineno) {
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_nullptr_node(this, level);
        }

    };
} // fir

#endif
