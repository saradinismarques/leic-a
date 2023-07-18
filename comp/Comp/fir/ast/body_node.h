#ifndef COMPILADORES_BODY_NODE_H
#define COMPILADORES_BODY_NODE_H

#include <cdk/ast/basic_node.h>
#include "ast/block_node.h"

namespace fir {

    /**
     * Class for describing body nodes.
     */

    class body_node: public cdk::basic_node {
        fir::block_node *_prologue;
        fir::block_node *_block;
        fir::block_node *_epilogue;

    public:
        body_node(int lineno, fir::block_node *prologue, fir::block_node *block, fir::block_node *epilogue) :
                cdk::basic_node(lineno), _prologue(prologue), _block(block), _epilogue(epilogue) {}

        fir::block_node *prologue() {
            return _prologue;
        }

        fir::block_node *block() {
            return _block;
        }

        fir::block_node *epilogue() {
            return _epilogue;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_body_node(this, level);
        }
    };
}

#endif
