#ifndef __L22_AST_FUNCTION_DEFINITION_H__
#define __L22_AST_FUNCTION_DEFINITION_H__

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/block_node.h"

namespace l22 {

    class function_definition_node: public cdk::expression_node {
        int _qualifier;
        std::string _identifier;
        cdk::sequence_node *_arguments;
        l22::block_node *_block;

    public:
        function_definition_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> funType, const std::string &identifier, cdk::sequence_node *arguments, l22::block_node *block) :
            cdk::expression_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _block(block) {
          type(funType);
        }

    public:
        int qualifier() {
          return _qualifier;
        }
        const std::string& identifier() const {
          return _identifier;
        }
        cdk::sequence_node* arguments() {
          return _arguments;
        }
        cdk::expression_node* argument(size_t ax) {
          return dynamic_cast<cdk::expression_node*>(_arguments->node(ax));
        }
        l22::block_node* block() {
          return _block;
        }

        void accept(basic_ast_visitor *sp, int level) {
          sp->do_function_definition_node(this, level);
        }

    };

} // l22

#endif
