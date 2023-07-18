#ifndef COMPILADORES_VAR_DECL_NODE_H
#define COMPILADORES_VAR_DECL_NODE_H

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>

namespace fir {
    class var_decl_node : public cdk::typed_node {
        int _access_lvl;
        std::string _identifier;
        cdk::expression_node *_initializer;

    public:
        var_decl_node(int lineno, int access_lvl, std::shared_ptr <cdk::basic_type> var_type,
                                  const std::string &identifier,
                                  cdk::expression_node *initializer) :
                cdk::typed_node(lineno), _access_lvl(access_lvl), _identifier(identifier), _initializer(initializer) {
            type(var_type);
        }

        int access_lvl() {
            return _access_lvl;
        }

        const std::string &identifier() const {
            return _identifier;
        }

        cdk::expression_node *initializer() {
            return _initializer;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_var_decl_node(this, level);
        }

    };
} // fir

#endif
