#ifndef COMPILADORES_BLOCK_NODE_H
#define COMPILADORES_BLOCK_NODE_H

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/basic_node.h>

namespace fir {
    class block_node : public cdk::basic_node {
        cdk::sequence_node *_declarations, *_instructions;

    public:
        block_node(int lineno, cdk::sequence_node *decls, cdk::sequence_node *insts) :
            cdk::basic_node(lineno), _declarations(decls), _instructions(insts) {}

    public:
        cdk::sequence_node *declarations() {
            return _declarations;
        }

        cdk::sequence_node *instructions() {
            return _instructions;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_block_node(this, level);
        }
    };
} // fir

#endif
