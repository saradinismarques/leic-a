#ifndef __L22_AST_WRITE_NODE_H__
#define __L22_AST_WRITE_NODE_H__

namespace l22 {

  /**
   * Class for describing write nodes.
   */
  class write_node: public cdk::basic_node {
    cdk::sequence_node *_argument;
    bool _newline = false;

  public:
    write_node(int lineno, cdk::sequence_node *argument, bool newline = false) :
        cdk::basic_node(lineno), _argument(argument), _newline(newline){
    }

  public:
    
    cdk::sequence_node *argument() {
      return _argument;
    }

    bool newline() {
      return _newline;
    }
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_write_node(this, level);
    }

  };

} // l22

#endif
