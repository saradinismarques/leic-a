#ifndef COMPILADORES_FUNC_CALL_NODE_H
#define COMPILADORES_FUNC_CALL_NODE_H

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace fir {
    class func_call_node : public cdk::expression_node {
        std::string _identifier;
        cdk::sequence_node *_arguments;

    public:
        func_call_node(int lineno, std::string &identifier, cdk::sequence_node *arguments) :
                cdk::expression_node(lineno), _identifier(identifier), _arguments(arguments) {}

        const std::string &identifier() {
            return _identifier;
        }

        cdk::sequence_node *arguments() {
            return _arguments;
        }

        cdk::expression_node *argument(size_t ix) {
            return dynamic_cast<cdk::expression_node *>(_arguments->node(ix));
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_func_call_node(this, level);
        }
    };
}

#endif
