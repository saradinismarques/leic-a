#ifndef __FIR_AST_PRINT_NODE_H__
#define __FIR_AST_PRINT_NODE_H__

#include <cdk/ast/sequence_node.h>

namespace fir {

    /**
     * Class for describing print nodes.
     */
    class print_node : public cdk::basic_node {
        cdk::sequence_node *_exprs;
        bool _newline;

    public:
        inline print_node(int lineno, cdk::sequence_node *exprs, bool newline = false) :
                cdk::basic_node(lineno), _exprs(exprs), _newline(newline) {
        }

    public:
        inline cdk::sequence_node *exprs() {
            return _exprs;
        }
        inline bool newline() {
            return _newline;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_print_node(this, level);
        }

    };

} // fir

#endif
