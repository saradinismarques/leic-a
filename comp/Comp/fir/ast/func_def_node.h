#ifndef COMPILADORES_FUNC_DEF_NODE_H
#define COMPILADORES_FUNC_DEF_NODE_H

#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/rvalue_node.h>
#include "ast/block_node.h"

namespace fir {

    /**
     * Class describing function definition
     */
    class func_def_node : public cdk::typed_node {
        int _access_lvl;
        std::string _identifier;
        cdk::sequence_node *_arguments;
        cdk::expression_node *_ret_value;
        fir::body_node *_body;

    public:
        func_def_node(int lineno, int access_lvl, std::string &id, cdk::sequence_node *args,
                      fir::body_node *body,
                      cdk::expression_node *ret_value = nullptr) :
                cdk::typed_node(lineno), _access_lvl(access_lvl), _identifier(id), _arguments(args),
                _ret_value(ret_value), _body(body) {
            type(cdk::primitive_type::create(0, cdk::TYPE_VOID));
        }

        func_def_node(int lineno, int access_lvl, std::string &id, cdk::sequence_node *args,
                      std::shared_ptr <cdk::basic_type> func_type, fir::body_node *body,
                      cdk::expression_node *ret_value = nullptr) :
                cdk::typed_node(lineno), _access_lvl(access_lvl), _identifier(id), _arguments(args),
                _ret_value(ret_value), _body(body) {
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

        cdk::expression_node *ret_value() {
            return _ret_value;
        }

        fir::body_node *body() {
            return _body;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_func_def_node(this, level);
        }
    };
} // fir


#endif