#ifndef COMPILADORES_FUNC_DECL_NODE_H
#define COMPILADORES_FUNC_DECL_NODE_H

#include <string>
#include <cdk/ast/literal_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/typed_node.h>

namespace fir {
    /**
     * Class describing function declaration
     */
    class func_decl_node : public cdk::typed_node {
        int _access_lvl;
        std::string _identifier;
        cdk::sequence_node *_arguments;

    public:
        func_decl_node(int lineno, int access_lvl, const std::string &id, cdk::sequence_node *args) :
                cdk::typed_node(lineno), _access_lvl(access_lvl), _identifier(id), _arguments(args) {
            type(cdk::primitive_type::create(0, cdk::TYPE_VOID));
        }

        func_decl_node(int lineno, int access_lvl, const std::string &id, cdk::sequence_node *args,
                       std::shared_ptr <cdk::basic_type> func_type) :
                cdk::typed_node(lineno), _access_lvl(access_lvl), _identifier(id), _arguments(args) {
            type(func_type);
        }

    public:
        int access_lvl() {
            return _access_lvl;
        }

        const std::string &identifier() {
            return _identifier;
        }

        cdk::typed_node *argument(size_t ix) {
            return dynamic_cast<cdk::typed_node *>(_arguments->node(ix));
        }

        cdk::sequence_node *arguments() {
            return _arguments;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_func_decl_node(this, level);
        }
    };

} // fir

#endif
