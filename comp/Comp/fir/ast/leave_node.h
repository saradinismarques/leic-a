#ifndef COMPILADORES_LEAVE_NODE_H
#define COMPILADORES_LEAVE_NODE_H

#include <cdk/ast/basic_node.h>

namespace fir {
    class leave_node : public cdk::basic_node {
        int _lvls;

    public:
        leave_node(int lineno, int lvls = 1) : cdk::basic_node(lineno), _lvls(lvls) {}

        int lvls() const {
            return _lvls;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_leave_node(this, level);
        }
    };
} // fir

#endif
