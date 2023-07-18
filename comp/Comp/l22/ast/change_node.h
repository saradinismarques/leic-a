#ifndef __L22_AST_BLOCK_H__
#define __L22_AST_BLOCK_H__

#include <cdk/ast/basic_node.h>
#include "targets/basic_ast_visitor.h"

namespace l22 {

  class change_node: public cdk::basic_node {
    cdk::expression_node *_function, *_vector;
    cdk::expression_node *_low, *_high;

  public:
    change_node(int lineno, cdk::expression_node *function, cdk::expression_node *vector, 
                cdk::expression_node *low, cdk::expression_node *high) :
        cdk::basic_node(lineno), _function(function), _vector(vector), _low(low), _high(high) {
    }

  public:
    inline cdk::expression_node *function() {
      return _function;
    }
    inline cdk::expression_node *vector() {
      return _vector;
    }
    inline cdk::expression_node *low() {
      return _low;
    }
    inline cdk::expression_node *high() {
      return _high;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_change_node(this, level);
    }

  };

} // l22

#endif
