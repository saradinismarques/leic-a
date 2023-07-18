#ifndef L22_AST_INPUT_NODE_H
#define L22_AST_INPUT_NODE_H

#include <cdk/ast/lvalue_node.h>

namespace l22 {

  /**
   * Class for describing read nodes.
   */
  class input_node: public cdk::basic_node {
    cdk::lvalue_node_argument;

  public:
    inline input_node(int lineno, cdk::lvalue_node argument) :
        cdk::basic_node(lineno), _argument(argument) {
    }

  public:
    inline cdk::lvalue_node *argument() {
      return _argument;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_read_node(this, level);
    }

  };

} // l22

#endif