#ifndef COMPILADORES_RETURN_NODE_H
#define COMPILADORES_RETURN_NODE_H

#include <cdk/ast/expression_node.h>

namespace fir {
    class return_node : public cdk::basic_node {

    public:
        return_node(int lineno) :
                cdk::basic_node(lineno) {}

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_return_node(this, level);
        }
    };
} // fir

#endif
