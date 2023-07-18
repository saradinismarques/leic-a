#ifndef COMPILADORES_INDEX_NODE_H
#define COMPILADORES_INDEX_NODE_H

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/expression_node.h>

namespace fir {

    /**
     * Class for describing index nodes.
     */
    class index_node: public cdk::lvalue_node {
        cdk::expression_node *_base;
        cdk::expression_node *_index;

    public:
        index_node(int lineno, cdk::expression_node *base, cdk::expression_node *index) :
                cdk::lvalue_node(lineno), _base(base), _index(index) {
        }

        cdk::expression_node* base() {
            return _base;
        }
        cdk::expression_node* index() {
            return _index;
        }

    public:
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_index_node(this, level);
        }

    };

} // fir

#endif
